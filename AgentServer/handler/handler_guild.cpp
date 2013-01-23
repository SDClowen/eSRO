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
#include "npc_util.hpp"

/// SRLF INCLUDES
#include <player.hpp>
#include <party.hpp>
#include <guild.hpp>
#include <guild_member.hpp>
#include <union.hpp>

/// SRDL INCLUDES
#include <player_query.hpp>
#include <guild_query.hpp>
#include <guild_member_query.hpp>

/// SRNL INCLUDES
#include <packet_guild.hpp>
#include <packet_alliance.hpp>
#include <packet_invitation.hpp>

#include <syslog.h>
#include <packet_util_functions.hpp>

#define INITIAL_GUILD_LEVEL 1
#define MIN_REQUIRED_LEVEL 20

int StateGame::OnGuildCreate (IPacket &packet)
{
    uint32_t npcID = packet.Read<uint32_t>();

    std::string name = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    World *world = m_server->GetWorld();

    boost::shared_ptr<NPC> npc = world->FindNPC_CloseToPosition(npcID,player->get_position());

    if (!npc || !IsGuildManager(npc->get_id()))
        return MSG_ERROR_ARG;

    if (player->level() < MIN_REQUIRED_LEVEL)
    {
#if defined DEBUG
        syslog(LOG_INFO,"Not the required level.");
#endif
        return MSG_SUCCESS;
    }

    boost::shared_ptr<GuildManager> mgr = world->GetGuildManager();

    if (!mgr->IsValidName(name))
    {
#if defined DEBUG
        syslog(LOG_INFO,"Name alredy in use.");
#endif
        //send(packet::GuildCreate(packet::ERROR_GUILD_INVALID_GUILD_NAME));

        return MSG_SUCCESS;
    }

    std::pair<uint16_t,uint16_t> feed = mgr->GetPromotePolicy(INITIAL_GUILD_LEVEL);

    boost::shared_ptr<Guild> guild = mgr->CreateGuild(name);

    if (!guild)
    {
        syslog(LOG_INFO,"StateGame::OnGuildCreate - Unable to allocate Guild object.");
        return MSG_SUCCESS;
    }

    boost::shared_ptr<GuildMember> member = mgr->CreateMember(guild->get_id(),player->level(),player->get_id(),
        true, player->get_name(),player->get_position().get_zone());

    if (!member)
    {
        syslog(LOG_INFO,"StateGame::OnGuildCreate - Unable to allocate GuildMember object.");
        return MSG_SUCCESS;
    }

    if (!player->spend_gold(feed.first))
    {
#if defined DEBUG
        syslog(LOG_INFO,"Not enough gold to create guild.");
#endif
        return MSG_SUCCESS;
    }

    guild->insert(member);
    player->set_guild(guild);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::GuildCreate(pkt,guild);
        m_connection->Send(pkt);
    }

    pkt.reset(new OPacket);

    if (pkt)
    {
        srv_pkt::GuildPlayer(pkt,player->GetUniqueID(),guild->get_name(),member,true);
        m_server->SendChannel(pkt,player->get_position().get_zone());
    }

    connect_guild_signals();

    guild->member_state(player->get_name(),false);

    boost::shared_ptr<Party> party = player->get_party();

    if (party)
        party->SendGuildNotification(player->GetUniqueID(),name);

    DB::PLAYER::GuildID query;
    query(m_server->DBConnection(),player->ID(),guild->get_id());

    return MSG_SUCCESS;
}

int StateGame::OnGuildDonate (IPacket &packet)
{
    uint32_t gp_donated = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild)
        return MSG_ERROR_ORDER;

    Guild::const_iterator i = guild->find(player->get_name());

    if ( i == guild->end() )
        return MSG_ERROR_ORDER;

    if (!player->spend_sp(gp_donated))
        return MSG_ERROR_ARG;

    if (!guild->donate_gp(player->get_name(),gp_donated))
        return MSG_ERROR_ORDER;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::GuildDonateGP(pkt,gp_donated);
        m_connection->Send(pkt);
    }

    DB::GUILD::GP guild_query;
    guild_query(m_server->DBConnection(),guild->get_id(),guild->get_gp());

    DB::GUILD_MEMBER::GP member_query;
    member_query(m_server->DBConnection(),i->second->ID,i->second->GP);

    return MSG_SUCCESS;
}

