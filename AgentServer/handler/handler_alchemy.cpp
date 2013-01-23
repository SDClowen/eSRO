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

/// SOL INCLUDES
#include <player.hpp>
#include <storage.hpp>
#include <item_property.hpp>

/// EPL INCLUDES
#include <packet_item.hpp>

#include "query/item_query.hpp"

#if defined DEBUG
#include <packet_util_functions.hpp>
#endif

#define ALCHEMY_OPERATION_TIMEOUT 3

enum
{
    ELIXIR_WEAPON = 100663296,
    ELIXIR_SHIELD = 67108864,
    ELIXIR_PROTECTOR = 16909056,
    ELIXIR_ACCESSORY = 83886080
};

enum
{
    ALCHEMY_OPERATION_CANCEL = 1,
    ALCHEMY_OPERATION_EXECUTE
};

int StateGame::OnAlchemyDisjoint (IPacket &packet)
{
#if defined DEBUG
    PrintPacketToSyslog(packet);
#endif
    /*
    [C -> S][77B6]02 01 03 12 1E 18
    ==========================
    02
        [BYTE] Alchemy Type (01 - Alchemy) (03 - Plusing)
        [BYTE] Number of Items
        [BYTE] Slot 1
        [BYTE] Slot 2
        [BYTE] Slot 3
    ==========================

    [S -> C][32A5]13 08 08 00

    [S -> C][B7B6]
    01
    02
    12
    01 00 00 00

    J->C(B3CF)01 0E 14 04 6A 18 00 00 01 00
    J->C(B3CF)01 0E 15 04 76 18 00 00 01 00
    J->C(B3CF)01 0E 16 04 82 18 00 00 01 00
    J->C(B3CF)01 0E 20 04 8E 18 00 00 01 00
    J->C(B3CF)01 0F 12 04
    J->C(32A5)14080200
    J->C(32A5)15080200
    J->C(32A5)16080200
    J->C(32A5)20080200
    J->C(32A5)13080600
    J->C(B7B6)01021E01000000
    J->C(B3CF)010F1E04
    J->C(32A5)13080300
    J->C(B7B6)01021801000000
    J->C(B3CF)010E12046C1800000200
    J->C(B3CF)010E1E04781800000100
    J->C(B3CF)010E2504841800000100
    J->C(B3CF)010E2604901800000200
    J->C(B3CF)010F1804
    */

    return MSG_SUCCESS;
}

int StateGame::OnAlchemyDismantle (IPacket &packet)
{
#if defined DEBUG
    PrintPacketToSyslog(packet);
#endif

    return MSG_SUCCESS;
}

int StateGame::OnAlchemyManufacture (IPacket &packet)
{
#if defined DEBUG
    PrintPacketToSyslog(packet);
#endif

    return MSG_SUCCESS;
}

int StateGame::OnAlchemyEnchant (IPacket &packet)
{
    enum
    {
        ACTION_ENCHANTING_BEGINNER = 3,
        ACTION_ENCHANTING_ADVANCED = 8
    };

    uint8_t operation = packet.Read<uint8_t>();

    switch(operation)
    {
    case ALCHEMY_OPERATION_EXECUTE:
        {
            uint8_t action = packet.Read<uint8_t> ();

            switch(action)
            {
            case ACTION_ENCHANTING_BEGINNER:
                OnAlchemyEnchant_Beginner(packet);
                break;
            case ACTION_ENCHANTING_ADVANCED:
                OnAlchemyEnchant_Advanced(packet);
                break;
            default:
#if defined DEBUG
                 PrintPacketToSyslog(packet);
                 syslog(LOG_INFO,"Unknown operation - OnItemStrengthen - %i",operation);
#endif
                break;
            }
        }
        break;
    case ALCHEMY_OPERATION_CANCEL:
        OnAlchemyEnchant_Cancel(packet);
        break;
    default:
#if defined DEBUG
     PrintPacketToSyslog(packet);
     syslog(LOG_INFO,"Unknown operation - OnItemStrengthen - %i",operation);
#endif
        break;
    }

    return MSG_SUCCESS;
}

