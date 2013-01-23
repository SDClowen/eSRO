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

#include "packet_item.hpp"
#include "packet_base_item.hpp"
#include "opcodes_shard_server.hpp"

#include <packet.hpp>

namespace srv_pkt
{

void ItemMove (const boost::shared_ptr<OPacket> &pkt, const ITEM_MOVE_ACTION action)
{
    pkt->WriteOpcode(SERVER_ITEM_MOVEMENT,ANSWER_ACCEPT);
    pkt->Write<uint8_t>(action);
}

void ItemMove (const boost::shared_ptr<OPacket> &pkt, const ITEM_ERROR error)
{
    pkt->WriteOpcode(SERVER_ITEM_MOVEMENT,ANSWER_ERROR,error);
    pkt->Write<uint8_t>(0x18);
}

void ItemMove_Transport (const boost::shared_ptr<OPacket> &pkt, uint32_t ID, uint8_t src, uint8_t dst,
    uint16_t quantity)
{
    ItemMove(pkt,ITEM_MOVE_TR_TO_TR);

    pkt->Write<uint32_t>(ID);
    pkt->Write<uint8_t>(src);
    pkt->Write<uint8_t>(dst);
    pkt->Write<uint16_t>(quantity);
}

void ItemMove_Transport (const boost::shared_ptr<OPacket> &pkt, uint32_t ID, uint8_t page, uint8_t pos, uint8_t dst,
    uint16_t quantity)
{
    ItemMove(pkt,ITEM_MOVE_NPC_TO_TR);

    pkt->Write<uint32_t>(ID);
    pkt->Write<uint8_t>(page);
    pkt->Write<uint8_t>(pos);
    pkt->Write<uint8_t>(1);
    pkt->Write<uint8_t>(dst);
    pkt->Write<uint16_t>(quantity);
}

void ItemMove_Transport (const boost::shared_ptr<OPacket> &pkt, uint32_t transportID, uint8_t pos, uint16_t quantity,
    uint32_t npcID)
{
    ItemMove(pkt,ITEM_MOVE_TR_TO_NPC);

    pkt->Write<uint32_t>(transportID);
    pkt->Write<uint8_t>(pos);
    pkt->Write<uint16_t>(quantity);
    pkt->Write<uint32_t>(npcID);
    pkt->Write<uint8_t>(0xFF);
}

void Item (const boost::shared_ptr<OPacket> &pkt, uint8_t src, const ITEM_MOVE_ACTION type)
{
    assert(type == ITEM_MOVE_DROP || type == ITEM_MOVE_EX_TO_INV);

    ItemMove(pkt,type);
    pkt->Write<uint8_t>(src);
}

void Item (const boost::shared_ptr<OPacket> &pkt, const ITEM_MOVE_ACTION action, const ::Item &item)
{
    assert(action == ITEM_MOVE_PICKUP);

    ItemMove(pkt,action);

    if (item.get_type() == ITEM_GOLD)
    {
        pkt->Write<uint8_t>(item.get_position());
        pkt->Write<uint32_t>(item.get_quantity());
    }
    else
        WriteItem(pkt,item);
}

void Item (const boost::shared_ptr<OPacket> &pkt, const ITEM_MOVE_ACTION type, const uint8_t src, const uint8_t dest)
{
    ItemMove(pkt,type);

    pkt->Write<uint8_t>(src);
    pkt->Write<uint8_t>(dest);
}

void Item (const boost::shared_ptr<OPacket> &pkt, const uint8_t pos, const uint32_t ID, const uint16_t quantity)
{
    ItemMove(pkt,ITEM_MOVE_CREATE);

    pkt->Write<uint8_t>(pos);
    pkt->Write<uint8_t>(0x00);
    pkt->Write<uint32_t>(ID);
    pkt->Write<uint16_t>(quantity);
}

void Item (const boost::shared_ptr<OPacket> &pkt, const uint8_t pos, const uint32_t ID, const uint16_t quantity,
    const uint8_t percent)
{
    ItemMove(pkt,ITEM_MOVE_CREATE);

    pkt->Write<uint8_t>(pos);
    pkt->Write<uint8_t>(0x00);
    pkt->Write<uint32_t>(ID);
    pkt->Write<uint16_t>(quantity);
    pkt->Write<uint8_t>(percent);
}

void Item (const boost::shared_ptr<OPacket> &pkt, const uint8_t page, const uint8_t pos, const uint8_t new_pos,
    const uint16_t quantity)
{
    ItemMove(pkt,ITEM_MOVE_BUY);

    pkt->Write<uint8_t>(page);
    pkt->Write<uint8_t>(pos);
    pkt->Write<uint8_t>(1);
    pkt->Write<uint8_t>(new_pos);
    pkt->Write<uint16_t>(quantity);
}

void Item (const boost::shared_ptr<OPacket> &pkt, uint8_t src, uint8_t dest, uint16_t quantity,
    const ITEM_MOVE_ACTION type)
{
    ItemMove(pkt,type);

    pkt->Write<uint8_t>(src);
    pkt->Write<uint8_t>(dest);

    switch(type)
    {
    case ITEM_MOVE_INV_TO_INV:
    case ITEM_MOVE_AVATAR_EQUIP:
    case ITEM_MOVE_AVATAR_UNEQUIP:
        pkt->Write<uint16_t>(quantity);
        pkt->Write<uint8_t>(0);
        break;
    case ITEM_MOVE_GUILD:
    case ITEM_MOVE_ST_TO_ST:
         pkt->Write<uint16_t>(quantity);
    default:
        break;
    }

}

void Item (const boost::shared_ptr<OPacket> &pkt, const ITEM_MOVE_ACTION type, uint64_t gold_amount)
{
    ItemMove(pkt,type);
    pkt->Write<uint64_t>(gold_amount);
}

void Item (const boost::shared_ptr<OPacket> &pkt, const uint8_t page, const uint8_t section, const uint8_t tab, 
    const uint8_t pos, const uint8_t new_pos, const uint16_t quantity)
{
    ItemMove(pkt,ITEM_MOVE_MALL_TO_INV);

    pkt->Write<uint8_t>(0x36);       ///ISRO HAS 0x54
    pkt->Write<uint8_t>(page);
    pkt->Write<uint8_t>(section);
    pkt->Write<uint8_t>(tab);
    pkt->Write<uint8_t>(pos);
    pkt->Write<uint8_t>(1);
    pkt->Write<uint8_t>(new_pos);
    pkt->Write<uint16_t>(quantity);
}

void ItemSell (const boost::shared_ptr<OPacket> &pkt, const uint8_t pos, const uint16_t quantity, const uint32_t ID)
{
    ItemMove(pkt,ITEM_MOVE_SELL);

    pkt->Write<uint8_t>(pos);
    pkt->Write<uint16_t>(quantity);
    pkt->Write<uint32_t>(ID);
    pkt->Write<uint8_t>(1);
}

void ItemEnchant (const boost::shared_ptr<OPacket> &pkt, const uint8_t pos)
{
    ItemMove(pkt,ITEM_MOVE_REMOVE);

    pkt->Write<uint8_t>(pos);
    pkt->Write<uint8_t>(4);
}

void ItemEquip (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const uint8_t position,
    const uint32_t itemID, const uint8_t enchant_lv)
{
    pkt->WriteOpcode(SERVER_ITEM_EQUIP);
    pkt->Write<uint32_t>(playerID);
    pkt->Write<uint8_t>(position);
    pkt->Write<uint32_t>(itemID);
    pkt->Write<uint8_t>(enchant_lv);
}

void ItemUnequip (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const uint8_t position,
    const uint32_t itemID)
{
    pkt->WriteOpcode(SERVER_ITEM_UNEQUIP);
    pkt->Write<uint32_t>(playerID);
    pkt->Write<uint8_t>(position);
    pkt->Write<uint32_t>(itemID);
}

void ItemUpdate (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_ITEM_STRENGTHEN,ANSWER_ACCEPT);
    pkt->Write<uint8_t>(ITEM_UPDATE_SUCCESS);
}