int StateGame::OnGuildNotice (IPacket &packet)
{
    std::string title = packet.Read();

    std::string msg = packet.Read();

    if (title.length() > 131)
        return MSG_ERROR_ARG;

    if (msg.length() > 1024)
        return MSG_ERROR_ARG;

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild)
        return MSG_ERROR_ORDER;

    Guild::const_iterator i = guild->find(player->get_name());

    if ( i == guild->end() )
        return MSG_ERROR_ORDER;

    /// CHECK THAT THE PLAYER HAVE THE PERMISSION TO CHANGE NOTICE
    if (i->second->Rights[NOTICE_RIGHTS])
    {
        guild->set_notice(title,msg);

        boost::shared_ptr<OPacket> pkt(new OPacket);

        if (pkt)
        {
            srv_pkt::GuildNotice(pkt);
            m_connection->Send(pkt);
        }

        DB::GUILD::Notice query;
        query(m_server->DBConnection(),guild->get_id(),title,msg);
    }
    else
        return MSG_ERROR_ORDER;

    return MSG_SUCCESS;
}

int StateGame::OnGuildGrantName (IPacket &packet)
{
    uint32_t memberID = packet.Read<uint32_t>();

    std::string grant = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    if (grant.length() > 12)
        return MSG_ERROR_ARG;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Guild> guild = player->get_guild();

    /// CHECK THAT WE ARE IN A GUILD AND THAT THE GUILD IS ATLEAST LEVEL 3
    if (!guild || guild->get_level() < 3)
        return MSG_ERROR_ORDER;

    /// CHECK THAT IS THE GUILD MASTER
    Guild::const_iterator i = guild->find(player->get_name());

    if (!i->second->Master)
        return MSG_ERROR_ORDER;

    guild->set_member_grantname(memberID,grant);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::GuildGrant(pkt,guild->get_id(),memberID,grant);
        m_connection->Send(pkt);
    }

    return MSG_SUCCESS;
}

int StateGame::OnGuildPosition (IPacket &packet)
{
    uint32_t memberID = packet.Read<uint32_t>();

    uint8_t position = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Guild> guild = player->get_guild();

    /// TODO IMPLEMENT THE ERROR CHECK!!
    if (!guild || !guild->member_fortress_position(memberID,position))
        return MSG_ERROR_ORDER;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::GuildPosition(pkt,guild->get_id(),memberID,position);
        m_connection->Send(pkt);
    }

    DB::GUILD_MEMBER::Position query;
    query(m_server->DBConnection(),memberID,position);

    /*

    {Position Full}
    [S -> C][B0B6][2 bytes]
    02 59

    {Has That Position Only}
    [S -> C][B0B6][2 bytes]
    02 5A

    {Do not Own Fortress to Give Position}
    [S -> C][B0B6][2 bytes]
    02 5B

    {Member has No Position Already}
    [S -> C][B0B6][2 bytes]
    02 5C
    */

    return MSG_SUCCESS;
}

int StateGame::OnGuildAuthority (IPacket &packet)
{
    uint8_t memberCount = packet.Read<uint8_t>();

    if (!memberCount)
        return MSG_ERROR_ARG;

    boost::shared_array<uint32_t> memberID(new uint32_t[memberCount]);

    boost::shared_array<uint32_t> authority(new uint32_t[memberCount]);

    for (int i = 0; i < memberCount; ++i)
    {
        memberID[i] = packet.Read<uint32_t>();

        authority[i] = packet.Read<uint32_t>(); /// 1ST BYTE IS AUTHORITY 1BIT EACH TOTAL 5BITS
    }

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild)
        return MSG_ERROR_ORDER;

    Guild::const_iterator iter = guild->find(player->get_name());

    if (iter == guild->end() || !iter->second->Master)
        return MSG_ERROR_ORDER;

    if(!guild->member_authority(memberCount,memberID,authority))
        return MSG_ERROR_ARG;

    DB::GUILD_MEMBER::Authority query;
    for (int i = 0; i < memberCount; ++i)
        query(m_server->DBConnection(),memberID[i],authority[i]);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::GuildAuthority(pkt);
        m_connection->Send(pkt);
    }

    return MSG_SUCCESS;
}

