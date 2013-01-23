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

/// SRLF INCLUDES
#include <player.hpp>
#include <transport.hpp>
#include <dropable_item.hpp>

/// SRNL INCLUDES
#include <packet_npc.hpp>
#include <packet_transport.hpp>

#if defined DEBUG
#include <packet_util_functions.hpp>
#endif

#define MAXIMUM_MOUNT_DISTANCE 15

int StateGame::OnTransportMovement (IPacket &packet)
{
    enum
    {
        ACTION_MOVE = 1,
        ACTION_ROTATE = 4
    };

    enum
    {
        ACTION_MOVE_SKY_CLICK,
        ACTION_MOVE_POSITION
    };

    uint32_t transportID = packet.Read<uint32_t>();

    uint8_t action = packet.Read<uint8_t>();

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Transport> transport = player->GetTransport();

    if (!transport)
        return MSG_SUCCESS;

    if (transport->GetUniqueID() != transportID)
        return MSG_ERROR_ARG;

    if (transport->IsDead())
        return MSG_SUCCESS;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    switch(action)
    {
    case ACTION_MOVE:
        {
            uint8_t action_move = packet.Read<uint8_t>();

            if (action_move)
            {
                Coord new_pos;

                int16_t zone = packet.Read<int16_t>();

                int16_t xpos = packet.Read<int16_t>();

                int16_t zpos = packet.Read<int16_t>();

                int16_t ypos = packet.Read<int16_t>();

                if (!packet.EndOfStream())
                    return MSG_ERROR_SIZE;

#if defined DEBUG
                syslog (LOG_INFO,"OnTransportMovement - %i, %i %i %i",zone,xpos,zpos,ypos);
#endif

                new_pos = Coord(xpos,ypos,zpos,zone);

                if (!transport->IsMounted())
                    return MSG_SUCCESS;

                if (transport->hasStatus(STATUS_GROUP_RESTRAIN))
                    return MSG_SUCCESS;

                Coord mpos;
                if (!m_server->GetWorld()->resolveMotion(transport->get_position(),new_pos,&mpos))
                    return MSG_SUCCESS;

                new_pos = mpos;

                if  (player->GetCurrentTask() != PLAYER_TASK_MOVE_TRANSPORT)
                {
                    m_server->GetWorld()->TerminateTask(player->GetUniqueID());

                    player->SetCurrentTask(PLAYER_TASK_MOVE_TRANSPORT);
                }

                transport->set_destination(new_pos);

                player->set_destination(new_pos);

                m_server->GetWorld()->InitTaskMove(player->GetUniqueID());

                srv_pkt::Movement(pkt,transportID,new_pos);
                m_server->SendChannel(pkt,player->get_position().get_zone());
            }
            else if (action_move == ACTION_MOVE_SKY_CLICK)
            {
                uint8_t unk = packet.Read<uint8_t>();

                int16_t angle = packet.Read<int16_t>();

                if (!packet.EndOfStream())
                    return MSG_ERROR_SIZE;

#if defined DEBUG
                syslog(LOG_INFO,"SkyClick Transport - Angle = %i",angle*360/0x10000);
#endif
            }
            else
                return MSG_SUCCESS;
        }
        break;
    case ACTION_ROTATE:
        {
            int16_t angle = packet.Read<int16_t>();

            if (!packet.EndOfStream())
                return MSG_ERROR_SIZE;

#if defined DEBUG
            syslog(LOG_INFO,"Horse rotate, angle = %i",angle*360/0x10000);
#endif

            if (!transport->IsMounted())
                return MSG_ERROR_ORDER;

            srv_pkt::Rotate(pkt,transport->GetUniqueID(),angle);
            m_server->SendChannel(pkt,transport->get_position().get_zone());
        }
        break;
    default:
#if defined DEBUG
        PrintPacketToSyslog(packet);
#endif
        break;
    }

    return MSG_SUCCESS;
}