void ItemUpdate (const boost::shared_ptr<OPacket> &pkt, bool success, const ::Item& item)
{
    pkt->WriteOpcode(SERVER_ITEM_STRENGTHEN,ANSWER_ACCEPT);
    pkt->Write<uint8_t>(2);
    pkt->Write<uint8_t>(success);
    pkt->Write<uint8_t>(item.get_position());

    if (!success)
        pkt->Write<uint8_t>(0);

    WriteItem(pkt,item,false);
}

void ItemRepair (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_ITEM_REPAIR,ANSWER_ACCEPT);
}

void ItemDurability (const boost::shared_ptr<OPacket> &pkt, const uint8_t pos, const uint32_t durability)
{
    pkt->WriteOpcode(SERVER_ITEM_DURABILITY);
    pkt->Write<uint8_t>(pos);
    pkt->Write<uint32_t>(durability);
}

void ItemUse (const boost::shared_ptr<OPacket> &pkt, const uint8_t pos, const uint16_t quantity,
    const uint16_t use_type)
{
    pkt->WriteOpcode(SERVER_ITEM_USE,ANSWER_ACCEPT);
    pkt->Write<uint8_t>(pos);
    pkt->Write<uint16_t>(quantity);
    pkt->Write<uint16_t>(use_type);
}