int StateGame::OnGuildPromote (IPacket &packet)
{
    uint32_t npcID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    World *world = m_server->GetWorld();

    boost::shared_ptr<NPC> npc = world->FindNPC_CloseToPosition(npcID,player->get_position());

    if (!npc || !IsGuildManager(npc->get_id()))
        return MSG_ERROR_ARG;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild)
        return MSG_ERROR_ORDER;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (guild->get_level() == GUILD_MAX_LEVEL)
    {
        if (pkt)
        {
            srv_pkt::GuildPromote(pkt,ERROR_GUILD_MAX_LEVEL);
            m_connection->Send(pkt);
        }

        return MSG_SUCCESS;
    }

    std::pair<uint16_t,uint16_t> requirements = world->GetGuildManager()->GetPromotePolicy(guild->get_level()+1);

    if (guild->get_gp() < requirements.second)
    {
        if (pkt)
        {
            srv_pkt::GuildPromote(pkt,ERROR_GUILD_INSUFFICIENT_GP);
            m_connection->Send(pkt);
        }

        return MSG_SUCCESS;
    }

    if (!player->spend_gold(requirements.first))
    {
        if (pkt)
        {
            srv_pkt::GuildPromote(pkt,ERROR_GUILD_INSUFFICIENT_GOLD);
            m_connection->Send(pkt);
        }

        return MSG_SUCCESS;
    }

    guild->spend_gp(requirements.second);

    /// TODO: UPDATE UNION!! IF GUILDS GETS PROMOTED TO LV 2 INCREASE THE GUILD STORAGE CAPACITY TO 0x90
    guild->promote();

    if (pkt)
    {
        srv_pkt::GuildPromote(pkt);
        m_connection->Send(pkt);
    }

    DB::GUILD::GP gp_query;
    gp_query(m_server->DBConnection(),guild->get_id(),guild->get_gp());

    DB::GUILD::Level lv_query;
    lv_query(m_server->DBConnection(),guild->get_id(),guild->get_level());

    return MSG_SUCCESS;
}

int StateGame::OnGuildInvite (IPacket &packet)
{
    uint32_t playerID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (playerID != player->get_selected_object())
        return MSG_ERROR_ARG;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild)
        return MSG_ERROR_ORDER;

    if (guild->full())
    {
#if defined DEBUG
        syslog(LOG_INFO,"Guild is full.");
#endif
        return MSG_SUCCESS;
    }

    /// CHECK IF THE PLAYER HAVE JOIN RIGHTS OR IS GUILD MASTER
    Guild::const_iterator member_iter = guild->find(player->get_name());

    if (member_iter == guild->end() || !member_iter->second->Master || !member_iter->second->CanJoin())
        return MSG_ERROR_ORDER;

    boost::shared_ptr<Player> inv_player;
    inv_player = m_server->GetWorld()->FindPlayer_CloseToPosition(playerID,player->get_position());

    if (!inv_player)
        return MSG_SUCCESS;

    boost::shared_ptr<OPacket> pkt(new OPacket);
    boost::shared_ptr<Guild> player_guild = inv_player->get_guild();

    if (!player_guild)
    {
        inv_player->set_invitation(INVITATION_GUILD);
        inv_player->set_invitation_player(player->GetUniqueID());

        if (pkt)
        {
            srv_pkt::Invite(pkt,INVITATION_GUILD,player->GetUniqueID());
            m_server->Send(pkt,inv_player->ConnectionID());
        }
    }
    else
    {
        if (pkt)
        {
            srv_pkt::GuildInviteError(pkt,srv_pkt::INVITE_ERROR_PLAYER_WITH_GUILD);
            m_connection->Send(pkt);
        }
    }

    return MSG_SUCCESS;
}

int StateGame::OnGuildWithdraw (IPacket &packet)
{
    std::string name = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    if (name.length() > 12) /// TODO: CHANGE 12 TO MAX_NAME_LEN DEFINE!! INSIDE PLAYER FILES
        return MSG_ERROR_ARG;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild)
        return MSG_ERROR_ORDER;

    /// CHECK IF THE PLAYER HAS WITHDRAW RIGHTS OR IS THE GUILD MASTER.
    Guild::const_iterator member_iter = guild->find(player->get_name());

    if (member_iter == guild->end() || !member_iter->second->Master || !member_iter->second->CanWithdraw())
        return MSG_ERROR_ORDER;

    if (!guild->member_withdraw(name,GUILD_MEMBER_BANNED))
        return MSG_ERROR_ORDER;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::GuildKick(pkt);
        m_connection->Send(pkt);
    }

    return MSG_SUCCESS;
}