int StateGame::OnTransportAction (IPacket &packet)
{
    uint8_t action = packet.Read<uint8_t>();

    uint32_t transportID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Transport> transport = player->GetTransport();

    if (!transport)
        return MSG_ERROR;

    if (transport->GetUniqueID() != transportID || action > srv_pkt::TRANSPORT_ACTION_MOUNT)
        return MSG_ERROR_ARG;

    if (transport->type() != ET_COS_TRANSPORT && transport->type() != ET_COS_VEHICLE)
        return MSG_SUCCESS;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (action == srv_pkt::TRANSPORT_ACTION_UNMOUNT)
    {
        if (!transport->IsMounted())
            return MSG_SUCCESS;

        srv_pkt::Speed(pkt,player);
        m_server->SendChannel(pkt,player->get_position().get_zone());

        pkt.reset(new OPacket);

        srv_pkt::TransportAction(pkt,player->GetUniqueID(),srv_pkt::TRANSPORT_ACTION_UNMOUNT,transportID);
        m_server->SendChannel(pkt,player->get_position().get_zone());

        if (transport->type() == ET_COS_VEHICLE)
        {
            pkt.reset(new OPacket);

            srv_pkt::TransportStatsRemove(pkt,transportID);
            m_connection->Send(pkt);

            m_server->GetWorld()->RemoveNPC(transportID,transport->get_position());

            player->m_transport.reset();
        }
        else
        {
            transport->Dismount();

            m_server->GetWorld()->InitTaskTraceNPC(transport,player->GetUniqueID());
        }
    }
    else
    {
        if (transport->type() != ET_COS_TRANSPORT || transport->IsMounted())
            return MSG_ERROR_ARG;

        if (distance(transport->get_position(),player->get_position()) <= MAXIMUM_MOUNT_DISTANCE)
        {
            srv_pkt::TransportAction(pkt,srv_pkt::MSG_ERROR_TRANSPORT_TO_FAR);
            m_connection->Send(pkt);
            return MSG_SUCCESS;
        }
        else
        {
            if (player->get_attack_state() != ATTACK_ACTION_STOP)
            {
                srv_pkt::TransportAction(pkt,srv_pkt::MSG_ERROR_STATE_ATTACK);
                m_connection->Send(pkt);
                return MSG_SUCCESS;
            }

            transport->Mount();

            srv_pkt::TransportAction(pkt,player->GetUniqueID(),srv_pkt::TRANSPORT_ACTION_MOUNT,transportID);
            m_server->SendChannel(pkt,player->get_position().get_zone());

            m_server->GetWorld()->TerminateTask(transport->GetUniqueID());
        }
    }

    if (player->GetCurrentTask() == PLAYER_TASK_MOVE_TRANSPORT)
        m_server->GetWorld()->TerminateTask(player->GetUniqueID());

    return MSG_SUCCESS;
}

int StateGame::OnTransportTerminate (IPacket &packet)
{
    uint32_t transportID = packet.Read<uint32_t>();

    if(!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Transport> transport = player->GetTransport();

    if (!transport || transport->GetUniqueID() != transportID)
        return MSG_ERROR_ARG;

    if (player->GetCurrentTask() == PLAYER_TASK_MOVE_TRANSPORT)
        m_server->GetWorld()->TerminateTask(player->GetUniqueID());
    else
        m_server->GetWorld()->TerminateTask(transport->GetUniqueID());

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::Speed(pkt,player);
    m_server->SendChannel(pkt,player->get_position().get_zone());

    pkt.reset(new OPacket);

    srv_pkt::TransportAction(pkt,player->GetUniqueID(),srv_pkt::TRANSPORT_ACTION_UNMOUNT,transportID);
    m_server->SendChannel(pkt,player->get_position().get_zone());

    pkt.reset(new OPacket);

    srv_pkt::TransportStatsRemove(pkt,transportID);
    m_connection->Send(pkt);

    m_server->GetWorld()->RemoveNPC(transportID,transport->get_position());

    pkt.reset(new OPacket);

    srv_pkt::TransportTerminate(pkt);
    m_connection->Send(pkt);

    if (transport->type() == ET_COS_TRANSPORT)
    {
        Job job = player->get_job();

        uint32_t accountID = static_cast<srv::Connection*>(m_connection)->AccountID();

        boost::shared_ptr<DropableItem> item;
        boost::shared_ptr<Storage> storage = transport->GetStorage();

        if (storage)
        {
            Coord new_pos;

            for (Storage::iterator it = storage->begin(); it != storage->end(); ++it)
            {
                item.reset(new DropableItem());

                if (item)
                {
                    /// DROP ITEMS IN A VALID POSITION
                    Coord mpos;

                    do
                    {
                        new_pos = player->get_position();

                        new_pos.set_x(new_pos.get_x() + (rand()%10)*sin((rand()%360)*3.1415/180));
                        new_pos.set_y(new_pos.get_y() + (rand()%10)*sin((rand()%360)*3.1415/180));
                    }
                    while (!m_server->GetWorld()->resolveMotion(player->get_position(),new_pos,&mpos));

                    new_pos = mpos;

                    item->m_Item = it->second;
                    item->m_OwnerAlias = job.get_alias();
                    item->m_OwnerAuth = accountID;
                    item->m_OwnerID = player->GetUniqueID();
                    item->set_position(new_pos);

                    m_server->GetWorld()->InsertItem(item);
                }
            }
        }
    }

    transport.reset();
    player->SetTransport(transport);

    player->SetCurrentTask(PLAYER_TASK_IDLE);

    return MSG_SUCCESS;
}
