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

#ifndef __SLES_GAME_SERVER_ITEM_PACKET_HPP__
#define __SLES_GAME_SERVER_ITEM_PACKET_HPP__

#include <item.hpp>

class OPacket;

namespace srv_pkt
{

enum ITEM_UPDATE_RESULT
{
    ITEM_UPDATE_SUCCESS = 1,
    ITEM_UPDATE_FAIL
};

enum ITEM_ERROR
{
    ITEM_ERROR_INVENTORY_FULL = 0x07,
    ITEM_ERROR_DIFFERENT_SEX = 0x16,
    ITEM_ERROR_DIFFERENT_RACE = 0x2F,
    ITEM_ERROR_DIFFERENT_TYPE = 0x32,
    ITEM_ERROR_BROKEN_ITEM = 0x37,
    ITEM_ERROR_ITEM_TRADE = 0x3A,
    ITEM_ERROR_EQUIP_JOB_CAPE_WHILE_JOBBING = 0x46,
    ITEM_ERROR_SUMMON_LIMIT = 0x5C,
    ITEM_ERROR_LOWER_LV = 0x6C,
    ITEM_ERROR_DROP_ITEM = 0x6D,
    ITEM_ERROR_SUMMON_ATTACK_STATE = 0x78,
    ITEM_ERROR_USE_WHILE_DEAD = 0x89,
    ITEM_ERROR_TRANSGEND_ITEM_EQUIPPED = 0x92,
    ITEM_ERROR_BUFF_STACKING = 0x94,
    ITEM_ERROR_NEED_JOB = 0x9E,
    ITEM_ERROR_EQUIP_JOB_CAPE_NEED_ALIAS = 0x9F,
    ITEM_ERROR_JOB_CAPE_ON_PARTY = 0xA0,
    ITEM_ERROR_JOB_CAPE_ONLY_TOWN = 0xA2,
    ITEM_ERROR_JOB_CAPE_ON_TRANSPORT = 0xCA,
    ITEM_ERROR_INSUFFICIENT_GP = 0xC5,
    ITEM_ERROR_LIMIT_REACHED = 0xFE
};

/*
02 A0 - Cannot equip/unequip Job items when in a party
02 A1 - Murder Status users cannot equip job items
02 A2 - Can equip/unequip job item only in towns
02 A3 - Cannot unequip job item
02 A4 - Cannot summon because the period of summon item has expired, or pet of the summon item is dead
02 A5 - Cannot summon a pet because its level is higher than yours
02 A6 - Cannot item-ize a pet when it dies
02 A7 - Cannot item-ize a pet during battle
02 A8 - Cannot use summon item anymore
02 A9 - Same type of summon items cannot be used
02 B0 - Have not completed trade related quest
02 B1 - The item cannot be stored in the pets inventory
02 B2 - Summon item cannot be stored at storage because pet is summoned
02 B3 - HGP recovery potion cannot be used because pet is not hungry
02 B6 - Summon item cannot be traded because pet is summoned
02 B7 - Summon item cannot be sold because it is summoned
02 B8 - The selected item is unusable
02 B9 - Under Free battle situation pets cannot be summoned
02 C1 - Currently guild member recall will not work, or guild members are unable to return to town
02 C2 - Currently guild member recall will not work, or guild members are unable to return to town
02 C3 - Job items cannot be equipped during guild wars
02 C4 - You are not qualified to purchase the item
02 C5 - Item cannot be purchased due to lack of guild points
02 C6 - Exceeded one day use limit
02 C7 - Guild member recall scrolls can only be used in towns
02 C8 - Guild Mercenary cannot be summoned when equipping a free battle cape
02 C9 - A different mercenary cannot be summoned at the moment because a guild mercenary has already been summoned
02 D1 - You cannot purchase bargain priced special goods because 10 minutes have not passed since the purchase of bargain priced special goods
02 D2 - only guild mercenaries the same race as guild master can be summoned
02 D3 - This item cannot be repurchased
02 D4 - Selected item cannot be purchased due to insufficient amount of honour points
02 D6 - You are not allowed to use the items
02 D7 - You are not allowed to use the items
02 D8 - You are not allowed to use the items
02 D9 - You are not allowed to use the items
02 E0 - You don't have a position to use the items
02 E1 - The items are available only within the fortress
02 E2 - The items are only allowed of use during the fortress war
02 E3 - Defending guild is not allowed to use this item
02 E4 - Siege guild is not allowed to use this item
02 E5 - Siege guild is not allowed to use this item
02 E6 - You are not authorized
02 E7 - You are not authorized
02 E8 - You are not allowed to install more structures
02 E9 - It is not allowed to return to the fortress wearing the uniform indicating your job
02 F1 - The structure has already been installed
02 F2 - The action is not allowed during the fortress war
02 F3 - You are not allowed to use the items
02 F4 - You are not allowed to use the items
02 F5 - Unable to use that item at this area
02 F8 - You can wear devil's spirit, after awaken
02 F9 - If Devil's spirit's awaken period is passed or awaken you can not wear it
0A 1A - Cannot combine the selected item because it is already combined
02 1E - User is busy
02 1F - The trade cannot be made normally
02 2B - The exchange has been canceled
02 2C - The trade has canceled
02 2F - Cannot equip because of different tribe
02 3B - Cannot trade the selected item at the current shop
02 3E - The selected item is unusable
02 3F - Cannot sell item when equipped
02 5A - Cannot unequip job equipment while summoning a transport
02 5B - Still have time to reuse the item
02 5D - Cannot choose another teleport method because you are now being teleported
02 5E - Cannot teleport in the case of having already summoned a trasnport (Can use a ferry)
02 5F - The requested order cannot be carried out
02 6A - The selected item can be used only by a thief
02 6B - Cannot be performed during logout countdown
02 6C - The selected item is unusable because your character's level is too low
02 6E - Cannot obtain the dropped special items after using returning scroll to theif town
02 6F - Cannot obtain the items that you bought in the thief status
02 9D - During this restricted time, transport cannot be used
02 9E - Must join the correspond job league to equip job items
02 9F - Can equip the job item after creating alias
02 03 - Cannot find target
02 10 - Cannot equip item. The character level or skill mastery level is too low
02 12 - Cannot obtain the selected item because it is other players
02 14 - Only a number lower than the number left over can be entered
02 16 - Cannot equip because of different sex
02 19 - The selected item cannot be combined
02 20 - The trade cannot be made normally
02 21 - Cannot add the selected item because the exchange window is full
02 22 - Cannot trade because the inventory is full
02 23 - Trade cannot be carried out because there is no item to be traded
02 26 - Trade request automatically canceled due to no response from the other player
02 27 - Cannot find the other player
02 28 - The request for a private trade has been cancelled
02 29 - Invalid numbers
02 30 - Cannot equip due to insufficient power
02 31 - Cannot equip because of low intelligence
02 32 - Armor and taoist robe cannot be worn at the same time
02 34 - You are now in the state of being unable to move or trade items
02 35 - Cannot use item during trade or talking to NPC
02 36 - Cannot add the selected item to the exchange window because the other players inventory is full
02 37 - Cannot equip a broken item
02 38 - The selected item is indisposable
02 39 - The selected item cannot be obtained
02 43 - The selected item cannot be captured
02 45 - Cannot trade the selected item at the current shop
02 46 - Have to unequip the previous job item before wearing new job item
02 47 - Cannot unequp job equipment for 10 seconds after wearing it
02 48 - Cannot unarm for 20 seconds after the end of combat
02 49 - Only trade or thief can summon a transport
02 50 - Cannot trade special products at the current shop
02 60 - Can use special products only when the trader job is activated
02 61 - Cannot obtain special products without a summoned transport
02 62 - Cannot obtain special products because nobody in the party possesses them
02 63 - You cannot obtain another players trade item
02 65 - Cannot trade special products because you are too far from your transport
02 66 - You are in penalty status so you cannot activate the job
02 67 - Can activate only thief job because you are in the wanted status
02 68 - Cannot trade the stolen goods at the special products shop. They can only be trade at the swag shop
02 70 - Cannot equip any job item for 10 minutes after unequipping it
02 71 - Cannot use without a transport
02 72 - Cannot have more than the number limit
02 73 - Cannot find target
02 74 - Cannot collect quest items anymore since completetion requirements have been met
02 75 - Cannot use return scrolls with murder status
02 76 - Cannot use a transport with murder status
02 77 - Cannot unequip voucher during combat
02 79 - Cannot summon a vechile to move or transport in the dungeon
02 80 - This purchase cannot complete due to insufficent silkj
02 81 - Silk is insufficient for this gift
02 82 - It is unusable on the equipped item
03 83 - Item transgendering is not possible due to high degree of related item
02 84 - The selected item can not transgendered
02 85 - Cannot find the place where you selected as recall point
02 86 - Cannot find the place where you died
02 87 - Can use it only in the state of death
02 88 - No item needs repairing
02 92 - To transgend your character, you must first unarm and empty your equipment and extended equipment slots(avatar slots)
02 93 - Can only buy up to trade level 3 of special products on current server
02 94 - The selected item cannot be used together or the skill is already in use
02 95 - Cannot recall transport in the Thieves Den
*/

/**
 *
 *  IN = INVENTORY
 *  ST = STORAGE
 *  EX = EXCHANGE WINDOW
 *  SH = SHOP
 *  IM = ITEM MALL
 *  TR = TRANSPORT
 *  GD = GUILD
 *  AV = AVATAR
 *
 **/

enum ITEM_MOVE_ACTION
{
    ITEM_MOVE_INV_TO_INV,
    ITEM_MOVE_ST_TO_ST,
    ITEM_MOVE_INV_TO_ST,
    ITEM_MOVE_ST_TO_INV,
    ITEM_MOVE_INV_TO_EX,
    ITEM_MOVE_EX_TO_INV,
    ITEM_MOVE_PICKUP,
    ITEM_MOVE_DROP,
    ITEM_MOVE_BUY,
    ITEM_MOVE_SELL,
    ITEM_MOVE_GOLD_DROP,
    ITEM_MOVE_GOLD_WITHDRAW,
    ITEM_MOVE_GOLD_STORE,
    ITEM_MOVE_GOLD_EXCHANGE,
    ITEM_MOVE_CREATE,
    ITEM_MOVE_REMOVE,
    ITEM_MOVE_TR_TO_TR,
    ITEM_MOVE_NPC_TO_TR = 0x13,
    ITEM_MOVE_TR_TO_NPC,
    ITEM_MOVE_MALL_TO_INV = 0x18,
    ITEM_MOVE_GUILD = 0x1D,
    ITEM_MOVE_GUILD_DEPOSIT = 0x1E,
    ITEM_MOVE_GUILD_WITHDRAW = 0x1F,
    ITEM_MOVE_GUILD_GOLD_DEPOSIT = 0x20,
    ITEM_MOVE_GUILD_GOLD_WITHDRAW = 0x21,
    ITEM_MOVE_AVATAR_UNEQUIP = 0x23,
    ITEM_MOVE_AVATAR_EQUIP = 0x24
};

enum ALCHEMY_ACTION_TYPE
{
    ALCHEMY_MATERIAL_RONDO = 1,
    ALCHEMY_STONE_CREATE
};

void ItemMove (const boost::shared_ptr<OPacket> &pkt, const ITEM_ERROR error);

/**
 *
 *  @brief: Construct move item inside transport result packet.
 *  @ID: Transport ID.
 *  @src: Item source position.
 *  @dst: Item destination position.
 *  @quantity: Amount of items to move.
 *
 **/

void ItemMove_Transport (const boost::shared_ptr<OPacket> &pkt, uint32_t ID, uint8_t src, uint8_t dst,
    uint16_t quantity);

/**
 *
 *  @brief: Construct buy special product result packet.
 *
 **/

void ItemMove_Transport (const boost::shared_ptr<OPacket> &pkt, uint32_t ID, uint8_t page, uint8_t pos, uint8_t dst,
    uint16_t quantity);

/**
 *
 *  @brief: Construct sell special product result packet.
 *
 **/

void ItemMove_Transport (const boost::shared_ptr<OPacket> &pkt, uint32_t transportID, uint8_t pos, uint16_t quantity,
    uint32_t npcID);

/**
 *
 *  @brief Build packet for drop or exchange item to floor.
 *  @src Position of the item to drop or exchange.
 *
 **/

void Item (const boost::shared_ptr<OPacket> &pkt, uint8_t src, const ITEM_MOVE_ACTION type);

/**
 *
 *  @brief Build the pickup item packet.
 *  @item item picked up.
 *
 **/

void Item (const boost::shared_ptr<OPacket> &pkt, const ITEM_MOVE_ACTION action, const ::Item &item);

void Item (const boost::shared_ptr<OPacket> &pkt, const ITEM_MOVE_ACTION type, uint8_t src, uint8_t dest);

/**
 *
 *	@brief Build packet for alchemy material.
 *
 **/

void Item (const boost::shared_ptr<OPacket> &pkt, const uint8_t pos, const uint32_t ID, const uint16_t quantity);

/**
 *
 *	@brief Build packet for alchemy stone.
 *
 **/

void Item (const boost::shared_ptr<OPacket> &pkt, const uint8_t pos, const uint32_t ID, const uint16_t quantity,
    const uint8_t percent);

void Item (const boost::shared_ptr<OPacket> &pkt, const uint8_t page, const uint8_t pos, const uint8_t new_pos,
    const uint16_t quantity);

void Item (const boost::shared_ptr<OPacket> &pkt, uint8_t src, uint8_t dest, uint16_t quantity,
    const ITEM_MOVE_ACTION type);

/**
 *
 *  @brief Build packet for gold transfer.
 *	@gold_amount the amount of gold to transfer.
 *
 **/

void Item (const boost::shared_ptr<OPacket> &pkt, const ITEM_MOVE_ACTION type,uint64_t gold_amount);

/**
 *
 *  @brief Build packet for buy from item mall.
 *
 **/

void Item (const boost::shared_ptr<OPacket> &pkt, const uint8_t page, const uint8_t section, const uint8_t tab,
    const uint8_t pos, const uint8_t new_pos, const uint16_t quantity);

void ItemSell (const boost::shared_ptr<OPacket> &pkt, const uint8_t pos, const uint16_t quantity, const uint32_t ID);

void ItemEnchant (const boost::shared_ptr<OPacket> &pkt, const uint8_t pos);

void ItemEquip (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const uint8_t position,
    const uint32_t itemID, const uint8_t enchantLv);

void ItemUnequip (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const uint8_t position,
    const uint32_t itemID);

void ItemUpdate (const boost::shared_ptr<OPacket> &pkt);

void ItemUpdate (const boost::shared_ptr<OPacket> &pkt, bool success, const ::Item& item);

void ItemRepair (const boost::shared_ptr<OPacket> &pkt);

void ItemDurability (const boost::shared_ptr<OPacket> &pkt, const uint8_t pos, const uint32_t durability);

void ItemUse (const boost::shared_ptr<OPacket> &pkt, const uint8_t pos, const uint16_t quantity,
    const uint16_t use_type);

void ItemUse (const boost::shared_ptr<OPacket> &pkt, const ITEM_ERROR &error);

void ItemAnimation (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const uint32_t item_type);

void ItemQuantity (const boost::shared_ptr<OPacket> &pkt, const uint8_t pos, const uint16_t quantity);

void ItemAlchemy (const boost::shared_ptr<OPacket> &pkt, const ALCHEMY_ACTION_TYPE action, const uint16_t quantity);

void ItemRondo (const boost::shared_ptr<OPacket> &pkt);

void ItemAvatar (const boost::shared_ptr<OPacket> &pkt, const ::Item &item);

void ItemEquipWait (const boost::shared_ptr<OPacket> &pkt, const uint32_t ID);

void ItemEquipWaitCancel (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID);

void ItemDrop (const boost::shared_ptr<OPacket> &pkt, const uint32_t ID);

void ItemAmmo (const boost::shared_ptr<OPacket> &pkt, const uint16_t quantity);

}

#endif //__SLES_GAME_SERVER_ITEM_PACKET_HPP__