int StateGame::OnGuildLeave (IPacket &packet)
{
    uint32_t npcID = packet.Read<uint32_t>();

    if (packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<NPC> npc = m_server->GetWorld()->FindNPC_CloseToPosition(npcID,player->get_position());

    if (!npc || !IsGuildManager(npc->get_id()))
        return MSG_ERROR_ARG;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild)
        return MSG_ERROR_ORDER;

    if(!guild->member_withdraw(player->get_name(),GUILD_MEMBER_LEFT))
    {
#if defined DEBUG
        syslog(LOG_INFO,"%s dont belong to the guild.",player->get_name().c_str());
#endif
        return MSG_ERROR_ORDER;
    }

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::GuildLeave(pkt);
        m_connection->Send(pkt);
    }

    return MSG_SUCCESS;
}

int StateGame::OnGuildLeaderVote (IPacket &packet)
{
#if defined DEBUG
    PrintPacketToSyslog(packet);
#endif

    return MSG_SUCCESS;
}

int StateGame::OnGuildTransferLeadership (IPacket &packet)
{
    uint32_t npcID = packet.Read<uint32_t>();

    uint32_t memberID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<NPC> npc = m_server->GetWorld()->FindNPC_CloseToPosition(npcID,player->get_position());

    if (!npc || !IsGuildManager(npc->get_id()))
        return MSG_ERROR_ARG;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild)
        return MSG_ERROR;

    Guild::const_iterator iter = guild->find(player->get_name());

    /// CHECK THAT IS THE GUILD MASTER!!!
    if (iter == guild->end() || !iter->second->Master)
        return MSG_ERROR_ORDER;

    if(!guild->member_transfer_leadership(iter->second->ID,memberID))
        return MSG_ERROR_ORDER;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::GuildTransferLeadership(pkt);
        m_connection->Send(pkt);
    }

    DB::GUILD_MEMBER::Master query;
    query(m_server->DBConnection(),iter->second->ID,false);
    query(m_server->DBConnection(),memberID,true);

    return MSG_SUCCESS;
}

int StateGame::OnGuildWarPropose (IPacket &packet)
{
#if defined DEBUG
    PrintPacketToSyslog(packet);
#endif
    /*std::string guildName = packet.Read();

	if (!packet.EndOfStream())
		throw Exception::InvalidPacketSize();


	DB::GUILD::ID id_query;
	uint32_t ID = id_query(m_conn,guild);

	if (!ID)
	{
		send(packet::GuildWarInvite(packet::ERROR_GUILD_INVALID_NAME));
		return;
	}
	*/

    return MSG_SUCCESS;
}

int StateGame::OnGuildWarRewardGold (IPacket &packet)
{
    uint32_t npcID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<NPC> npc = m_server->GetWorld()->FindNPC_CloseToPosition(npcID,player->get_position());

    if (!npc || !IsGuildManager(npc->get_id()))
        return MSG_ERROR_ARG;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild)
        return MSG_ERROR_ORDER;

    /// TODO: GET WAR REWARD GOLD!!

    return MSG_SUCCESS;
}

int StateGame::OnGuildStorageOpen (IPacket &packet)
{
    uint32_t npcID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<NPC> npc = m_server->GetWorld()->FindNPC_CloseToPosition(npcID,player->get_position());

    if (!npc || !IsGuildManager(npc->get_id()))
       return MSG_ERROR_ARG;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild || guild->get_level() < GUILD_STORAGE_MIN_ACCESS_LEVEL)
        return MSG_ERROR_ORDER;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (guild->is_storage_in_use())
    {
        if (pkt)
        {
            srv_pkt::GuildStorageOpen(pkt,ERROR_GUILD_STORAGE_IN_USE);
            m_connection->Send(pkt);
        }
    }
    else
    {
        guild->set_storage_usage(true);

        if (pkt)
        {
            srv_pkt::GuildStorageOpen(pkt);
            m_connection->Send(pkt);
        }
    }

    return MSG_SUCCESS;
}

