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
#include "item_factory.hpp"
#include "character_factory.hpp"

/// SRLF INCLUDES
#include <player.hpp>
#include <storage.hpp>

/// SRNL INCLUDES
#include <packet_item.hpp>
#include <packet_spawn.hpp>
#include <packet_msg.hpp>

#if defined DEBUG
#include <packet_util_functions.hpp>
#endif

enum GM_COMMAND
{
    GM_COMMAND_LOAD_MONSTER = 6,
    GM_COMMAND_CREATE_ITEM,
    GM_COMMAND_MOVE_TO_USER,
    GM_COMMAND_BAN_USER = 13,
    GM_COMMAND_RECALL_USER = 17,
    GM_COMMAND_RECALL_GUILD,
    GM_COMMAND_KILL = 20,
    GM_COMMAND_BLOCK_LOGOUT = 23,
    GM_COMMAND_ALLOW_LOGOUT,
    GM_COMMAND_LIE_NAME,
    GM_COMMAND_SPAWN_ALL_UNIQUES = 43,
    GM_COMMAND_CLEAR_INVENTORY = 49
};

int StateGame::OnGMCommand (IPacket &packet)
{
    uint16_t command = packet.Read<uint16_t>();

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player->is_gm())
        return MSG_ERROR;

    switch(command)
    {
    case GM_COMMAND_LOAD_MONSTER:
        OnGMCommand_LoadMonster(packet);
        break;
    case GM_COMMAND_CREATE_ITEM:
        OnGMCommand_CreateItem(packet);
        break;
    case GM_COMMAND_MOVE_TO_USER:
        OnGMCommand_MoveToUser(packet);
        break;
    case GM_COMMAND_RECALL_USER:
        OnGMCommand_RecallUser(packet);
        break;
    case GM_COMMAND_RECALL_GUILD:
        OnGMCommand_RecallGuild(packet);
        break;
    case GM_COMMAND_KILL:
        OnGMCommand_Kill(packet);
        break;
    case GM_COMMAND_BLOCK_LOGOUT:
        OnGMCommand_BlockLogout(packet);
        break;
    case GM_COMMAND_ALLOW_LOGOUT:
        OnGMCommand_AllowLogout(packet);
        break;
    case GM_COMMAND_LIE_NAME:
        OnGMCommand_LieName(packet);
        break;
    case GM_COMMAND_SPAWN_ALL_UNIQUES:
        OnGMCommand_SpawnAllUniques(packet);
        break;
    case GM_COMMAND_CLEAR_INVENTORY:
        OnGMCommand_ClearInventory(packet);
        break;
    default:
#if defined DEBUG
        syslog(LOG_INFO,"Command %i, not implemented.",command);
        PrintPacketToSyslog(packet);
#endif
        break;
    }

    return MSG_SUCCESS;
}

int StateGame::OnGMCommand_LoadMonster (IPacket &packet)
{
    uint32_t ID = packet.Read<uint32_t>();

    uint8_t quantity = packet.Read<uint8_t>();

    uint8_t type = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    if (!quantity)
        quantity = 1;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if(m_server->GetWorld()->IsInsideTown(player->get_position()))
        return MSG_SUCCESS;

    boost::shared_ptr<NPC> mob;
    std::vector<boost::shared_ptr<NPC> > list;

    World *world = m_server->GetWorld();

    for (uint16_t i = 0; i < quantity; ++i)
    {
        mob = world->GetCharacterFactory()->Create(ID);

        if (!mob)
        {
            syslog(LOG_INFO,"Failed to create NPC with ID = %i",ID);
            return MSG_SUCCESS;
        }

        mob->set_position(player->get_position());

        list.push_back(mob);
    }

    world->InsertNPC(list,player->get_position());

    return MSG_SUCCESS;
}

int StateGame::OnGMCommand_CreateItem (IPacket &packet)
{
    uint32_t itemID = packet.Read<uint32_t>();

    uint8_t arg = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    World *world = m_server->GetWorld();

    if (!world)
        return MSG_ERROR;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    try
    {
        Item item = world->GetItemFactory()->Create(itemID);

        boost::shared_ptr<Storage> inv = player->get_storage(STORAGE_INVENTORY);

        uint8_t pos = inv->get_empty_position(13);

        if (pos == 255)
        {
            srv_pkt::ItemMove(pkt,srv_pkt::ITEM_ERROR_INVENTORY_FULL);
            m_connection->Send(pkt);
            return MSG_SUCCESS;
        }

        item.set_quantity(1);
        item.set_position(pos);
        item.set_place(STORAGE_INVENTORY);

        if (item.IsWeapon() || item.IsShield() || item.IsArmor() || item.IsAccessory())
            item.set_enchant_lv(arg);
        else
            item.set_quantity(arg);

        inv->InsertItem(item);

        srv_pkt::Item(pkt,srv_pkt::ITEM_MOVE_PICKUP,item);
        m_connection->Send(pkt);
    }
    catch (const Exception::InvalidItemID &error)
    {
        syslog(LOG_INFO,"Failed to create item with ID = %i",itemID);
    }

    return MSG_SUCCESS;
}

int StateGame::OnGMCommand_MoveToUser (IPacket &packet)
{
    std::string name = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Player> target = m_server->GetWorld()->FindPlayer_ByName(name);

    if (target)
    {
        player->SetCurrentTask(PLAYER_TASK_TELEPORT);
        m_server->GetWorld()->InitTaskTeleport(player->GetUniqueID(),target->get_position(),0);
    }

    return MSG_SUCCESS;
}

int StateGame::OnGMCommand_BanUser (IPacket &packet)
{
    std::string name = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    return MSG_SUCCESS;
}

int StateGame::OnGMCommand_RecallUser (IPacket &packet)
{
    std::string name = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    return MSG_SUCCESS;
}

int StateGame::OnGMCommand_RecallGuild (IPacket &packet)
{
    std::string guildName = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    return MSG_SUCCESS;
}

int StateGame::OnGMCommand_Kill (IPacket &packet)
{
    uint32_t entityID = packet.Read<uint32_t>();

    uint16_t action = packet.Read<uint16_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    if (action)
    {
        boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

        if (!player)
            return MSG_ERROR;

        boost::shared_ptr<NPC> npc = m_server->GetWorld()->FindNPC_CloseToPosition(entityID,player->get_position());

        if (npc)
            npc->consume_hp(npc->hp());
    }

    return MSG_SUCCESS;
}

int StateGame::OnGMCommand_BlockLogout (IPacket &packet)
{
    std::string playerName = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    return MSG_SUCCESS;
}

int StateGame::OnGMCommand_AllowLogout (IPacket &packet)
{
    std::string playerName = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    return MSG_SUCCESS;
}

int StateGame::OnGMCommand_LieName (IPacket &packet)
{
    std::string name = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    return MSG_SUCCESS;
}

int StateGame::OnGMCommand_SpawnAllUniques (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    return MSG_SUCCESS;
}

int StateGame::OnGMCommand_ClearInventory (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

//    boost::shared_ptr<Storage> inv = m_player->get_storage(STORAGE_INVENTORY);
//
//    for (Storage::iterator iter = inv->begin(); iter != inv->end(); ++iter)
//    {
//        if (iter->first >= Item::MAX_EQUIPMENT_SLOT)
//        {
//            send(packet::ItemEnchant(iter->first)); ///REMOVE ITEM FROM INVENTORY
//
//            inv->RemoveItem(iter->first);
//        }
//    }

    return MSG_SUCCESS;
}
