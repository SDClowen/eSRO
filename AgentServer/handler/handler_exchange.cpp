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

#include <player.hpp>
#include <storage.hpp>
#include <exchange_window.hpp>

/// SRNL INCLUDES
#include <packet_exchange.hpp>
#include <packet_invitation.hpp>

int StateGame::OnExchangeRequest (IPacket &packet)
{
    uint32_t targetID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    World *world = m_server->GetWorld();

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (player->get_selected_object() != targetID)
        return MSG_ERROR_ARG;

    boost::shared_ptr<Player> target = world->FindPlayer_ByID(targetID);

    if(!target)
        return MSG_SUCCESS;

    player->set_invitation_player(targetID);

    target->set_invitation_player(player->GetUniqueID());
    target->set_invitation(INVITATION_EXCHANGE);

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::Invite(pkt,INVITATION_EXCHANGE,player->GetUniqueID());
    m_server->Send(pkt,target->ConnectionID());

    return MSG_SUCCESS;
}

int StateGame::OnExchangeConfirm (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Player> target = get_invitation_player(player);

    if(!target)
        return MSG_SUCCESS;

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::ExchangeConfirm(pkt);
    m_connection->Send(pkt);

    pkt.reset(new OPacket);

    srv_pkt::ExchangePlayerConfirm(pkt);
    m_server->Send(pkt,target->ConnectionID());

    return MSG_SUCCESS;
}

int StateGame::OnExchangeApprove (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;
    
    boost::shared_ptr<Player> target = get_invitation_player(player);

    if(!target)
        return MSG_SUCCESS;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    srv_pkt::ExchangeApprove(pkt);
    m_connection->Send(pkt);

    player->set_exchange_approval(true);

    if (target->is_exchange_approved())
    {
        pkt.reset(new OPacket);

        srv_pkt::ExchangeCompleted(pkt);

        m_connection->Send(pkt,false,false);
        m_server->Send(pkt,target->ConnectionID());

        boost::shared_ptr<Storage> my_inv = player->get_storage(STORAGE_INVENTORY);

        boost::shared_ptr<ExchangeWnd> my_exchange = player->get_exchange_window();

        boost::shared_ptr<Storage> other_inv = target->get_storage(STORAGE_INVENTORY);

        boost::shared_ptr<ExchangeWnd> other_exchange = target->get_exchange_window();

        //Exchange players gold.
        player->gain_gold(other_exchange->get_gold_amount());
        player->spend_gold(my_exchange->get_gold_amount());

        target->gain_gold(my_exchange->get_gold_amount());
        target->spend_gold(other_exchange->get_gold_amount());

        //Exchange players items.
        ExchangeWnd::iterator iter;

        for (iter = my_exchange->begin(); iter != my_exchange->end(); ++iter)
            my_inv->RemoveItem(iter->second.get_alt_position());

        for (iter = other_exchange->begin(); iter != other_exchange->end(); ++iter)
            other_inv->RemoveItem(iter->second.get_alt_position());

        uint8_t new_pos;

        for (iter = my_exchange->begin(); iter != my_exchange->end(); ++iter)
        {
            new_pos = other_inv->get_empty_position(13);

            iter->second.set_position(new_pos);

            other_inv->InsertItem(iter->second);
        }

        for (iter = other_exchange->begin(); iter != other_exchange->end(); ++iter)
        {
            new_pos = my_inv->get_empty_position(13);

            iter->second.set_position(new_pos);

            my_inv->InsertItem(iter->second);
        }
    }

    return MSG_SUCCESS;
}

int StateGame::OnExchangeClose (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    bool closed = false;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (closed)
    {
        player->set_invitation_player(0);

        player->close_exchange();
    }

    boost::shared_ptr<OPacket> pkt(new OPacket);

    srv_pkt::ExchangeClose(pkt,closed);
    m_connection->Send(pkt);

    boost::shared_ptr<Player> target = get_invitation_player(player);

    if (target)
    {
        target->set_invitation_player(0);

        target->close_exchange();

        pkt.reset(new OPacket);

        srv_pkt::ExchangeClose(pkt,closed);
        m_server->Send(pkt,target->ConnectionID());
    }

    return MSG_SUCCESS;
}