int StateGame::OnGuildStorageInfo (IPacket &packet)
{
    uint32_t npcID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<NPC> npc = m_server->GetWorld()->FindNPC_CloseToPosition(npcID,player->get_position());

    if (!npc || !IsGuildManager(npc->get_id()))
        return MSG_ERROR_ARG;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild)
        return MSG_ERROR_ORDER;

    boost::shared_ptr<Storage> storage = guild->get_guild_storage();

    if (!storage)
        return MSG_ERROR;

    boost::shared_ptr<OPacket> p1(new OPacket),p2(new OPacket),p3(new OPacket);

    if (p1 && p2 && p3)
    {
        srv_pkt::GuildStorageGold(p1,guild->get_stored_gold());
        srv_pkt::GuildStorage(p2,storage);
        srv_pkt::GuildStorageEnd(p3);
        m_connection->Send(p1,p2,p3);
    }

    connect_storage_signals(storage);

    return MSG_SUCCESS;
}

int StateGame::OnGuildStorageClose (IPacket &packet)
{
    uint32_t npcID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<NPC> npc = m_server->GetWorld()->FindNPC_CloseToPosition(npcID,player->get_position());

    if (!npc || !IsGuildManager(npc->get_id()))
        return MSG_ERROR_ARG;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild)
        return MSG_ERROR_ORDER;

    guild->set_storage_usage(false);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::GuildStorageClose(pkt);
        m_connection->Send(pkt);
    }

    boost::shared_ptr<Storage> storage = guild->get_guild_storage();

    if (storage)
        disconnect_storage_signals(storage);

    return MSG_SUCCESS;
}

int StateGame::OnGuildDisband (IPacket &packet)
{
    uint32_t npcID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<NPC> npc = m_server->GetWorld()->FindNPC_CloseToPosition(npcID,player->get_position());

    if (!npc || !IsGuildManager(npc->get_id()))
        return MSG_ERROR_ARG;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild)
        return MSG_ERROR_ORDER;

    /// CHECK THAT THE PLAYER DISBANDING THE GUILD IS THE MASTER
    if (!guild->is_guild_master(player->get_name()))
        return MSG_ERROR_ORDER;

    Guild::member_container_type member_list = guild->get_member_list();

    m_server->GetWorld()->GetGuildManager()->DeleteGuild(guild->get_id());

    DB::GUILD_MEMBER::Remove member_query;
    for (Guild::const_iterator iter = member_list.begin(); iter != member_list.end(); ++iter)
        member_query(m_server->DBConnection(),iter->second->ID);

    if (guild->get_union_id())
    {
        boost::shared_ptr<Union> alliance = m_server->GetWorld()->GetUnionManager()->Find(guild->get_union_id());

        if (alliance)
        {
            alliance->remove(guild->get_id(),UG_REASON_SECEDE);

            /// IF THE GUILD IS THE LEADING GUILD DESTROY ALLIANCE AND CLEAR UNION DATA ON THE GUILDS
            if (guild->get_id() == alliance->head_guild_id())
            {
                std::vector<boost::shared_ptr<Guild> > glist = alliance->guild_list();

                DB::GUILD::Union union_query;
                for (Union::const_iterator iter = glist.begin(); iter != glist.end(); ++iter)
                {
                    if ((*iter))
                    {
                        (*iter)->set_union_id(0);
                        union_query(m_server->DBConnection(),(*iter)->get_id(),0);
                    }
                }

                m_server->GetWorld()->GetUnionManager()->Remove(guild->get_union_id());
            }
        }
    }

    guild->disband();

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::GuildDisband(pkt);
        m_connection->Send(pkt);
    }

    return MSG_SUCCESS;
}

void StateGame::connect_guild_signals ()
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (guild)
    {
        using boost::bind;

        guild->signal_gp.connect(bind(&StateGame::OnGuildEvent_GP,this,_1));
        guild->signal_notice.connect(bind(&StateGame::OnGuildEvent_Notice,this,_1,_2));
        guild->signal_promote.connect(bind(&StateGame::OnGuildEvent_Promote,this,_1,_2));
        guild->signal_disband.connect(bind(&StateGame::OnGuildEvent_Disband,this));
        guild->signal_alliance.connect(bind(&StateGame::OnGuildEvent_Alliance,this,_1));

        guild->signal_member_gp.connect(bind(&StateGame::OnGuildEvent_MemberGP,this,_1,_2));
        guild->signal_member_state.connect(bind(&StateGame::OnGuildEvent_MemberState,this,_1,_2));
        guild->signal_member_grant.connect(bind(&StateGame::OnGuildEvent_MemberGrant,this,_1,_2,_3));
        guild->signal_member_insert.connect(bind(&StateGame::OnGuildEvent_MemberJoin,this,_1,_2));
        guild->signal_member_remove.connect(bind(&StateGame::OnGuildEvent_MemberWithdraw,this,_1,_2,_3));
        guild->signal_member_position.connect(bind(&StateGame::OnGuildEvent_MemberPosition,this,_1,_2));
        guild->signal_member_zone.connect(bind(&StateGame::OnGuildEvent_MemberZone,this,_1,_2));
        guild->signal_member_leader.connect(bind(&StateGame::OnGuildEvent_MemberLeader,this,_1,_2));
        guild->signal_member_authority.connect(bind(&StateGame::OnGuildEvent_MemberAuthority,this,_1,_2,_3));
        guild->signal_member_level.connect(bind(&StateGame::OnGuildEvent_MemberLevel,this,_1,_2));
    }
}