int StateGame::OnAlchemyEnchant_Beginner (IPacket &packet)
{
    enum
    {
        MIN_ALCHEMY_ITEMS = 2,
        MAX_ALCHEMY_ITEMS
    };

    enum
    {
        POSITION_ITEM,
        POSITION_ELIXIR,
        POSITION_LUCKY
    };

    uint8_t count = packet.Read<uint8_t>();

    if (count < MIN_ALCHEMY_ITEMS || count > MAX_ALCHEMY_ITEMS)
        return MSG_ERROR_ARG;

    ///	[ITEM POS | ELIXIR POS | LUCKY POS]
    uint8_t pos[MAX_ALCHEMY_ITEMS] = { 0 , 0 , 0 };

    for (int i = 0; i < count; ++i)
        pos[i] = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (player->IsDead() || !player->IsTaskInterruptable())
        return MSG_SUCCESS;

    m_server->GetWorld()->TerminateTask(player->GetUniqueID());

    boost::shared_ptr<Storage> inv = player->get_storage(STORAGE_INVENTORY);

    Storage::iterator item_iter = inv->find(pos[POSITION_ITEM]);

    Storage::iterator elx_iter = inv->find(pos[POSITION_ELIXIR]);

    if (item_iter == inv->end() || elx_iter == inv->end())
    {
#if defined DEBUG
        syslog(LOG_INFO,"Item and/or Elixir dont exist.");
#endif
        return MSG_ERROR_ARG;
    }

    /// CHECK THAT THE ELIXIR CORRESPOND TO THE ITEM TYPE
    boost::shared_ptr<ItemProperty> property = elx_iter->second.property();

    bool valid = false;

    switch(property->Effects[0].first)
    {
    case ELIXIR_WEAPON:
        if (item_iter->second.IsWeapon())
            valid = true;
        break;
    case ELIXIR_SHIELD:
        if (item_iter->second.IsShield())
            valid = true;
        break;
    case ELIXIR_PROTECTOR:
        if (item_iter->second.IsArmor())
            valid = true;
        break;
    case ELIXIR_ACCESSORY:
        if (item_iter->second.IsAccessory())
            valid = true;
        break;
    default:
#if defined DEBUG
        syslog(LOG_INFO,"Unknown elixir type - %i",property->Effects[0].first);
#endif
        break;
    }

    if (pos[POSITION_LUCKY] != 0)
    {
        Storage::iterator lucky_iter = inv->find(pos[POSITION_LUCKY]);

        if (lucky_iter == inv->end())
        {
#if defined DEBUG
            syslog(LOG_INFO,"Lucky dont exist.");
#endif
            return MSG_ERROR_ARG;
        }

        /// CHECK THAT THE LUCKY POWDER DEGREE CORRESPOND TO THE ITEM DEGREE

        uint8_t min_item_dg = 1 + 3*(lucky_iter->second.degree()-1);
        uint8_t max_item_dg = 3*lucky_iter->second.degree();

        if (item_iter->second.degree() < min_item_dg || item_iter->second.degree() > max_item_dg)
            return MSG_ERROR_ORDER;
    }

    if (valid)
    {
        m_server->GetWorld()->InitTaskAlchemy(player->GetUniqueID(),pos[POSITION_ITEM],
            pos[POSITION_ELIXIR],pos[POSITION_LUCKY],false);
    }

    return MSG_SUCCESS;
}

