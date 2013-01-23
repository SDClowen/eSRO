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
#include "manager/union_manager.hpp"

///SRLF INCLUDES
#include <player.hpp>
#include <stall.hpp>
#include <guild.hpp>
#include <guild_member.hpp>
#include <union.hpp>
#include <academy.hpp>
#include <party.hpp>

/// SRNL INCLUDES
#include <packet_msg.hpp>

int StateGame::OnMsg (IPacket &packet)
{
    uint8_t type = packet.Read<uint8_t>();

    uint8_t num_msgs = packet.Read<uint8_t>();

    uint8_t link_count = packet.Read<uint8_t>();

    uint8_t ret;

    switch(type)
    {
    case srv_pkt::MSG_PUBLIC:
    case srv_pkt::MSG_PUBLIC_MULTIBYTE:
        ret = OnMsgPublic(packet,num_msgs,link_count);
        break;
    case srv_pkt::MSG_PRIVATE:
        ret = OnMsgPrivate(packet,num_msgs,link_count);
        break;
    case srv_pkt::MSG_PARTY:
        ret = OnMsgParty(packet, num_msgs,link_count);
        break;
    case srv_pkt::MSG_GUILD:
        ret = OnMsgGuild(packet, num_msgs,link_count);
        break;
    case srv_pkt::MSG_STALL:
        ret = OnMsgStall(packet,num_msgs,link_count);
        break;
    case srv_pkt::MSG_NOTICE:
        ret = OnMsgNotice(packet,num_msgs,link_count);
        break;
    case srv_pkt::MSG_ACADEMY:
        ret = OnMsgAcademy(packet,num_msgs,link_count);
        break;
    case srv_pkt::MSG_UNION:
        ret = OnMsgUnion(packet,num_msgs,link_count);
        break;
    default:
#if defined DEBUG
        syslog(LOG_INFO,"Unknow Msg type (%i).",type);
#endif
        ret = MSG_SUCCESS;
        break;
    }

    return MSG_SUCCESS;
}

int StateGame::OnMsgPublic(IPacket &packet, const uint8_t num_msgs, const uint8_t linkCount)
{
    std::string msg = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::Msg(pkt,player->GetUniqueID(),msg);
    m_server->SendChannel(pkt,player->get_position().get_zone());

    pkt.reset(new OPacket);

    srv_pkt::Msg(pkt,srv_pkt::MSG_PUBLIC,num_msgs);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnMsgPrivate(IPacket &packet, const uint8_t num_msgs, const uint8_t linkCount)
{
    std::string name = packet.Read();

    std::string msg = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Player> dst = m_server->GetWorld()->FindPlayer_ByName(name);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (dst)
    {
        srv_pkt::Msg(pkt,srv_pkt::MSG_PRIVATE,player->get_name(),msg);
        m_server->Send(pkt,dst->ConnectionID());

        pkt.reset(new OPacket);

        srv_pkt::Msg(pkt,srv_pkt::MSG_PRIVATE,num_msgs);
        m_connection->Send(pkt);
    }
    else
    {
            /*
            //TODO MOVE TO PACKET OBJECT!!
            OPacket p;
            p.WriteOpcode(0xB367);
            p.Write<uint8_t>(2);
            p.Write<uint8_t>(3);
            p.Write<uint8_t>(2);
            p.Write<uint8_t>(num_msgs);

            send(p);
            */
    }

    return MSG_SUCCESS;
}

int StateGame::OnMsgParty (IPacket &packet, const uint8_t num_msgs, const uint8_t linkCount)
{
    std::string msg = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Party> party = player->get_party();

    if (party)
    {
            party->send_msg(player->get_name(),msg);

            boost::shared_ptr<OPacket> pkt(new OPacket);
            srv_pkt::Msg(pkt,srv_pkt::MSG_PARTY,num_msgs);
            m_connection->Send(pkt);
    }
    else
        return MSG_ERROR_ORDER;

    return MSG_SUCCESS;
}

int  StateGame::OnMsgGuild (IPacket &packet, const uint8_t num_msgs, const uint8_t linkCount)
{
    std::string msg = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild)
        return MSG_ERROR;

    if (guild->send_msg(player->get_name(),msg))
    {
        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::Msg(pkt,srv_pkt::MSG_GUILD,num_msgs);
        m_connection->Send(pkt);
    }

    return MSG_SUCCESS;
}

int StateGame::OnMsgStall (IPacket &packet, const uint8_t num_msgs, const uint8_t linkCount)
{
    std::string msg = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Stall> stall = player->get_stall();

    if (stall)
    {
        stall->send_msg(player->get_name(),msg);

        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::Msg(pkt,srv_pkt::MSG_STALL,num_msgs);
        m_connection->Send(pkt);
    }

    return MSG_SUCCESS;
}

int StateGame::OnMsgNotice (IPacket &packet, const uint8_t num_msgs, const uint8_t linkCount)
{
    std::string msg = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (!player->is_gm())
        return MSG_ERROR_ORDER;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    srv_pkt::Msg(pkt,msg);
    m_server->Broadcast(pkt);

    pkt.reset(new OPacket);

    srv_pkt::Msg(pkt,srv_pkt::MSG_NOTICE,num_msgs);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnMsgAcademy (IPacket &packet, const uint8_t num_msgs, const uint8_t linkCount)
{
    std::string msg = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    srv_pkt::Msg(pkt,srv_pkt::MSG_ACADEMY,num_msgs);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnMsgUnion (IPacket &packet, const uint8_t num_msgs, const uint8_t linkCount)
{
    std::string msg = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    World *world = m_server->GetWorld();

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild || !guild->get_union_id())
        return MSG_ERROR_ORDER;

    ///CHECK THAT THE PLAYER HAVE THE PERMISSION TO TALK IN UNION
    Guild::const_iterator member_iter = guild->find(player->get_name());

    if (member_iter == guild->end())
        return MSG_ERROR_ORDER;

    if (!member_iter->second->CanUnion())
    {
        syslog(LOG_INFO,"Player %s, dont have union rights.",player->get_name().c_str());
        return MSG_SUCCESS;
    }

    boost::shared_ptr<Union> ally = world->GetUnionManager()->Find(guild->get_union_id());

    if (!ally)
        return MSG_ERROR;

    ally->send_msg(player->get_name(),msg);

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::Msg(pkt,srv_pkt::MSG_UNION,num_msgs);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}