void StateGame::disconnect_guild_signals ()
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (guild)
    {
        using boost::bind;

        guild->signal_gp.disconnect(bind(&StateGame::OnGuildEvent_GP,this,_1));
        guild->signal_notice.disconnect(bind(&StateGame::OnGuildEvent_Notice,this,_1,_2));
        guild->signal_promote.disconnect(bind(&StateGame::OnGuildEvent_Promote,this,_1,_2));
        guild->signal_disband.disconnect(bind(&StateGame::OnGuildEvent_Disband,this));
        guild->signal_alliance.disconnect(bind(&StateGame::OnGuildEvent_Alliance,this,_1));

        guild->signal_member_gp.disconnect(bind(&StateGame::OnGuildEvent_MemberGP,this,_1,_2));
        guild->signal_member_state.disconnect(bind(&StateGame::OnGuildEvent_MemberState,this,_1,_2));
        guild->signal_member_grant.disconnect(bind(&StateGame::OnGuildEvent_MemberGrant,this,_1,_2,_3));
        guild->signal_member_insert.disconnect(bind(&StateGame::OnGuildEvent_MemberJoin,this,_1,_2));
        guild->signal_member_remove.disconnect(bind(&StateGame::OnGuildEvent_MemberWithdraw,this,_1,_2,_3));
        guild->signal_member_position.disconnect(bind(&StateGame::OnGuildEvent_MemberPosition,this,_1,_2));
        guild->signal_member_zone.disconnect(bind(&StateGame::OnGuildEvent_MemberZone,this,_1,_2));
        guild->signal_member_leader.disconnect(bind(&StateGame::OnGuildEvent_MemberLeader,this,_1,_2));
        guild->signal_member_authority.disconnect(bind(&StateGame::OnGuildEvent_MemberAuthority,this,_1,_2,_3));
        guild->signal_member_level.disconnect(bind(&StateGame::OnGuildEvent_MemberLevel,this,_1,_2));
    }
}

void StateGame::OnGuildEvent_Notice (const std::string &title, const std::string &msg)
{
    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::GuildUpdate(pkt,title,msg);
        m_connection->Send(pkt);
    }
}

void StateGame::OnGuildEvent_GP (const uint32_t GP)
{
    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::GuildUpdate(pkt,GP);
        m_connection->Send(pkt);
    }
}

void StateGame::OnGuildEvent_Promote (const uint8_t lvl, const uint32_t GP)
{
    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::GuildUpdate(pkt,lvl,GP);
        m_connection->Send(pkt);
    }
}

void StateGame::OnGuildEvent_Disband ()
{
    disconnect_guild_signals();

    boost::shared_ptr<OPacket> pkt(new OPacket);
    
    if (pkt)
    {
        srv_pkt::GuildUpdate(pkt,GUILD_NOTIFY_DISBAND);
        m_connection->Send(pkt);
    }

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (player)
    {
        pkt.reset(new OPacket);

        if (pkt)
        {
            srv_pkt::GuildClear(pkt,player->GetUniqueID());
            m_server->SendChannel(pkt,player->get_position().get_zone());
        }

        player->guild_clear();

        boost::shared_ptr<Party> party = player->get_party();

        if (party)
            party->SendGuildNotification(player->GetUniqueID(),"");
    }
}

void StateGame::OnGuildEvent_MemberGP (const uint32_t ID, const uint32_t GP)
{
    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::GuildUpdate(pkt,ID,GP,0);
        m_connection->Send(pkt);
    }
}

