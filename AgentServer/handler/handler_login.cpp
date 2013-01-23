/*********************************************************************************
 *
 * This file is part of eSRO.
 *
 * eSRO is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * eSRO is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright ⓒ 2013  Rafael Dominguez (npcdoom)
 *
 *********************************************************************************/

#include "server.hpp"
#include "server_connection.hpp"
#include "server_state_game.hpp"

#include "world.hpp"
#include "manager/academy_manager.hpp"
#include "manager/guild_manager.hpp"
#include "manager/fortress_manager.hpp"
#include "manager/friend_manager.hpp"
#include "manager/skill_manager.hpp"
#include "manager/union_manager.hpp"

/// SOL INCLUDES
#include <skill.hpp>
#include <player.hpp>
#include <transport.hpp>
#include <guild.hpp>
#include <union.hpp>

/// EPL INCLUDES
#include <packet_guild.hpp>
#include <packet_alliance.hpp>
#include <packet_skill.hpp>
#include <packet_player.hpp>
#include <packet_fortress.hpp>
#include <packet_friend.hpp>
#include <packet_academy.hpp>
#include <packet_transport.hpp>
#include <packet_party.hpp>

/// SQL INCLUDES
#include <academy_query.hpp>
#include <player_query.hpp>

int StateGame::OnLoad (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    World *world = m_server->GetWorld();

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    player->ConnectionID(m_connection->ID());

    boost::shared_ptr<OPacket> pkt(new OPacket);
//
//    /*DB::FORTRESS::Select fortress_query;
//    std::vector<Fortress> fort_list = fortress_query(m_db_conn);
//
//    if (!fort_list.empty())
//        send(packet::FortressInfo(fort_list));
//    */
//
    /// CURRENT ACTIVE BUFFS
    std::map<uint32_t,Buff> buff_list = player->get_buff_list();

    for (std::map<uint32_t,Buff>::const_iterator iter = buff_list.begin(); iter != buff_list.end(); ++iter)
    {
        if (!iter->second.passive)
        {
            srv_pkt::InitBuff(pkt,player->GetUniqueID(),iter->second.SkillID,iter->second.CastID);
            m_connection->Send(pkt);

            pkt.reset(new OPacket);

            uint32_t time = m_server->GetWorld()->GetBackgroundTaskTime(iter->second.taskID)*1000;

            srv_pkt::BuffTimeout(pkt,iter->second.CastID,time);
            m_connection->Send(pkt);

            pkt.reset(new OPacket);
        }
    }

    /// READ PERSISTENT SKILL

    DB::PLAYER::Buffs buff_query;
    std::vector<std::pair<uint32_t,uint32_t> > persistent_buff = buff_query(m_server->DBConnection(),player->ID());;

    boost::shared_ptr<Skill> skill;
    for (size_t i = 0; i < persistent_buff.size(); ++i)
    {
        skill = world->GetSkillManager()->find(persistent_buff[i].first);

        if (skill)
        {
            Buff buff;
            buff.CastID = rand();
            buff.SkillID = skill->id();
            buff.GroupID = skill->group_id();
            buff.Level = skill->level();
            buff.duration = persistent_buff[i].second;
            buff.persistent = true;
            buff.passive = false;
            buff.Effects = skill->m_buff_list;

            buff.ReqShield = skill->require_shield();
            buff.ReqDevil = skill->require_devil();
            buff.ReqWeapon = skill->required_weapons();

            if (buff.duration)
            {
                BTask task;
                task.task = BG_TASK_BUFF;
                task.entity = player;
                task.persistent = false;
                task.seconds = buff.duration/1000;
                task.arg = buff.SkillID;

                buff.taskID = world->InitBackgroundTask(task);
            }

            player->insert_buff(buff);

            srv_pkt::InitBuff(pkt,player->GetUniqueID(),skill->id(),buff.CastID);
            m_server->SendChannel(pkt,player->get_position().get_zone());

            pkt.reset(new OPacket);
        }
    }

    DB::PLAYER::RemoveBuffs rbuff_query;
    rbuff_query(m_server->DBConnection(),player->ID());

    /// LOAD AND SEND GUILD INFORMATION

    if (player->get_guild_id())
    {
        boost::shared_ptr<GuildManager> guild_mgr = world->GetGuildManager();

        boost::shared_ptr<Guild> guild = guild_mgr->Find(player->get_guild_id());

        if (!guild)
        {
            guild = guild_mgr->LoadGuild(player->get_guild_id());

            if (!guild)
            {
#if defined DEBUG
                syslog(LOG_INFO,"Guild with ID = %i dont exist.",player->get_guild_id());
#endif
                return MSG_ERROR;
            }
        }

        player->set_guild(guild);

        if (!m_hooked)
        {
            connect_guild_signals();
            m_hooked = true;
        }

        srv_pkt::GuildBegin(pkt);
        m_connection->Send(pkt);

        pkt.reset(new OPacket);

        srv_pkt::GuildInfo(pkt,guild);
        m_connection->Send(pkt);

        pkt.reset(new OPacket);

        srv_pkt::GuildEnd(pkt);
        m_connection->Send(pkt);

        Guild::const_iterator member_iter = guild->find(player->get_name());

        if (member_iter == guild->end())
        {
#if defined DEBUG
            syslog(LOG_INFO,"The player %s, dont belong to this guild.",player->get_name().c_str());
#endif
            return MSG_ERROR;
        }

        pkt.reset(new OPacket);

        srv_pkt::GuildPlayer(pkt,player->GetUniqueID(),guild->get_name(),member_iter->second,true);
        m_connection->Send(pkt);

        guild->member_state(player->get_name(),false);

        /// LOAD AND SEND UNION INFORMATION

        if (guild->get_union_id())
        {
            boost::shared_ptr<Union> ally = world->GetUnionManager()->Find(guild->get_union_id());

            if (!ally)
                ally = world->GetUnionManager()->Load(guild->get_union_id());

            if (!ally)
            {
#if defined DEBUG
                syslog(LOG_INFO,"Alliance (%i) dont exists.",guild->get_union_id());
#endif
                return MSG_ERROR;
            }

            pkt.reset(new OPacket);

            srv_pkt::AllianceList(pkt,ally);
            m_connection->Send(pkt);
        }
    }

    world->GetFriendManager()->loadFriendbook(player->ID());

    FriendBook fbook = world->GetFriendManager()->findFriendbook(player->ID());

    if (!fbook.friends.empty())
    {
        boost::shared_ptr<Player> fplayer;

        pkt.reset(new OPacket);

        srv_pkt::FriendList(pkt,fbook);
        m_connection->Send(pkt);

        for (FriendBook::friend_const_iterator i = fbook.friends.begin(); i != fbook.friends.end(); ++i)
        {
            if (i->status)
            {
                fplayer = world->FindPlayer_ByName(i->Name);

                if (fplayer)
                {
                    pkt.reset(new OPacket);

                    srv_pkt::FriendNotify(pkt,player->ID(),srv_pkt::FRIEND_ONLINE);
                    m_server->Send(pkt,player->ConnectionID());
                }
            }
        }
    }

    pkt.reset(new OPacket);

    srv_pkt::PlayerStats(pkt,player);
    m_connection->Send(pkt);

    /*DB::ACADEMY::Check acd_chk_query;

    if (acd_chk_query(m_db_conn,m_player->get_name()))
    {
            DB::ACADEMY::Select acd_sl_query;
            Academy acd = acd_sl_query(m_db_conn,m_player->get_name());

            send(packet::AcademyInfo(acd));
    }*/

    pkt.reset(new OPacket);

    /// SEND 2 UNKNOWN PACKETS
    pkt->WriteOpcode(0x3D8C);
    pkt->Write<uint8_t>(0);

    m_connection->Send(pkt);

    pkt.reset(new OPacket);

    pkt->WriteOpcode(0x3288);
    pkt->Write<uint16_t>(0);

    m_connection->Send(pkt);

    uint32_t silk = static_cast<srv::Connection*>(m_connection)->Silk();

    pkt.reset(new OPacket);

    srv_pkt::PlayerSilk(pkt,silk);
    m_connection->Send(pkt);

    connect_player_signals();

    world->InsertPlayer(player);

    /// SPAWN TRANSPORT IF IT HAS ONE

    boost::shared_ptr<Transport> transport = player->m_transport;

    if (transport)
    {
        m_server->GetWorld()->InsertNPC(transport);

        pkt.reset(new OPacket);

        srv_pkt::TransportStats(pkt,transport,player->GetUniqueID());
        m_connection->Send(pkt);

        transport->Mount();

        pkt.reset(new OPacket);

        srv_pkt::TransportAction(pkt,player->GetUniqueID(),srv_pkt::TRANSPORT_ACTION_MOUNT,transport->GetUniqueID());
        m_server->SendChannel(pkt,player->get_position().get_zone());
    }

    boost::shared_ptr<Party> party = player->get_party();

    if (party)
    {
        pkt.reset(new OPacket);

        if (pkt)
        {
            srv_pkt::PartyInfo(pkt,party);
            m_connection->Send(pkt);
        }
    }

    /// ONLY SEND NOTIFICATION AFTER THE FIRST TIME U LOGGED TO THE GAME
    if (m_hooked && player->get_guild_id())
    {
        boost::shared_ptr<Guild> guild = player->get_guild();

        guild->member_zone(player->get_name(),player->get_position().get_zone());
    }

    m_server->GetWorld()->InitTaskIdle(player->GetUniqueID());

    return MSG_SUCCESS;
}
