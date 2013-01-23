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
#include "stall_manager.hpp"

/// SRLF INCLUDES
#include <player.hpp>
#include <stall.hpp>

/// SRNL INCLUDES
#include <packet_stall.hpp>
#include <packet_item.hpp>
#include <packet_msg.hpp>

#if defined DEBUG
#include <packet_util_functions.hpp>
#endif

int StateGame::OnStallCreate (IPacket &packet)
{
    std::string name = packet.Read();    ///ON ISRO THE NAME IS UNICODE | ON KSRO IS NORMAL STRING

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    if (name.length() > 50)
        return MSG_ERROR_ARG;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (!player->IsTaskInterruptable() || player->IsDead())
        return MSG_SUCCESS;

    player->create_stall(name);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    srv_pkt::StallSpawn(pkt,player->GetUniqueID(),name,player->get_stall_avatar());
    m_server->SendChannel(pkt,player->get_position().get_zone());

    pkt.reset(new OPacket);

    srv_pkt::StallOpen(pkt);
    m_connection->Send(pkt);

    connect_stall_signals();

    return MSG_SUCCESS;
}

int StateGame::OnStallAction (IPacket &packet)
{
    enum
    {
        STALL_ACTION_MODIFY = 1,
        STALL_ACTION_ADD = 2,
        STALL_ACTION_REMOVE,
        STALL_ACTION_STATUS = 5,
        STALL_ACTION_GREETING,
        STALL_ACTION_NAME
    };

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Stall> stall = player->get_stall();

    if (!stall || stall->get_owner() != player->GetUniqueID())
        return MSG_ERROR_ORDER;

    uint8_t action = packet.Read<uint8_t>();

    if (action == STALL_ACTION_MODIFY)
    {
        uint8_t pos = packet.Read<uint8_t>();

        uint16_t quantity = packet.Read<uint16_t>();

        uint64_t price = packet.Read<uint64_t>();

        if (!packet.EndOfStream())
            return MSG_ERROR_SIZE;

        stall->change_item(pos,quantity,price);
    }
    else if (action == STALL_ACTION_ADD)
    {
        uint8_t pos = packet.Read<uint8_t>();

        uint8_t inv_pos = packet.Read<uint8_t>();

        uint16_t quantity = packet.Read<uint16_t>();

        uint64_t price = packet.Read<uint64_t>();

        uint32_t unk1 = packet.Read<uint32_t>(); //<--- ITEM RELATED!!!

        uint16_t unk2 = packet.Read<uint16_t>();

        if (!packet.EndOfStream())
            return MSG_ERROR_SIZE;

        boost::shared_ptr<Storage> st = player->get_storage(STORAGE_INVENTORY);

        if (!st)
            return MSG_ERROR;

        Item itm = st->GetItem(inv_pos);

        if (!itm.is_tradable())
        {
            return MSG_SUCCESS;
        }

        itm.set_position(pos);

        itm.set_alt_position(inv_pos);

        itm.set_stall_quantity(quantity);

        itm.set_stall_price(price);

        stall->InsertItem(itm);
    }
    else if (action == STALL_ACTION_REMOVE)
    {
        uint8_t pos = packet.Read<uint8_t>();

        uint8_t inv_pos = packet.Read<uint8_t>();

        if (!packet.EndOfStream())
            return MSG_ERROR_SIZE;

        stall->RemoveItem(pos);
    }
    else if (action == STALL_ACTION_STATUS)
    {
        uint8_t state = packet.Read<uint8_t>();

        uint8_t network_register = packet.Read<uint8_t>();

        uint8_t unk = packet.Read<uint8_t>();

        if (!packet.EndOfStream())
            return MSG_ERROR_SIZE;

        stall->change_state();
    }
    else if (action == STALL_ACTION_GREETING)
    {
        std::string greeting = packet.Read();    ///ON ISRO THE NAME IS UNICODE | ON KSRO IS NORMAL STRING

        if (!packet.EndOfStream())
            return MSG_ERROR_SIZE;

        stall->set_greeting(greeting);
    }
    else if (action == STALL_ACTION_NAME)
    {
        std::string name = packet.Read();       ///ON ISRO THE NAME IS UNICODE | ON KSRO IS NORMAL STRING

        if (!packet.EndOfStream())
            return MSG_ERROR_SIZE;

        stall->set_name(name);

        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::StallName(pkt,name,player->GetUniqueID());
        m_server->SendChannel(pkt,player->get_position().get_zone());

        pkt.reset(new OPacket);

        srv_pkt::StallAction(pkt,action);
        m_server->SendChannel(pkt,player->get_position().get_zone());
    }

    return MSG_SUCCESS;
}

int StateGame::OnStallEnter (IPacket &packet)
{
    uint32_t playerID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (!player->IsTaskInterruptable() || player->IsDead())
        return MSG_SUCCESS;

    if (playerID != player->get_selected_object())
        return MSG_ERROR_ARG;

    boost::shared_ptr<Player> stall_player;
    stall_player = m_server->GetWorld()->FindPlayer_CloseToPosition(playerID,player->get_position());

    if(!stall_player)
        return MSG_SUCCESS;

    if (stall_player->is_stalling())
    {
        boost::shared_ptr<Stall> stall = stall_player->get_stall();

        if (!stall)
            return MSG_SUCCESS;

        player->join_stall(stall);

        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::StallEnter(pkt,stall_player->GetUniqueID(),player->GetUniqueID(),stall);
        m_connection->Send(pkt);

        connect_stall_signals();
    }

    return MSG_SUCCESS;
}

