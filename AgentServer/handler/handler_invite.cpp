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
#include "union_manager.hpp"
#include "guild_manager.hpp"

/// SRLF INCLUDES
#include <player.hpp>
#include <guild.hpp>
#include <union.hpp>

/// SRNL INCLUDES
#include <packet_exchange.hpp>
#include <packet_party.hpp>
#include <packet_guild.hpp>
#include <packet_alliance.hpp>

/// SRDL INCLUDES
#include <guild_query.hpp>
#include <player_query.hpp>

#include <opcodes_shard_server.hpp>

#if defined DEBUG
#include <packet_util_functions.hpp>
#endif

int StateGame::OnInvite (IPacket &packet)
{
    uint8_t ret = MSG_SUCCESS;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    switch(player->get_invitation())
    {
    case INVITATION_EXCHANGE:
        ret = OnInvite_Exchange(packet,player);
        break;
    case INVITATION_PARTY:
        ret = OnInvite_Party(packet,player);
        break;
    case INVITATION_GUILD:
        ret = OnInvite_Guild(packet,player);
        break;
    case INVITATION_UNION:
        ret = OnInvite_Alliance(packet,player);
        break;
    case INVITATION_ACADEMY:
        ret = OnInvite_Academy(packet,player);
        break;
    default:
#if defined DEBUG
        syslog(LOG_INFO,"Unknown invitation - %i",player->get_invitation());
#endif
        break;
    }

    /**
     *
     *	NOTE: Exchange keep using invitation player for his operation the rest of the invitations
     *	needs to be cleared.
     *
     **/

    if (player->get_invitation() != INVITATION_EXCHANGE)
    {
        player->set_invitation(INVITATION_NONE);
        player->set_invitation_player(0);
    }

    return ret;
}

int StateGame::OnInvite_Exchange (IPacket &packet, const boost::shared_ptr<Player> &player)
{
    enum
    {
        REJECT_EXCHANGE = 0x0001,
        ACCEPT_EXCHANGE = 0x0101
    };

    uint16_t answer = packet.Read<uint16_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> inv_player = get_invitation_player(player);

    if(!inv_player)
        return MSG_ERROR;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if ( answer == ACCEPT_EXCHANGE)
    {
        srv_pkt::ExchangeAccept(pkt,inv_player->GetUniqueID());
        m_connection->Send(pkt);

        pkt.reset(new OPacket);

        srv_pkt::ExchangeAccept(pkt,player->GetUniqueID());
        m_server->Send(pkt,inv_player->ConnectionID());
    }
    else if (answer == REJECT_EXCHANGE)
    {
        inv_player->set_invitation_player(0);
        player->set_invitation_player(0);

        srv_pkt::ExchangeReject(pkt);
        m_connection->Send(pkt,false,false);
        m_server->Send(pkt,inv_player->ConnectionID());
    }
    else
        return MSG_ERROR_ARG;

    return MSG_SUCCESS;
}

int StateGame::OnInvite_Party (IPacket &packet, const boost::shared_ptr<Player> &player)
{
    enum
    {
        ACCEPT_PARTY = 0x0101,
        REJECT_PARTY = 0x0C02
    };

    uint16_t answer = packet.Read<uint16_t>();

    boost::shared_ptr<Player> inv_player = get_invitation_player(player);

    if (!inv_player)
        return MSG_SUCCESS;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (answer == ACCEPT_PARTY)
    {
        if (!packet.EndOfStream())
            return MSG_ERROR_SIZE;

        boost::shared_ptr<Party> party = inv_player->get_party();

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
            pkt->Write<uint32_t>(inv_player->GetUniqueID());
            m_server->Send(pkt,inv_player->ConnectionID());
        }

        pkt.reset(new OPacket);

        srv_pkt::PartyAccepted(pkt,player->GetUniqueID());
        m_connection->Send(pkt);

        player->join_party(party);

        pkt.reset(new OPacket);

        srv_pkt::PartyInfo(pkt,party);
        m_connection->Send(pkt);

        connect_party_signals();
    }
    else if (answer == REJECT_PARTY)
    {
        uint8_t type = packet.Read<uint8_t>();

        if (!packet.EndOfStream())
            return MSG_ERROR_SIZE;

        if (type != 0x2C)
            return MSG_ERROR_ARG;

        pkt.reset(new OPacket);

        srv_pkt::PartyInvite(pkt,srv_pkt::PARTY_ERROR_REJECTED);
        m_connection->Send(pkt);

        pkt.reset(new OPacket);

        srv_pkt::PartyInvite(pkt,srv_pkt::PARTY_ERROR_REQUEST_DENY);
        m_server->Send(pkt,inv_player->ConnectionID());

        if (inv_player->get_party()->get_member_count() == 1)
            inv_player->clear_party();
    }
    else
        return MSG_ERROR_ARG;

    return MSG_SUCCESS;
}