void ItemUse (const boost::shared_ptr<OPacket> &pkt, const ITEM_ERROR &error)
{
    pkt->WriteOpcode(SERVER_ITEM_USE,ANSWER_ERROR,error);
    pkt->Write<uint8_t>(0x18);
}

void ItemAnimation (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const uint32_t item_type)
{
    pkt->WriteOpcode(SERVER_ITEM_ANIMATION);
    pkt->Write<uint32_t>(playerID);
    pkt->Write<uint32_t>(item_type);
}

void ItemQuantity (const boost::shared_ptr<OPacket> &pkt, const uint8_t pos, const uint16_t quantity)
{
    pkt->WriteOpcode(SERVER_ITEM_QUANTITY_NOTIFY);
    pkt->Write<uint8_t>(pos);
    pkt->Write<uint8_t>(0x08);
    pkt->Write<uint16_t>(quantity);
}

void ItemAlchemy (const boost::shared_ptr<OPacket> &pkt, const ALCHEMY_ACTION_TYPE action, const uint16_t quantity)
{
    pkt->WriteOpcode(SERVER_ITEM_STONE_CREATE,ANSWER_ACCEPT);
    pkt->Write<uint8_t>(action);
    pkt->Write<uint16_t>(quantity);
    pkt->Write<uint16_t>(0);
}

void ItemRondo (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_ITEM_RONDO,ANSWER_ACCEPT);
}

void ItemAvatar (const boost::shared_ptr<OPacket> &pkt, const ::Item &item)
{
    pkt->WriteOpcode(SERVER_ITEM_AVATAR_MAGIC_OPTION,ANSWER_ACCEPT);
    pkt->Write<uint8_t>(0x01); //SUCCESS

    WriteItem(pkt,item);
}

void ItemEquipWait (const boost::shared_ptr<OPacket> &pkt, const uint32_t ID)
{
    pkt->WriteOpcode(SERVER_ITEM_EQUIP_WAIT);
    pkt->Write<uint32_t>(ID);
    pkt->Write<uint8_t>(2);
    pkt->Write<uint8_t>(1);
    pkt->Write<uint8_t>(10);
}

void ItemEquipWaitCancel (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID)
{
    pkt->WriteOpcode(SERVER_ITEM_EQUIP_WAIT_CANCEL);
    pkt->Write<uint32_t>(playerID);
}

void ItemDrop (const boost::shared_ptr<OPacket> &pkt, const uint32_t ID)
{
    pkt->WriteOpcode(SERVER_ITEM_DROP);
    pkt->Write<uint32_t>(ID);
}

void ItemAmmo (const boost::shared_ptr<OPacket> &pkt, const uint16_t quantity)
{
    pkt->WriteOpcode(SERVER_ITEM_AMMO);
    pkt->Write<uint16_t>(quantity);
}

}