int StateGame::OnStallBuy (IPacket &packet)
{
    uint8_t pos = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Stall> stall = player->get_stall();

    if (!stall)
        return MSG_ERROR_ORDER;

    boost::shared_ptr<Player> splayer;
    splayer = m_server->GetWorld()->FindPlayer_ByID(stall->get_owner());

    if (!splayer)
        return MSG_SUCCESS;

    if (pos < 9 && stall)
    {
        boost::shared_ptr<Storage> inv = player->get_storage(STORAGE_INVENTORY);

        uint8_t new_pos = inv->get_empty_position(13);

        boost::shared_ptr<OPacket> pkt(new OPacket);

        if (new_pos == 255)
        {
            srv_pkt::StallBuy(pkt,srv_pkt::ERROR_STALL_INVENTORY_FULL);
            m_connection->Send(pkt);
            return MSG_SUCCESS;
        }

        Item itm = stall->GetItem(pos);

        if (!player->spend_gold(itm.get_stall_price()))
        {
            srv_pkt::StallBuy(pkt,srv_pkt::ERROR_STALL_INSUFFICIENT_GOLD);
            m_connection->Send(pkt);
            return MSG_SUCCESS;
        }

        splayer->gain_gold(itm.get_stall_price());

        srv_pkt::StallBuy(pkt,pos);
        m_connection->Send(pkt);

        stall->buy(player->get_name(),pos);

        itm.set_position(new_pos);

        inv->InsertItem(itm);

        inv = splayer->get_storage(STORAGE_INVENTORY);

        inv->RemoveItem(itm.get_alt_position());

        pkt.reset(new OPacket);
        srv_pkt::Item(pkt,srv_pkt::ITEM_MOVE_REMOVE,itm.get_alt_position(),4);
        m_server->Send(pkt,splayer->ConnectionID());
    }
    else
        return MSG_ERROR_ARG;

    return MSG_SUCCESS;
}

int StateGame::OnStallLeave (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::StallLeave(pkt);
    m_connection->Send(pkt);

    player->leave_stall();

    disconnect_stall_signals();

    return MSG_SUCCESS;
}

int StateGame::OnStallClose (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Stall> stall = player->get_stall();

    if (!stall)
        return MSG_SUCCESS;

    if (stall->get_owner() != player->GetUniqueID())
        return MSG_ERROR_ORDER;

    player->close_stall();

    return MSG_SUCCESS;
}

int StateGame::OnStallNetworkSearch (IPacket &packet)
{
#if defined DEBUG
    PrintPacketToSyslog(packet);
#endif

    uint8_t unk = packet.Read<uint8_t>();

    uint8_t page =  packet.Read<uint8_t>();

    uint32_t type = packet.Read<uint32_t>();

    uint8_t dg =  packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    /*
    Stall Network Find Class Alchemy Type Elixir Degree None
    [<Unlabeled 1423158>][C -> S][76F9][7 bytes]
    01 00 20 00 00 00 00

         Stall Network Find Class Alchemy Type Alchemic Stone Degree 1
    [<Unlabeled 1423158>][C -> S][76F9][7 bytes]
    02 00 22 00 00 00 01

         Stall Network Find Class Alchemy Type Alchemic Stone Degree 2
    [<Unlabeled 1423158>][C -> S][76F9][7 bytes]
    02 00 22 00 00 00 02

         Request Page 2 of Elixirs
    [<Unlabeled 1423158>][C -> S][76F9][7 bytes]
    03 01 20 00 00 00 00

         Request Page 3
    [<Unlabeled 1423158>][C -> S][76F9][7 bytes]
    03 02 20 00 00 00 00

         Alchemy Type Element Degree 1
    [<Unlabeled 1423158>][C -> S][76F9][7 bytes]
    02 00 21 00 00 00 01

         Alchemy Type Alchemic Table Degree 1
    [<Unlabeled 1423158>][C -> S][76F9][7 bytes]
    02 00 24 00 00 00 01
    */

    return MSG_SUCCESS;
}

int StateGame::OnStallNetworkPurchase (IPacket &packet)
{
#if defined DEBUG
    PrintPacketToSyslog(packet);
#endif

    return MSG_SUCCESS;
}