void StateGame::OnGuildEvent_MemberState (const uint32_t ID, const uint8_t state)
{
    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::GuildMemberState(pkt,ID,static_cast<GUILD_MEMBER_STATE>(state));
        m_connection->Send(pkt);
    }
}

void StateGame::OnGuildEvent_MemberGrant (const uint32_t guildID, const std::string &name, const std::string &grant)
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (player)
    {
        boost::shared_ptr<OPacket> pkt(new OPacket);

        if (pkt)
        {
            srv_pkt::PlayerGrantName(pkt,guildID,name,grant);

            if (player->get_name() == name)
            {
                m_server->SendChannel(pkt,player->get_position().get_zone());
            }
            else
                m_connection->Send(pkt);
        }
    }
}

void StateGame::OnGuildEvent_MemberPosition (const uint32_t memberID, const uint8_t position)
{
    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::GuildUpdate(pkt,memberID,position);
        m_connection->Send(pkt);
    }
}

void StateGame::OnGuildEvent_MemberZone (const uint32_t memberID, const int16_t zoneID)
{
    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::GuildNotify_MemberZone(pkt,memberID,zoneID);
        m_connection->Send(pkt);
    }
}

void StateGame::OnGuildEvent_MemberLevel (const uint32_t memberID, const uint8_t level)
{
    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::GuildNotify_MemberLevel(pkt,memberID,level);
        m_connection->Send(pkt);
    }
}

void StateGame::OnGuildEvent_MemberJoin (const uint32_t guildID, const std::string &memberName)
{
    boost::shared_ptr<Guild> guild = m_server->GetWorld()->GetGuildManager()->Find(guildID);

    if (guild)
    {
        Guild::const_iterator iter = guild->find(memberName);

        if (iter != guild->end())
        {
            boost::shared_ptr<OPacket> pkt(new OPacket);

            if (pkt)
            {
                srv_pkt::GuildNewMember(pkt,iter->second);
                m_connection->Send(pkt);
            }
        }
    }
}

void StateGame::OnGuildEvent_MemberWithdraw (const std::string &name, const uint32_t memberID, const uint8_t reason)
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (player)
    {
        boost::shared_ptr<OPacket> pkt(new OPacket);

        if (pkt)
        {
            srv_pkt::GuildUpdate(pkt,memberID,reason);
            m_connection->Send(pkt);
        }

        if (player->get_name() == name)
        {
            /// TODO: ADD PLAYER PENALTY FOR JOINING GUILD!! 3 DAYS!!

            player->guild_clear();

            pkt.reset(new OPacket);

            if (pkt)
            {
                srv_pkt::GuildClear(pkt,player->GetUniqueID());
                m_server->SendChannel(pkt,player->get_position().get_zone());
            }

            DB::GUILD_MEMBER::Remove query;
            query(m_server->DBConnection(),memberID);

            boost::shared_ptr<Party> party = player->get_party();

            if (party)
                party->SendGuildNotification(player->GetUniqueID(),"");
        }
    }
}

void StateGame::OnGuildEvent_MemberLeader (const uint32_t currentMasterID, const uint32_t nextMasterID)
{
    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::GuildUpdate(pkt,currentMasterID,nextMasterID);
        m_connection->Send(pkt);
    }
}

void StateGame::OnGuildEvent_MemberAuthority (const uint8_t count, const boost::shared_array<uint32_t> &memberIDs,
        const boost::shared_array<uint32_t> &authority)
{
    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::GuildNotify_Authority_V1(pkt,count,memberIDs,authority);
        m_connection->Send(pkt);
    }

    pkt.reset(new OPacket);

    if (pkt)
    {
        srv_pkt::GuildNotify_Authority_V2(pkt,count,memberIDs,authority);
        m_connection->Send(pkt);
    }
}

void StateGame::OnGuildEvent_Alliance (const uint32_t allianceID)
{
    if (allianceID)
    {
        boost::shared_ptr<Union> ally = m_server->GetWorld()->GetUnionManager()->Find(allianceID);

        if (ally)
        {
            boost::shared_ptr<OPacket> pkt(new OPacket);

            if (pkt)
            {
                srv_pkt::AllianceList(pkt,ally);
                m_connection->Send(pkt);
            }

            ConnectAllianceSignals();
        }
    }
    else
    {
        DisconnectAllianceSignals();
    }
}