int StateGame::OnInvite_Guild (IPacket &packet, const boost::shared_ptr<Player> &player)
{
    enum
    {
        ACCEPT_GUILD = 0x0101,
        REJECT_GUILD = 0x1602
    };

    uint16_t answer = packet.Read<uint16_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> inv_player = get_invitation_player(player);

    if(!inv_player)
        return MSG_ERROR;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (answer == REJECT_GUILD)
    {
        srv_pkt::GuildInviteDeny(pkt,srv_pkt::INVITE_GUILD_REJECT);
        m_connection->Send(pkt);

        pkt.reset(new OPacket);

        srv_pkt::GuildInviteDenied(pkt,srv_pkt::INVITE_GUILD_REJECT);
        m_server->Send(pkt,inv_player->ConnectionID());
    }
    else if (answer == ACCEPT_GUILD)
    {
        boost::shared_ptr<Guild> guild = inv_player->get_guild();

        if (!guild)
            return MSG_SUCCESS;

        boost::shared_ptr<GuildMember> member = m_server->GetWorld()->GetGuildManager()->CreateMember(guild->get_id(),
            player->level(), player->get_id(),false, player->get_name(),player->get_position().get_zone());

        if (!member)
        {
            syslog(LOG_WARNING,"StateGame::OnInvite_Guild - Unable to allocate GuildMember object.");
            return MSG_ERROR;
        }

        player->set_guild(guild);

        guild->insert(member);

        srv_pkt::GuildJoin(pkt,guild);
        m_connection->Send(pkt);

        pkt.reset(new OPacket);

        srv_pkt::GuildPlayer(pkt,player->GetUniqueID(),guild->get_name(),member,true);
        m_server->SendChannel(pkt,player->get_position().get_zone());

        connect_guild_signals();

        guild->member_state(player->get_name(),false);

        DB::PLAYER::GuildID query;
        query(m_server->DBConnection(),player->ID(),guild->get_id());

        boost::shared_ptr<Party> party = player->get_party();

        if (party)
            party->SendGuildNotification(player->GetUniqueID(),guild->get_name());
    }
    else
        return MSG_ERROR_ARG;

    return MSG_SUCCESS;
}

int StateGame::OnInvite_Alliance (IPacket &packet, const boost::shared_ptr<Player> &player)
{
    enum
    {
        UNION_ACCEPT = 0x0101,
        UNION_REJECT = 0x0002
    };

    uint16_t answer = packet.Read<uint16_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> inv_player = get_invitation_player(player);

    if(!inv_player)
        return MSG_ERROR;

    if (answer == UNION_ACCEPT)
    {
        boost::shared_ptr<Guild> master = inv_player->get_guild();
        boost::shared_ptr<Guild> slave = player->get_guild();

        if (!master || !slave || master->get_id() == slave->get_id())
            return MSG_ERROR;

        DB::GUILD::Union query;
        boost::shared_ptr<Union> ally;

        if (master->get_union_id())
        {
            ally = m_server->GetWorld()->GetUnionManager()->Find(master->get_union_id());

            if (!ally)
                return MSG_ERROR;

            ally->insert(slave);

            slave->set_union_id(ally->id());
        }
        else
        {
            ally = m_server->GetWorld()->GetUnionManager()->Create(master,slave);

            if (!ally)
                return MSG_ERROR;

            query(m_server->DBConnection(),master->get_id(),ally->id());
        }

        query(m_server->DBConnection(),slave->get_id(),ally->id());
    }
    else if (answer == UNION_REJECT)
    {
    }
    else
        return MSG_ERROR_ARG;

    return MSG_SUCCESS;
}

int StateGame::OnInvite_Academy (IPacket &packet, const boost::shared_ptr<Player> &player)
{
    return MSG_SUCCESS;
}

boost::shared_ptr<Player> StateGame::get_invitation_player (const boost::shared_ptr<Player> &player) const
{
    boost::shared_ptr<Player> target;

    if (player)
    {
        uint32_t ID = player->get_invitation_player();

         target = m_server->GetWorld()->FindPlayer_ByID(ID);
     }

    return target;
}