void StateGame::connect_stall_signals ()
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return;

    boost::shared_ptr<Stall> stall = player->get_stall();

    if (stall)
    {
        stall->signal_greeting.connect(boost::bind(&StateGame::OnStallEvent_Greeting,this,_1));

        stall->signal_enter.connect(boost::bind(&StateGame::OnStallEvent_Enter,this,_1,_2));

        stall->signal_leave.connect(boost::bind(&StateGame::OnStallEvent_Leave,this,_1));

        stall->signal_buy.connect(boost::bind(&StateGame::OnStallEvent_Buy,this,_1,_2));

        stall->signal_insert.connect(boost::bind(&StateGame::OnStallEvent_Insert,this,_1,_2));

        stall->signal_remove.connect(boost::bind(&StateGame::OnStallEvent_Remove,this,_1));

        stall->signal_msg.connect(boost::bind(&StateGame::OnStallEvent_Msg,this,_1,_2));

        stall->signal_price.connect(boost::bind(&StateGame::OnStallEvent_Price,this,_1,_2,_3));

        stall->signal_state.connect(boost::bind(&StateGame::OnStallEvent_State,this,_1));

        if ( stall->get_owner() == player->GetUniqueID())
                stall->signal_close.connect(boost::bind(&StateGame::OnStallEvent_Close,this));
    }
}

void StateGame::disconnect_stall_signals ()
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return;

    boost::shared_ptr<Stall> stall = player->get_stall();

    if (stall)
    {
        stall->signal_greeting.disconnect(boost::bind(&StateGame::OnStallEvent_Greeting,this,_1));

        stall->signal_enter.disconnect(boost::bind(&StateGame::OnStallEvent_Enter,this,_1,_2));

        stall->signal_leave.disconnect(boost::bind(&StateGame::OnStallEvent_Leave,this,_1));

        stall->signal_buy.disconnect(boost::bind(&StateGame::OnStallEvent_Buy,this,_1,_2));

        stall->signal_insert.disconnect(boost::bind(&StateGame::OnStallEvent_Insert,this,_1,_2));

        stall->signal_remove.disconnect(boost::bind(&StateGame::OnStallEvent_Remove,this,_1));

        stall->signal_msg.disconnect(boost::bind(&StateGame::OnStallEvent_Msg,this,_1,_2));

        stall->signal_price.disconnect(boost::bind(&StateGame::OnStallEvent_Price,this,_1,_2,_3));

        stall->signal_state.disconnect(boost::bind(&StateGame::OnStallEvent_State,this,_1));

        if ( stall->get_owner() == player->GetUniqueID())
            stall->signal_close.disconnect(boost::bind(&StateGame::OnStallEvent_Close,this));
    }
}

void StateGame::OnStallEvent_Greeting (const std::string& greeting)
{
    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::StallAction(pkt,greeting);
    m_connection->Send(pkt);
}

void StateGame::OnStallEvent_State (const bool is_open)
{
    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (is_open)
        srv_pkt::StallAction(pkt,(uint16_t)0x0101);
    else
        srv_pkt::StallAction(pkt,(uint16_t)0x0300);

    m_connection->Send(pkt);
}

void StateGame::OnStallEvent_Enter (const uint32_t playerID, const std::string &name)
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (player)
    {
        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::StallNotify(pkt,srv_pkt::STALL_ACTION_ENTER,playerID);
        m_connection->Send(pkt);
    }
}

void StateGame::OnStallEvent_Leave (const uint32_t playerID)
{
    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::StallNotify(pkt,srv_pkt::STALL_ACTION_LEAVE,playerID);
    m_connection->Send(pkt);
}

void StateGame::OnStallEvent_Buy (const uint8_t pos,const std::string &name)
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (player)
    {
        boost::shared_ptr<Stall> stall = player->get_stall();

        if (stall)
        {
            boost::shared_ptr<OPacket> pkt(new OPacket);
            srv_pkt::StallNotify(pkt,name,stall);
            m_connection->Send(pkt);
        }
    }
}

void StateGame::OnStallEvent_Price (const uint8_t pos, const uint16_t quantity, const uint64_t price)
{
    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::StallAction(pkt,pos,quantity,price);
    m_connection->Send(pkt);
}

void StateGame::OnStallEvent_Close ()
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (player)
    {
        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::StallDespawn(pkt,player->GetUniqueID());
        m_server->SendChannel(pkt,player->get_position().get_zone());

        pkt.reset(new OPacket);

        srv_pkt::StallClose(pkt);
        m_connection->Send(pkt);

        disconnect_stall_signals();
    }
}

void StateGame::OnStallEvent_Msg (const std::string &name, const std::string &msg)
{
    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::Msg(pkt,srv_pkt::MSG_STALL,name,msg);
    m_connection->Send(pkt);
}

uint32_t StateGame::OnStallEvent_Insert (const Item &item, const STORAGE_TYPE type)
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (player)
    {
        boost::shared_ptr<Stall> stall = player->get_stall();

        if (stall)
        {
            boost::shared_ptr<OPacket> pkt(new OPacket);
            srv_pkt::StallItem(pkt,stall, srv_pkt::STALL_INSERT_ITEM);
            m_connection->Send(pkt);
        }
    }

    return 0;
}

void StateGame::OnStallEvent_Remove (const uint32_t itemID)
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (player)
    {
        boost::shared_ptr<Stall> stall = player->get_stall();

        if (stall)
        {
            boost::shared_ptr<OPacket> pkt(new OPacket);
            srv_pkt::StallItem(pkt,stall,srv_pkt::STALL_REMOVE_ITEM);
            m_connection->Send(pkt);
        }
    }
}
