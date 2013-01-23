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
#include "guild_manager.hpp"
#include "union_manager.hpp"

/// SOL INCLUDES
#include <player.hpp>
#include <guild.hpp>
#include <guild_member.hpp>
#include <union.hpp>

/// EPL INCLUDES
#include <packet_msg.hpp>
#include <packet_invitation.hpp>
#include <packet_alliance.hpp>
#include <packet_guild.hpp>

/// SRDL INCLUDES
#include <guild_query.hpp>

int StateGame::OnAllianceInvite (IPacket &packet)
{
    uint32_t playerID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Guild> master_guild = player->get_guild();

    if (!master_guild || !master_guild->is_guild_master(player->get_name()))
        return MSG_ERROR_ORDER;

    boost::shared_ptr<Player> inv_player = m_server->GetWorld()->FindPlayer_ByID(playerID);

    if (!inv_player)
        return MSG_SUCCESS;

    boost::shared_ptr<OPacket> pkt(new OPacket);
    boost::shared_ptr<Guild> inv_guild = inv_player->get_guild();

    if (!inv_guild)
    {
        syslog(LOG_INFO,"Player dont have a guild.");
        return MSG_SUCCESS;
    }

    if (!inv_guild->is_guild_master(inv_player->get_name()))
    {
        srv_pkt::AllianceInvite(pkt,srv_pkt::ERROR_ALLIANCE_PLAYER_NOT_AUTHORIZED);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    if (master_guild->get_id() == inv_guild->get_id())
    {
        srv_pkt::AllianceInvite(pkt,srv_pkt::ERROR_ALLIANCE_GUILD_SAME);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    if (master_guild->get_level() < 2)
    {
        srv_pkt::AllianceInvite(pkt,srv_pkt::ERROR_ALLIANCE_MASTER_GUILD_LEVEL);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    if (inv_guild->get_level() < 2)
    {
        srv_pkt::AllianceInvite(pkt,srv_pkt::ERROR_ALLIANCE_SLAVE_GUILD_LEVEL);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    if (inv_guild->get_union_id())
    {
        if (master_guild->get_union_id() == inv_guild->get_union_id())
        {
            srv_pkt::AllianceInvite(pkt,srv_pkt::ERROR_ALLIANCE_SAME);
            m_connection->Send(pkt);
            return MSG_SUCCESS;
        }
        else
        {
            /// ALREADY HAVE AN UNION!!!
            return MSG_SUCCESS;
        }
    }

    if (master_guild->get_union_id())
    {
        boost::shared_ptr<Union> masterUnion;
        masterUnion = m_server->GetWorld()->GetUnionManager()->Find(master_guild->get_union_id());

        if (masterUnion->full())
        {
            srv_pkt::AllianceInvite(pkt,srv_pkt::ERROR_ALLIANCE_FULL);
            m_connection->Send(pkt);
            return MSG_SUCCESS;
        }
    }

    inv_player->set_invitation(INVITATION_UNION);
    inv_player->set_invitation_player(player->GetUniqueID());

    srv_pkt::Invite(pkt,INVITATION_UNION,player->GetUniqueID());
    m_server->Send(pkt,inv_player->ConnectionID());

    return MSG_SUCCESS;
}

int StateGame::OnAllianceSecede (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild || !guild->get_union_id() || !guild->is_guild_master(player->get_name()))
        return MSG_ERROR_ORDER;

    boost::shared_ptr<Union> ally = m_server->GetWorld()->GetUnionManager()->Find(guild->get_union_id());

    if (!ally)
        return MSG_ERROR_ORDER;

    if (ally->is_leading_guild(guild->get_id()))
    {
        if (ally->size() > UNION_MIMINUM_GUILD_COUNT)
        {
            syslog(LOG_INFO,"Cant disband union because ur the leading guild.");
            return MSG_SUCCESS;
        }
    }

    if (!ally->remove(guild->get_id(),UG_REASON_SECEDE))
        return MSG_ERROR_ARG;

    DB::GUILD::Union query;

    if (ally->size() < UNION_MIMINUM_GUILD_COUNT)
    {
        std::vector<boost::shared_ptr<Guild> > glist = ally->guild_list();

        Union::const_iterator iter = glist.begin();

        ally->remove((*iter)->get_id(),UG_REASON_SECEDE);

        query(m_server->DBConnection(),(*iter)->get_id(),(*iter)->get_union_id());

        m_server->GetWorld()->GetUnionManager()->Remove(ally->id());
    }

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::AllianceSecede(pkt);
    m_connection->Send(pkt);

    query(m_server->DBConnection(),guild->get_id(),guild->get_union_id());

    return MSG_SUCCESS;
}

int StateGame::OnAllianceExpel (IPacket &packet)
{
    uint32_t guildID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild || !guild->get_union_id() || !guild->is_guild_master(player->get_name()))
        return MSG_ERROR_ORDER;

    if (guild->get_id() == guildID)
    {
        syslog(LOG_INFO,"Cant ban urself.");
        return MSG_SUCCESS;
    }

    boost::shared_ptr<Union> ally = m_server->GetWorld()->GetUnionManager()->Find(guild->get_union_id());

    if (!ally)
        return MSG_ERROR_ORDER;

    if (!ally->remove(guildID,UG_REASON_EXPEL))
        return MSG_ERROR_ARG;

    DB::GUILD::Union query;
    query(m_server->DBConnection(),guildID,0);

    if ( ally->size() < UNION_MIMINUM_GUILD_COUNT)
    {
        ally->remove(guild->get_id(),UG_REASON_SECEDE);

        query(m_server->DBConnection(),guild->get_id(),0);

        m_server->GetWorld()->GetUnionManager()->Remove(ally->id());
    }

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::AllianceExpel(pkt);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

void StateGame::ConnectAllianceSignals ()
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild)
        return;

    boost::shared_ptr<Union> ally = m_server->GetWorld()->GetUnionManager()->Find(guild->get_union_id());

    if (ally)
    {
        ally->signal_insert.connect(boost::bind(&StateGame::OnAllianceEvent_GuildInsert,this,_1));
        ally->signal_level.connect(boost::bind(&StateGame::OnAllianceEvent_GuildLevel,this,_1,_2));
        ally->signal_member.connect(boost::bind(&StateGame::OnAllianceEvent_GuildMember,this,_1,_2));
        ally->signal_msg.connect(boost::bind(&StateGame::OnAllianceEvent_Msg,this,_1,_2));
        ally->signal_remove.connect(boost::bind(&StateGame::OnAllianceEvent_GuildRemove,this,_1,_2));
        ally->signal_disband.connect(boost::bind(&StateGame::OnAllianceEvent_Disband,this,_1));
        ally->signal_leader.connect(boost::bind(&StateGame::OnAllianceEvent_GuildLeader,this,_1));
    }
}

void StateGame::DisconnectAllianceSignals ()
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild)
        return;

    boost::shared_ptr<Union> ally = m_server->GetWorld()->GetUnionManager()->Find(guild->get_union_id());

    if (ally)
    {
        ally->signal_insert.disconnect(boost::bind(&StateGame::OnAllianceEvent_GuildInsert,this,_1));
        ally->signal_level.disconnect(boost::bind(&StateGame::OnAllianceEvent_GuildLevel,this,_1,_2));
        ally->signal_member.disconnect(boost::bind(&StateGame::OnAllianceEvent_GuildMember,this,_1,_2));
        ally->signal_msg.disconnect(boost::bind(&StateGame::OnAllianceEvent_Msg,this,_1,_2));
        ally->signal_remove.disconnect(boost::bind(&StateGame::OnAllianceEvent_GuildRemove,this,_1,_2));
        ally->signal_disband.disconnect(boost::bind(&StateGame::OnAllianceEvent_Disband,this,_1));
        ally->signal_leader.disconnect(boost::bind(&StateGame::OnAllianceEvent_GuildLeader,this,_1));
    }
}

