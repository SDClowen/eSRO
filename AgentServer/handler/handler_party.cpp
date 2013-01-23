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
#include "manager/party_manager.hpp"

#include <player.hpp>

/// SRNL INCLUDES
#include <packet_party.hpp>
#include <packet_misc.hpp>
#include <packet_msg.hpp>

#if defined DEBUG
#include <packet_util_functions.hpp>
#endif

#include <opcodes_shard_server.hpp>

int StateGame::OnPartyCreate (IPacket &packet)
{
    uint32_t targetID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (player->level() < 5)
    {
        srv_pkt::PartyInvite(pkt,srv_pkt::PARTY_ERROR_INSUFFICIENT_LEVEL);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    switch(player->GetCurrentTask())
    {
    case PLAYER_TASK_STALL:
    case PLAYER_TASK_EXCHANGE:
    case PLAYER_TASK_NPC_TALK:
        srv_pkt::PartyInvite(pkt,srv_pkt::PARTY_ERROR_PLAYER_BUSY);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    default:
        break;
    }

    boost::shared_ptr<Player> inv_player = m_server->GetWorld()->FindPlayer_ByID(targetID);

    if (!inv_player)
    {
        srv_pkt::PartyInvite(pkt,srv_pkt::PARTY_ERROR_INVALID_TARGET);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    if (!CanPlayerInteract(player,inv_player))
    {
        srv_pkt::PartyInvite(pkt,srv_pkt::PARTY_ERROR_OPPOSING_ROLES);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    switch(inv_player->GetCurrentTask())
    {
    case PLAYER_TASK_STALL:
    case PLAYER_TASK_EXCHANGE:
    case PLAYER_TASK_NPC_TALK:
        srv_pkt::PartyInvite(pkt,srv_pkt::PARTY_ERROR_PLAYER_BUSY);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    default:
        break;
    }

    if (!inv_player->in_party())
    {
        if (!inv_player->get_invitation_player())
        {
            inv_player->set_invitation(INVITATION_PARTY);

            inv_player->set_invitation_player(player->GetUniqueID());

            boost::shared_ptr<Party> party = player->get_party();

            if (party)
            {
                srv_pkt::PartyInvitation(pkt,player->GetUniqueID(),1);
                m_server->Send(pkt,inv_player->ConnectionID());
            }
        }
        else
        {
            srv_pkt::PartyInvite(pkt,srv_pkt::PARTY_ERROR_PENDING_INVITATION);
            m_connection->Send(pkt);
            return MSG_SUCCESS;
        }
    }
    else
    {
        srv_pkt::PartyInvite(pkt,srv_pkt::PARTY_ERROR_TARGET_IN_PARTY);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    return MSG_SUCCESS;
}

int StateGame::OnPartyAnswer (IPacket &packet)
{
    uint32_t sessionID = packet.Read<uint32_t>();

    uint32_t playerID = packet.Read<uint32_t>();

    uint8_t answer = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    if (answer > 2)
        return MSG_ERROR_ARG;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Player> inv_player = get_invitation_player(player);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (!inv_player || inv_player->GetUniqueID() != playerID)
    {
        srv_pkt::PartyAnswer(pkt,srv_pkt::PARTY_ERROR_INVALID_APPLICANT);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    switch(answer)
    {
    case srv_pkt::PARTY_ANSWER_DENY:
    case srv_pkt::PARTY_ANSWER_IGNORE:
        srv_pkt::PartyAnswer(pkt,(srv_pkt::PARTY_ANSWER_TYPE)answer);
        m_connection->Send(pkt,false,false);
        m_server->Send(pkt,inv_player->ConnectionID());
        break;
    case srv_pkt::PARTY_ANSWER_ACCEPT:
        {
            boost::shared_ptr<Party> party = player->get_party();

            if (!party)
                return MSG_SUCCESS;

            srv_pkt::PartyAnswer(pkt,srv_pkt::PARTY_ANSWER_ACCEPT);
            m_connection->Send(pkt,false,false);
            m_server->Send(pkt,inv_player->ConnectionID());

            if (party->get_member_count() == 1)
            {
                pkt.reset(new OPacket);

                pkt->WriteOpcode(0xB442);
                pkt->Write<uint8_t>(ANSWER_ACCEPT);
                pkt->Write<uint32_t>(player->GetUniqueID());
                m_connection->Send(pkt);
            }

            pkt.reset(new OPacket);

            srv_pkt::PartyAccepted(pkt,inv_player->GetUniqueID());
            m_server->Send(pkt,inv_player->ConnectionID());

            inv_player->join_party(party);

            inv_player->signal_connect_party();

            pkt.reset(new OPacket);

            srv_pkt::PartyInfo(pkt,party);
            m_server->Send(pkt,inv_player->ConnectionID());
        }
        break;
    default:
        break;
    }

    inv_player->set_invitation(INVITATION_NONE);
    inv_player->set_invitation_player(0);

    player->set_invitation(INVITATION_NONE);
    player->set_invitation_player(0);

    return MSG_SUCCESS;
}

int StateGame::OnPartyLeave (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (player->in_party())
        player->leave_party();

    return MSG_SUCCESS;
}

int StateGame::OnPartyKick (IPacket &packet)
{
    uint32_t playerID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (!player->in_party())
        return MSG_ERROR_ORDER;

    if (player->GetUniqueID() == playerID)
        return MSG_ERROR_ARG;

    boost::shared_ptr<Party> party = player->get_party();

    if (!party)
        return MSG_ERROR;

    boost::shared_ptr<Player> master = party->get_master();

    if (!master || player->GetUniqueID() != master->GetUniqueID())
        return MSG_ERROR_ARG;

    party->kick(playerID);

    return MSG_SUCCESS;
}

int StateGame::OnPartyLeader (IPacket &packet)
{
    uint32_t memberID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (!player->in_party())
        return MSG_ERROR_ORDER;

    if (player->GetUniqueID() == memberID)
        return MSG_ERROR_ARG;

    boost::shared_ptr<Party> party = player->get_party();

    if (!party)
        return MSG_ERROR;

    boost::shared_ptr<Player> master = party->get_master();

    if (!master || player->GetUniqueID() != master->GetUniqueID())
        return MSG_ERROR_ARG;

    party->transferLeadership(memberID);

    return MSG_SUCCESS;
}

int StateGame::OnPartySearch (IPacket &packet)
{
    typedef boost::shared_ptr<Party> party_type;

    uint8_t page = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    uint32_t partyID = 0;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if ( player->in_party())
        partyID = player->get_party()->get_number();

    uint8_t max_pages = m_server->GetWorld()->GetPartyManager()->max_pages();

    if (page > max_pages)
        return MSG_SUCCESS;

    std::vector<party_type> party_list = m_server->GetWorld()->GetPartyManager()->get_party_list(page,partyID);

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::PartyList(pkt,max_pages,page,party_list);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnPartyInformation (IPacket &packet)
{
    uint32_t partyID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Party> party = m_server->GetWorld()->GetPartyManager()->find(partyID);

    if (!party)
        return MSG_ERROR_ARG;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (!pkt)
        return MSG_ERROR_MEMORY;

    return MSG_SUCCESS;
}

int StateGame::OnPartyForm (IPacket &packet)
{
    /// First 8 bytes 0.
    uint64_t reserved = packet.Read<uint64_t>();

    uint8_t pt_type = packet.Read<uint8_t>();

    uint8_t purpose = packet.Read<uint8_t>();

    uint8_t min_lv = packet.Read<uint8_t>();

    uint8_t max_lv = packet.Read<uint8_t>();

    std::string description = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    if (reserved || pt_type > 7 || purpose > PARTY_PURPOSE_THIEF)
        return MSG_ERROR_ARG;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (player->level() < 5)
    {
        std::cout << "Invalid level." << std::endl;
        return MSG_SUCCESS;
    }

    if (!player->in_party())
    {
        player->create_party();

        connect_party_signals();
    }

    boost::shared_ptr<Party> party = player->get_party();

    if (!party)
        return MSG_SUCCESS;

    boost::shared_ptr<PartyManager> pmgr = m_server->GetWorld()->GetPartyManager();

    uint32_t pt_number = pmgr->get_next_number();

    party->set_entry(PartyEntry(pt_number,pt_type,purpose,min_lv,max_lv,description));

    pmgr->insert_party(party);

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::PartyForm(pkt,party->get_entry(),party->get_id());
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnPartyChange (IPacket &packet)
{
    uint32_t partyID = packet.Read<uint32_t>();

    uint32_t reserved = packet.Read<uint32_t>();

    uint8_t pt_type = packet.Read<uint8_t>();

    uint8_t purpose = packet.Read<uint8_t>();

    uint8_t min_lv = packet.Read<uint8_t>();

    uint8_t max_lv = packet.Read<uint8_t>();

    std::string description = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    if (reserved || pt_type > 7 || purpose > PARTY_PURPOSE_THIEF)
        return MSG_ERROR_ARG;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (!player->in_party())
        return MSG_ERROR_ORDER;

    boost::shared_ptr<Party> party = player->get_party();

    if (!party)
        return MSG_ERROR;

    if (party->get_id() != partyID)
    {
        syslog(LOG_DEBUG,"PartyIDs mismatch.");
        return MSG_ERROR_ARG;
    }

    PartyEntry entry(party->get_number(),pt_type,purpose,min_lv,max_lv,description);

    party->set_entry(entry);

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::PartyChange(pkt,entry,partyID);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnPartyRemove (IPacket &packet)
{
    uint32_t partyID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    m_server->GetWorld()->GetPartyManager()->remove_party(partyID);

    boost::shared_ptr<Party> party = player->get_party();

    if (!party)
        return MSG_ERROR;

    party->set_entry(PartyEntry());

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::PartyDelete(pkt,partyID);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnPartyJoin (IPacket &packet)
{
    uint32_t party_number = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Party> party = m_server->GetWorld()->GetPartyManager()->find(party_number);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (party)
    {
        switch(player->GetCurrentTask())
        {
        case PLAYER_TASK_STALL:
        case PLAYER_TASK_EXCHANGE:
        case PLAYER_TASK_NPC_TALK:
            srv_pkt::PartyAnswer(pkt,srv_pkt::PARTY_ERROR_PLAYER_BUSY);
            m_connection->Send(pkt);
            return MSG_SUCCESS;
        default:
            break;
        }

        if (!party->valid_level(player->level()))
        {
            srv_pkt::PartyAnswer(pkt,srv_pkt::PARTY_ERROR_INVALID_LEVEL);
            m_connection->Send(pkt);
            return MSG_SUCCESS;
        }

        boost::shared_ptr<Player> master_player = party->get_master();

        if (!master_player)
        {
            srv_pkt::PartyAnswer(pkt,srv_pkt::PARTY_ERROR_INVALID_TARGET);
            m_connection->Send(pkt);
            return MSG_SUCCESS;
        }

        /// JOB CHECKING!!
        /// HUNTER AND TRADERS OR THIEF WITH THIEF AND NORMAL WITH NORMAL
        bool job_allow = CanPlayerInteract(player,master_player);

        if (!job_allow)
        {
            srv_pkt::PartyAnswer(pkt,srv_pkt::PARTY_ERROR_OPPOSING_ROLES);
            m_connection->Send(pkt);
            return MSG_SUCCESS;
        }

        switch(master_player->GetCurrentTask())
        {
        case PLAYER_TASK_STALL:
        case PLAYER_TASK_EXCHANGE:
        case PLAYER_TASK_NPC_TALK:
            srv_pkt::PartyAnswer(pkt,srv_pkt::PARTY_ERROR_PLAYER_BUSY);
            m_connection->Send(pkt);
            return MSG_SUCCESS;
        default:
            break;
        }

        if (!master_player->get_invitation_player())
        {
            master_player->set_invitation(INVITATION_PARTY);

            master_player->set_invitation_player(player->GetUniqueID());

            uint32_t sessionID = 1;

            srv_pkt::PartyRequest(pkt,sessionID,party_number,player);
            m_server->Send(pkt,master_player->ConnectionID());
        }
        else
        {
            srv_pkt::PartyAnswer(pkt,srv_pkt::PARTY_ERROR_PENDING_INVITATION);
            m_connection->Send(pkt);
            return MSG_SUCCESS;
        }
    }
    else
    {
        srv_pkt::PartyAnswer(pkt,srv_pkt::PARTY_ERROR_INVALID_PARTY);
        m_connection->Send(pkt);
    }

    return MSG_SUCCESS;
}

int StateGame::OnPartyInvite (IPacket &packet)
{
    uint32_t targetID = packet.Read<uint32_t>();

    uint8_t type = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    if (!targetID)
        return MSG_ERROR_ARG;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (player->level() < 5)
    {
        srv_pkt::PartyInvite(pkt,srv_pkt::PARTY_ERROR_INSUFFICIENT_LEVEL);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    switch(player->GetCurrentTask())
    {
    case PLAYER_TASK_STALL:
    case PLAYER_TASK_EXCHANGE:
    case PLAYER_TASK_NPC_TALK:
        srv_pkt::PartyInvite(pkt,srv_pkt::PARTY_ERROR_PLAYER_BUSY);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    default:
        break;
    }

    boost::shared_ptr<Player> inv_player = m_server->GetWorld()->FindPlayer_ByID(targetID);

    if (!inv_player)
    {
        srv_pkt::PartyInvite(pkt,srv_pkt::PARTY_ERROR_INVALID_TARGET);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    if (!CanPlayerInteract(player,inv_player))
    {
        srv_pkt::PartyInvite(pkt,srv_pkt::PARTY_ERROR_OPPOSING_ROLES);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    switch(inv_player->GetCurrentTask())
    {
    case PLAYER_TASK_STALL:
    case PLAYER_TASK_EXCHANGE:
    case PLAYER_TASK_NPC_TALK:
        srv_pkt::PartyInvite(pkt,srv_pkt::PARTY_ERROR_PLAYER_BUSY);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    default:
        break;
    }

    if (!inv_player->in_party())
    {
        if (!inv_player->get_invitation_player())
        {
            inv_player->set_invitation(INVITATION_PARTY);

            inv_player->set_invitation_player(player->GetUniqueID());

            srv_pkt::PartyInvitation(pkt,player->GetUniqueID(),type);
            m_server->Send(pkt,inv_player->ConnectionID());

            if (!inv_player->in_party())
            {
                player->create_party();

                connect_party_signals();
            }
        }
        else
        {
            srv_pkt::PartyInvite(pkt,srv_pkt::PARTY_ERROR_PENDING_INVITATION);
            m_connection->Send(pkt);
        }
    }
    else
    {
        srv_pkt::PartyInvite(pkt,srv_pkt::PARTY_ERROR_TARGET_IN_PARTY);
        m_connection->Send(pkt);
    }

    return MSG_SUCCESS;
}

void StateGame::connect_party_signals ()
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return;

    boost::shared_ptr<Party> party = player->get_party();

    if (party)
    {
        party->signal_join.connect(boost::bind(&StateGame::OnPartyEvent_Join,this,_1));

        party->signal_leave.connect(boost::bind(&StateGame::OnPartyEvent_Leave,this,_1));

        party->signal_kick.connect(boost::bind(&StateGame::OnPartyEvent_Kick,this,_1));

        party->signal_dismiss.connect(boost::bind(&StateGame::OnPartyEvent_Dismiss,this));

        party->signal_leader.connect(boost::bind(&StateGame::OnPartyEvent_Leader,this,_1));

        party->signal_mastery.connect(boost::bind(&StateGame::OnPartyEvent_Mastery,this,_1,_2,_3));

        party->signal_move.connect(boost::bind(&StateGame::OnPartyEvent_Move,this,_1,_2));

        party->signal_msg.connect(boost::bind(&StateGame::OnPartyEvent_Msg,this,_1,_2));

        party->signal_guild.connect(boost::bind(&StateGame::OnPartyEvent_Guild,this,_1,_2));

        party->signal_hp_mp.connect(boost::bind(&StateGame::OnPartyEvent_HPMP,this,_1,_2,_3,_4,_5));

        party->signal_level.connect(boost::bind(&StateGame::OnPartyEvent_Level,this,_1,_2));

        party->signal_mastery.connect(boost::bind(&StateGame::OnPartyEvent_Mastery,this,_1,_2,_3));
    }
}

void StateGame::disconnect_party_signals ()
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return;

    boost::shared_ptr<Party> party = player->get_party();

    if (party)
    {
        party->signal_join.disconnect(boost::bind(&StateGame::OnPartyEvent_Join,this,_1));

        party->signal_leave.disconnect(boost::bind(&StateGame::OnPartyEvent_Leave,this,_1));

        party->signal_kick.disconnect(boost::bind(&StateGame::OnPartyEvent_Kick,this,_1));

        party->signal_dismiss.disconnect(boost::bind(&StateGame::OnPartyEvent_Dismiss,this));

        party->signal_leader.disconnect(boost::bind(&StateGame::OnPartyEvent_Leader,this,_1));

        party->signal_mastery.disconnect(boost::bind(&StateGame::OnPartyEvent_Mastery,this));

        party->signal_move.disconnect(boost::bind(&StateGame::OnPartyEvent_Move,this,_1,_2));

        party->signal_msg.disconnect(boost::bind(&StateGame::OnPartyEvent_Msg,this,_1,_2));

        party->signal_guild.disconnect(boost::bind(&StateGame::OnPartyEvent_Guild,this,_1,_2));

        party->signal_hp_mp.disconnect(boost::bind(&StateGame::OnPartyEvent_HPMP,this,_1,_2,_3,_4,_5));

        party->signal_level.disconnect(boost::bind(&StateGame::OnPartyEvent_Level,this,_1,_2));

        party->signal_mastery.disconnect(boost::bind(&StateGame::OnPartyEvent_Mastery,this,_1,_2,_3));
    }
}

void StateGame::OnPartyEvent_Join (boost::shared_ptr<Player> player)
{
    if (!player)
        return;

    boost::shared_ptr<Party> party = player->get_party();

    if (party)
    {
        boost::shared_ptr<OPacket> pkt(new OPacket);

        if (party->get_member_count() == 1)
        {
            srv_pkt::PartyInfo(pkt,party);
            m_connection->Send(pkt);

            pkt.reset(new OPacket);
        }

        srv_pkt::PartyUpdate(pkt,player);
        m_connection->Send(pkt);
    }
}

void StateGame::OnPartyEvent_Leave (const uint32_t playerID)
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return;

    boost::shared_ptr<Party> party = player->get_party();

    if (!party)
        return;

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::PartyUpdate(pkt,playerID,srv_pkt::PT_LEFT);
    m_connection->Send(pkt);

    if (party->get_member_count() == 1)
        player->leave_party();

    if (player->GetUniqueID() == playerID)
    {
        disconnect_party_signals();

        player->clear_party();
    }
}

void StateGame::OnPartyEvent_Dismiss ()
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return;

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::PartyUpdate(pkt);
    m_connection->Send(pkt);

    disconnect_party_signals();

    player->clear_party();
}

void StateGame::OnPartyEvent_Leader (const uint32_t playerID)
{
    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::PartyUpdate(pkt,playerID);
    m_connection->Send(pkt);
}

void StateGame::OnPartyEvent_Kick (const uint32_t playerID)
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return;

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::PartyUpdate(pkt,playerID,srv_pkt::PT_KICK);
    m_connection->Send(pkt);

    if (player->GetUniqueID() == playerID)
    {
        boost::shared_ptr<Party> party = player->get_party();

        if (party)
        {
            disconnect_party_signals();

            boost::shared_ptr<Player> master = party->get_master();

            if (master)
            {
                pkt.reset(new OPacket);

                pkt->WriteOpcode(SERVER_PARTY_BANNED,ANSWER_ACCEPT);
                pkt->Write<uint32_t>(master->GetUniqueID());

                m_connection->Send(pkt);
            }

            player->clear_party();
        }
    }
}

void StateGame::OnPartyEvent_Mastery (const uint32_t playerID, const uint32_t fMastID, const uint32_t sMastID)
{
    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::PartyUpdate(pkt,playerID,fMastID,sMastID);
    m_connection->Send(pkt);
}

void StateGame::OnPartyEvent_Move (const uint32_t playerID, const Coord &position)
{
    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::PartyUpdate(pkt,playerID,position);
    m_connection->Send(pkt);
}

void StateGame::OnPartyEvent_Msg (const std::string &name, const std::string &msg)
{
    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::Msg(pkt,srv_pkt::MSG_PARTY,name,msg);
    m_connection->Send(pkt);
}

void StateGame::OnPartyEvent_Level (const uint32_t playerID, const uint8_t level)
{
    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::PartyUpdate(pkt,playerID,level);
    m_connection->Send(pkt);
}

void StateGame::OnPartyEvent_Guild (const uint32_t playerID, const std::string &name)
{
    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::PartyUpdate(pkt,playerID,name);
    m_connection->Send(pkt);
}

void StateGame::OnPartyEvent_HPMP (const uint32_t playerID, const uint32_t HP, const uint32_t maxHP, const uint32_t MP,
    const uint32_t maxMP)
{
    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::PartyUpdate(pkt,playerID,HP,maxHP,MP,maxMP);
    m_connection->Send(pkt);
}