int StateGame::OnAlchemyEnchant_Advanced (IPacket &packet)
{
    enum
    {
        POSITION_ITEM,
        POSITION_ELIXIR,
        POSITION_LUCKY
    };

    uint8_t count = packet.Read<uint8_t>();

    if (count != 2)
        return MSG_ERROR_ARG;

    ///	[ITEM POS | ELIXIR POS | LUCKY POS]
    uint8_t pos[2] = { 0 , 0 };

    for (int i = 0; i < count; ++i)
        pos[i] = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (player->IsDead() || !player->IsTaskInterruptable())
        return MSG_SUCCESS;

    m_server->GetWorld()->TerminateTask(player->GetUniqueID());

    boost::shared_ptr<Storage> inv = player->get_storage(STORAGE_INVENTORY);

    Storage::iterator item_iter = inv->find(pos[POSITION_ITEM]);

    Storage::iterator elx_iter = inv->find(pos[POSITION_ELIXIR]);

    if (item_iter == inv->end() || elx_iter == inv->end())
    {
#if defined DEBUG
        syslog(LOG_INFO,"Item and/or Elixir dont exist.");
#endif
        return MSG_ERROR_ARG;
    }

    /// CHECK THAT THE ELIXIR CORRESPOND TO THE ITEM TYPE
    boost::shared_ptr<ItemProperty> property = elx_iter->second.property();

    bool valid = false;

    switch(property->Effects[0].first)
    {
    case ELIXIR_WEAPON:
        if (item_iter->second.IsWeapon())
            valid = true;
        break;
    case ELIXIR_SHIELD:
        if (item_iter->second.IsShield())
            valid = true;
        break;
    case ELIXIR_PROTECTOR:
        if (item_iter->second.IsArmor())
            valid = true;
        break;
    case ELIXIR_ACCESSORY:
        if (item_iter->second.IsAccessory())
            valid = true;
        break;
    default:
#if defined DEBUG
        syslog(LOG_INFO,"Unknown elixir type - %i",property->Effects[0].first);
#endif
        break;
    }

    /// CHECK IF THE ELIXIR IS FOR SOX ITEMS OR NOT
    if ((item_iter->second.sox_type() && elx_iter->second.sox_type() != 2) ||
        (!item_iter->second.sox_type() && elx_iter->second.sox_type() != 1))
    {
#if defined DEBUG
        syslog(LOG_INFO,"Invalid elixir type for SoX item.");
#endif
    }

    return MSG_SUCCESS;
}

int StateGame::OnAlchemyEnchant_Cancel (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (player->IsDead())
        return MSG_SUCCESS;

    if (!player->IsTaskInterruptable() && player->GetCurrentTask() != PLAYER_TASK_ALCHEMY)
        return MSG_SUCCESS;

    m_server->GetWorld()->TerminateTask(player->GetUniqueID());

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::ItemUpdate(pkt);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnAlchemyMagicOption (IPacket &packet)
{
#if defined DEBUG
    PrintPacketToSyslog(packet);
#endif
    /*
	boost::shared_ptr<Storage> storage = m_player->get_storage(STORAGE_INVENTORY);

	if (item_pos > storage->GetCapacity() || stone_pos > storage->GetCapacity())
		throw Exception::InvalidPacketArg();

	Item item = storage->GetItem(item_pos);

	if (!item.get_id())
		return;

	Item stone = storage->GetItem(stone_pos);

	if (!stone.get_id())
		return;

	if (stone.get_type() < ITEM_ALCHEMY_MAGIC_STONE && stone.get_type() > ITEM_ALCHEMY_ATTR_STONE)
		return;

	DB::ITEM::Effect effect_query;
	std::vector<ItemEffect> list = effect_query(m_conn,stone.get_id());

	if (list.size() < 7)
		return;

	DB::ITEM::Degree dg_query;
	uint8_t dg = dg_query(m_conn,item.get_id());

	if (dg != list[0].first)
		return;

	bool success = false;

	if ( rand() % 100 > 40 + item.get_enchant_lv()*5)
	{
		success = true;

		//TODO: ASSIMILATION AND INSERT BLUE AMOUNT
	}

	send(packet::ItemUpdate(success,item));

	//send(packet::Item(packet::Item::MOVE_REMOVE,stone_pos,4));
    */

    return MSG_SUCCESS;
}