void StateGame::OnAllianceEvent_GuildInsert (const uint32_t guildID)
{
    boost::shared_ptr<Guild> guild = m_server->GetWorld()->GetGuildManager()->Find(guildID);

    if (guild)
    {
//        boost::shared_ptr<OPacket> pkt(new OPacket);
//        srv_pkt::GuildNotify_UnionJoin(pkt,guild);
//        m_connection->Send(pkt);
    }
}

void StateGame::OnAllianceEvent_GuildMember (const uint32_t guildID, const uint8_t count)
{
    boost::shared_ptr<Guild> guild = m_server->GetWorld()->GetGuildManager()->Find(guildID);

    if (guild)
    {
        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::GuildNotify_UnionMemberCount(pkt,guildID,count);
        m_connection->Send(pkt);
    }
}

void StateGame::OnAllianceEvent_GuildLeader (const uint32_t guildID)
{
    boost::shared_ptr<Guild> guild = m_server->GetWorld()->GetGuildManager()->Find(guildID);

    if (guild)
    {
        boost::shared_ptr<GuildMember> member = guild->get_master();

        if (member)
        {
            boost::shared_ptr<OPacket> pkt(new OPacket);
            srv_pkt::GuildNotify_UnionMemberLeader(pkt,guildID,member->Name,member->refCharID);
            m_connection->Send(pkt);
        }
    }
}

void StateGame::OnAllianceEvent_GuildLevel (const uint32_t guildID, const uint8_t level)
{
    boost::shared_ptr<Guild> guild = m_server->GetWorld()->GetGuildManager()->Find(guildID);

    if (guild)
    {
        uint8_t count = 0;

        switch(level)
        {
        case 1:
            count = 15;
            break;
        case 2:
            count = 20;
            break;
        case 3:
            count = 25;
            break;
        case 4:
            count = 35;
            break;
        case 5:
            count = 50;
            break;
        }

        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::GuildNotify_UnionMemberMax(pkt,guildID,count);
        m_connection->Send(pkt);
    }
}

void StateGame::OnAllianceEvent_Msg (const std::string &name, const std::string &msg)
{
    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::Msg(pkt,srv_pkt::MSG_UNION,name,msg);
    m_connection->Send(pkt);
}

void StateGame::OnAllianceEvent_GuildRemove (const uint32_t guildID, const uint8_t reason)
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (player)
    {
        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::GuildNotify_UnionRemove(pkt,guildID,reason);
        m_connection->Send(pkt);

        boost::shared_ptr<Guild> guild = player->get_guild();

        if (guild && guild->get_id() == guildID)
        {
            pkt.reset(new OPacket);

            srv_pkt::GuildNotify_UnionClear(pkt);
            m_connection->Send(pkt);

            DisconnectAllianceSignals();
        }
    }
}

void StateGame::OnAllianceEvent_Disband (const uint32_t guildID)
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (player)
    {
        boost::shared_ptr<Guild> guild = player->get_guild();

        if (guild && guild->get_id() != guildID)
        {
            boost::shared_ptr<OPacket> pkt(new OPacket);
            srv_pkt::GuildNotify_UnionClear(pkt);
            m_connection->Send(pkt);

            DisconnectAllianceSignals();
        }
    }
}
