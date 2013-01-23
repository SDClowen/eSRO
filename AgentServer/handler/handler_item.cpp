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
#include "skill_manager.hpp"
#include "trade_manager.hpp"

/// SOL INCLUDES
#include <party.hpp>
#include <skill.hpp>
#include <guild.hpp>
#include <transport.hpp>
#include <dropable_item.hpp>
#include <teleport_building.hpp>
#include <exchange_window.hpp>
#include <item_factory.hpp>
#include <character_factory.hpp>
#include <npc_util.hpp>
#include <item_property.hpp>

/// SRDL INCLUDES
#include <item_query.hpp>
#include <shop_query.hpp>
#include <guild_query.hpp>
#include <player_query.hpp>
#include <job_query.hpp>

/// EPL INCLUDES
#include <packet_msg.hpp>
#include <packet_npc.hpp>
#include <packet_player.hpp>
#include <packet_skill.hpp>
#include <packet_item.hpp>
#include <packet_guild.hpp>
#include <packet_exchange.hpp>
#include <packet_transport.hpp>
#include <packet_shard.hpp>
#include <packet_job.hpp>

#define JOB_CAPE_WAIT_TIME 10
#define THIEF_DEN_TELEPORT 10

#if defined DEBUG
#include <packet_util_functions.hpp>
#endif

enum ITEM_USE
{
    USE_ATTACK_PET = 0x08CC,
    USE_SPECIAL_PILL = 0x316C,
    USE_PURIFICATION_PILL = 0x096C,
    USE_HP_POTS_V1 = 0x08EC,
    USE_HP_POTS_V2 = 0x08ED,
    USE_RETURN_SCROLL_V1 = 0x09EC,
    USE_RETURN_SCROLL_V2 = 0x09ED,
    USE_MP_POTS_V1 = 0x10EC,
    USE_MP_POTS_V2 = 0x10ED,
    USE_TRANSPORT_SCROLL_V1 = 0x11EC,
    USE_TRANSPORT_SCROLL_V2 = 0x11ED,
    USE_VIGOR_POTS_V1 = 0x18EC,
    USE_VIGOR_POTS_V2 = 0x18ED,
    USE_REVERSE_RETURN_V1 = 0x19EC,
    USE_REVERSE_RETURN_V2 = 0x19ED,
    USE_STALL_AVATAR = 0x21ED,
    USE_RESURRECT_SCROLL = 0x36EC,
    USE_BUFF_SCROLL_V1 = 0x0EEC,
    USE_BUFF_SCROLL_V2 = 0x0EED,
    USE_DEVIL_SPIRIT = 0x0F2D,
    USE_GLOBAL_SCROLL = 0x29ED,
    USE_REPAIR_HAMMER_V1 = 0x3EEC,
    USE_REPAIR_HAMMER_V2 = 0x3EED,
    USE_GENDER_SWITCH_TOOL = 0x46ED,
    USE_SKIN_CHANGER = 0x4EED,
    USE_ZERK_POT = 0x76EC,
    USE_STORAGE_SPACE_EXPANSION = 0x8EED,
    USE_DEVEL_SPIRIT_EXTENSION = 0x86ED    ///next byte is the pos of the awaked devil spirit!!
};

enum PRICE_TYPE
{
    PRICE_GOLD = 1,
    PRICE_SILK = 2,
    PRICE_GUILD_POINTS = 8,
    PRICE_HONOR_POINTS = 32
};

std::string horse_level (const std::string &str, const int level);

bool IsPositionValid (Item &item, const uint8_t pos);

int IsItemEquipable (const boost::shared_ptr<Player> &player, const Item &item,
    const boost::shared_ptr<Storage> &store);

int StateGame::OnItemMove (IPacket &packet)
{
    uint8_t action = packet.Read<uint8_t>();

    uint8_t ret;

    switch(action)
    {
    case srv_pkt::ITEM_MOVE_INV_TO_INV:
        ret =OnItemMove_FromInvToInv(packet);
        break;
    case srv_pkt::ITEM_MOVE_ST_TO_ST:
        ret = OnItemMove_FromStToSt(packet);
        break;
    case srv_pkt::ITEM_MOVE_INV_TO_ST:
        ret = OnItemMove_FromInvToSt(packet);
        break;
    case srv_pkt::ITEM_MOVE_ST_TO_INV:
        ret = OnItemMove_FromStToInv(packet);
        break;
    case srv_pkt::ITEM_MOVE_GOLD_STORE:
        ret = OnItemMove_St_Deposit_Gold(packet);
        break;
    case srv_pkt::ITEM_MOVE_GOLD_WITHDRAW:
        ret = OnItemMove_St_Withdraw_Gold(packet);
        break;
    case srv_pkt::ITEM_MOVE_INV_TO_EX:
        ret = OnItemMove_FromInvToEx(packet);
        break;
    case srv_pkt::ITEM_MOVE_EX_TO_INV:
        ret = OnItemMove_FromExToInv(packet);
        break;
    case srv_pkt::ITEM_MOVE_GOLD_EXCHANGE:
        ret = OnItemMove_Exchange_Gold(packet);
        break;
    case srv_pkt::ITEM_MOVE_DROP:
        ret = OnItemMove_FromInvToFloor_I(packet);
        break;
    case srv_pkt::ITEM_MOVE_GOLD_DROP:
        ret = OnItemMove_FromInvToFloor_G(packet);
        break;
    case srv_pkt::ITEM_MOVE_BUY:
        ret = OnItemMove_FromNpcToInv(packet);
        break;
    case srv_pkt::ITEM_MOVE_SELL:
        ret = OnItemMove_FromInvToNpc(packet);
        break;
    case srv_pkt::ITEM_MOVE_TR_TO_TR:
        ret = OnItemMove_FromTransportToTransport(packet);
        break;
    case srv_pkt::ITEM_MOVE_NPC_TO_TR:
        ret = OnItemMove_FromNpcToTransport(packet);
        break;
    case srv_pkt::ITEM_MOVE_TR_TO_NPC:
        ret = OnItemMove_FromTransportToNpc(packet);
        break;
    case srv_pkt::ITEM_MOVE_MALL_TO_INV:
        ret = OnItemMove_FromMallToInv(packet);
        break;
    case srv_pkt::ITEM_MOVE_GUILD:
        ret = OnItemMove_FromGuildStToGuildSt(packet);
        break;
    case srv_pkt::ITEM_MOVE_GUILD_DEPOSIT:
        ret = OnItemMove_FromInvToGuildSt(packet);
        break;
    case srv_pkt::ITEM_MOVE_GUILD_WITHDRAW:
        ret = OnItemMove_FromGuildStToInv(packet);
        break;
    case srv_pkt::ITEM_MOVE_GUILD_GOLD_DEPOSIT:
        ret = OnItemMove_GuildSt_Deposit_Gold(packet);
        break;
    case srv_pkt::ITEM_MOVE_GUILD_GOLD_WITHDRAW:
        ret = OnItemMove_GuildSt_Withdraw_Gold(packet);
        break;
    case srv_pkt::ITEM_MOVE_AVATAR_EQUIP:
        ret = OnItemMove_FromInvToAva(packet);
        break;
    case srv_pkt::ITEM_MOVE_AVATAR_UNEQUIP:
        ret = OnItemMove_FromAvaToInv(packet);
        break;
    default:
#if defined DEBUG
        syslog(LOG_INFO,"OnItemMove - Unknown action %i",action);
        PrintPacketToSyslog(packet);
#endif
        ret = MSG_SUCCESS;
        break;
    }

    return ret;
}

int StateGame::OnItemMove_FromInvToInv (IPacket &packet)
{
    uint8_t src = packet.Read<uint8_t>();

    uint8_t dest = packet.Read<uint8_t>();

    uint16_t quantity = packet.Read<uint16_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (player->IsDead() || !player->IsTaskInterruptable())
        return MSG_SUCCESS;

    boost::shared_ptr<Storage> store = player->get_storage(STORAGE_INVENTORY);

    if (src > store->GetCapacity() || dest > store->GetCapacity() || dest == src)
        return MSG_ERROR_ARG;

    int seconds = 0;
    Item isrc = store->GetItem(src);
    Item idest = store->GetItem(dest);

    if (!isrc.get_id())
    {
#if defined DEBUG
        syslog(LOG_INFO,"OnItemMove_InvInv -  Item dont exists.");
#endif
        return MSG_ERROR_ARG;
    }

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if ( dest < Item::MAX_EQUIPMENT_SLOT)
    {
        /// INVENTORY SLOTS TO CHARACTER SLOTS

        if (!isrc.get_id())
        {
#if defined DEBUG
            syslog(LOG_INFO,"OnItemMove_InvInv - Invalid item.");
#endif
            return MSG_ERROR_ARG;
        }

        /// CHECK THAT THE DEFAULT POSITION OF THE ITEM IS THE DESTINATION POSITION
        if (!IsPositionValid(isrc,dest))
        {
#if defined DEBUG
            syslog(LOG_INFO,"Default position dont equal the dest.");
#endif
            return MSG_SUCCESS;
        }

        int ret = IsItemEquipable(player,isrc,store);

        if (ret)
        {
            srv_pkt::ItemMove(pkt,static_cast<srv_pkt::ITEM_ERROR>(ret));
            m_connection->Send(pkt);
            return MSG_SUCCESS;
        }

        if (!CheckWeaponShieldSlots(isrc,player,store))
            return MSG_SUCCESS;

        if ( dest == Item::JOB_CAPE_SLOT )
        {
            if (isrc.get_type() != ITEM_PVP_CAPE)
            {
                Job job = player->get_job();

                /// CHECK THAT ITS HAS A JOB
                if (job.get_type() == Job::NONE)
                {
                    srv_pkt::ItemMove(pkt,srv_pkt::ITEM_ERROR_NEED_JOB);
                    m_connection->Send(pkt);
                    return MSG_SUCCESS;
                }

                /// CHECK THAT IT HAS AN ALIAS
                if (job.get_alias().empty())
                {
                    srv_pkt::ItemMove(pkt,srv_pkt::ITEM_ERROR_EQUIP_JOB_CAPE_NEED_ALIAS);
                    m_connection->Send(pkt);
                    return MSG_SUCCESS;
                }

                /// CHECK THAT THE JOB ITEM MATCH HIS JOB
                bool valid_job = true;
                switch(isrc.get_type())
                {
                case ITEM_JOB_TRADER:
                    if (job.get_type() != Job::TRADER)
                        valid_job = false;
                    break;
                case ITEM_JOB_HUNTER:
                    if (job.get_type() != Job::HUNTER)
                        valid_job = false;
                    break;
                case ITEM_JOB_THIEF:
                    if (job.get_type() != Job::THIEF)
                        valid_job = false;
                    break;
                default:
                    break;
                }

                if (!valid_job)
                {
                    srv_pkt::ItemMove(pkt,srv_pkt::ITEM_ERROR_NEED_JOB);
                    m_connection->Send(pkt);
                    return MSG_SUCCESS;
                }

                /// CHECK THAT UR IN A TOWN
                if (!m_server->GetWorld()->IsInsideTown(player->get_position()))
                {
                    srv_pkt::ItemMove(pkt,srv_pkt::ITEM_ERROR_JOB_CAPE_ONLY_TOWN);
                    m_connection->Send(pkt);
                    return MSG_SUCCESS;
                }

                /// CHECK THAT UR NOT WEARING A JOB ITEM ALREDY
                if (idest.get_id() && idest.get_type() != ITEM_PVP_CAPE)
                {
                    srv_pkt::ItemMove(pkt,srv_pkt::ITEM_ERROR_EQUIP_JOB_CAPE_WHILE_JOBBING);
                    m_connection->Send(pkt);
                    return MSG_SUCCESS;
                }

                /// CHECK THAT UR NOT IN A PARTY

                if (player->in_party())
                {
                    srv_pkt::ItemMove(pkt,srv_pkt::ITEM_ERROR_JOB_CAPE_ON_PARTY);
                    m_connection->Send(pkt);
                    return MSG_SUCCESS;
                }

                /// CHECK THAT THERES NO TRANSPORT SUMMONED

                if (player->TransportID())
                {
                    srv_pkt::ItemMove(pkt,srv_pkt::ITEM_ERROR_JOB_CAPE_ON_TRANSPORT);
                    m_connection->Send(pkt);
                    return MSG_SUCCESS;
                }

                /// CHECK THAT UR NOT IN MURDER STATUS
                
                boost::shared_ptr<TeleportBuilding> blg;
                blg = m_server->GetWorld()->FindBuilding_CloseToPosition(player->get_position());

                if (!blg)
                {
                    srv_pkt::ItemMove(pkt,srv_pkt::ITEM_ERROR_JOB_CAPE_ONLY_TOWN);
                    m_connection->Send(pkt);
                    return MSG_SUCCESS;
                }
            }

            seconds = JOB_CAPE_WAIT_TIME;

            srv_pkt::ItemEquipWait(pkt,player->GetUniqueID());
            m_server->SendChannel(pkt,player->get_position().get_zone());
        }
    }
    else if ( src < Item::MAX_EQUIPMENT_SLOT)
    {
        /// CHARACTER SLOTS TO INVENTORY SLOTS

        /// CHECK THAT THE SUBSTITUTE ITEM CAN BE EQUIPED IN THE SAME SLOT (IF THERES ANY)

        if (idest.get_id())
        {
            /// CHECK THAT THE DEFAULT POSITION OF THE ITEM IS THE DESTINATION POSITION
            if (!IsPositionValid(idest,src))
            {
    #if defined DEBUG
                syslog(LOG_INFO,"Default position dont equal the dest.");
    #endif
                return MSG_SUCCESS;
            }

            int ret = IsItemEquipable(player,idest,store);

            if (ret)
            {
                srv_pkt::ItemMove(pkt,static_cast<srv_pkt::ITEM_ERROR>(ret));
                m_connection->Send(pkt);
                return MSG_SUCCESS;
            }

            if (!CheckWeaponShieldSlots(idest,player,store))
                return MSG_SUCCESS;
        }

        if ( src == Item::JOB_CAPE_SLOT )
        {
            if (isrc.get_type() != ITEM_PVP_CAPE)
            {
                /// CHECK THAT UR IN A TOWN
                if (!m_server->GetWorld()->IsInsideTown(player->get_position()))
                {
                    srv_pkt::ItemMove(pkt,srv_pkt::ITEM_ERROR_JOB_CAPE_ONLY_TOWN);
                    m_connection->Send(pkt);
                    return MSG_SUCCESS;
                }

                /// CHECK THAT UR NOT IN A PARTY

                if (player->in_party())
                {
                    srv_pkt::ItemMove(pkt,srv_pkt::ITEM_ERROR_JOB_CAPE_ON_PARTY);
                    m_connection->Send(pkt);
                    return MSG_SUCCESS;
                }

                /// CHECK THAT THERES NO TRANSPORT SUMMONED

                if (player->m_transport)
                {
                    srv_pkt::ItemMove(pkt,srv_pkt::ITEM_ERROR_JOB_CAPE_ON_TRANSPORT);
                    m_connection->Send(pkt);
                    return MSG_SUCCESS;
                }

                boost::shared_ptr<TeleportBuilding> blg;
                blg = m_server->GetWorld()->FindBuilding_CloseToPosition(player->get_position());

                if (!blg)
                {
                    srv_pkt::ItemMove(pkt,srv_pkt::ITEM_ERROR_JOB_CAPE_ONLY_TOWN);
                    m_connection->Send(pkt);
                    return MSG_SUCCESS;
                }
            }

            seconds = JOB_CAPE_WAIT_TIME;

            srv_pkt::ItemEquipWait(pkt,player->GetUniqueID());
            m_server->SendChannel(pkt,player->get_position().get_zone());
        }
    }
    else
    {
        /// FROM INVENTORY SLOTS TO INVENTORY SLOTS
    }

    m_server->GetWorld()->TerminateTask(player->GetUniqueID());

    if (!m_server->GetWorld()->InitTaskMoveItem(player->GetUniqueID(),src,dest,quantity,seconds))
        return MSG_ERROR;

    return MSG_SUCCESS;
}

int StateGame::OnItemMove_FromStToSt (IPacket &packet)
{
    uint8_t src = packet.Read<uint8_t>();

    uint8_t dest = packet.Read<uint8_t>();

    uint16_t quantity = packet.Read<uint16_t>();

    uint32_t npcID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<NPC> npc = m_server->GetWorld()->FindNPC_CloseToPosition(npcID,player->get_position());

    if (!npc || !IsStorageManager(npc->get_id()))
        return MSG_ERROR_ARG;

    boost::shared_ptr<Storage> store = player->get_storage(STORAGE_MAIN);

    if (!store)
        return MSG_ERROR;

    if (src > store->GetCapacity() || dest > store->GetCapacity() || dest == src)
        return MSG_ERROR_ARG;

    Item isrc = store->GetItem(src);
    Item idest = store->GetItem(dest);

    if (!isrc.get_id())
    {
#if defined DEBUG
        syslog(LOG_INFO,"OnItemMove_StSt - Item dont exists.");
#endif
        return MSG_ERROR_ARG;
    }

    store->MoveItem(src,dest,quantity);

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::Item(pkt,src,dest,quantity,srv_pkt::ITEM_MOVE_ST_TO_ST);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnItemMove_FromInvToSt (IPacket &packet)
{
    uint8_t src = packet.Read<uint8_t>();

    uint8_t dest = packet.Read<uint8_t>();

    uint32_t npcID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<NPC> npc = m_server->GetWorld()->FindNPC_CloseToPosition(npcID,player->get_position());

    if (!npc || !IsStorageManager(npc->get_id()))
        return MSG_ERROR_ARG;

    boost::shared_ptr<Storage> inv = player->get_storage(STORAGE_INVENTORY);

    if (!inv)
        return MSG_ERROR;

    boost::shared_ptr<Storage> main = player->get_storage(STORAGE_MAIN);

    if (!main)
        return MSG_ERROR;

    if (src >= inv->GetCapacity() || dest >= main->GetCapacity())
        return MSG_ERROR_ARG;

    Item itm = inv->GetItem(src);

    if (!itm.get_id())
        return MSG_ERROR;

    uint32_t total = itm.get_storage_feed()*itm.get_quantity();

    if (!total)
        total = 1;

    if (!player->spend_gold(itm.get_storage_feed()))
    {
        syslog(LOG_INFO,"Insufficient gold.");
        ///Packet p1;
        ///p1.WriteOpcode(0xB495);
        ///p1.Write<uint8_t>(2);
        ///p1.Write<uint8_t>(7);

        ///send(p1);

        return MSG_SUCCESS;
    }

    inv->RemoveItem(src);

    itm.set_position(dest);
    itm.set_place(PLACE_STORAGE);

    main->InsertItem(itm);

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::Item(pkt,src,dest,0,srv_pkt::ITEM_MOVE_INV_TO_ST);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnItemMove_FromStToInv (IPacket &packet)
{
    uint8_t src = packet.Read<uint8_t>();

    uint8_t dest = packet.Read<uint8_t>();

    uint32_t npcID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<NPC> npc = m_server->GetWorld()->FindNPC_CloseToPosition(npcID,player->get_position());

    if (!npc || !IsStorageManager(npc->get_id()))
        return MSG_ERROR_ARG;

    boost::shared_ptr<Storage> main = player->get_storage(STORAGE_MAIN);

    if (!main)
        return MSG_ERROR;

    boost::shared_ptr<Storage> inv = player->get_storage(STORAGE_INVENTORY);

    if (!inv)
        return MSG_ERROR;

    if (src >= inv->GetCapacity() || dest >= main->GetCapacity())
        return MSG_ERROR_ARG;

    Item itm = main->GetItem(src);

    if (!itm.get_id())
        return MSG_SUCCESS;

    main->RemoveItem(src);

    itm.set_position(dest);
    itm.set_place(PLACE_INVENTORY);

    inv->InsertItem(itm);

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::Item(pkt,src,dest,0,srv_pkt::ITEM_MOVE_ST_TO_INV);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnItemMove_St_Deposit_Gold (IPacket &packet)
{
    uint64_t amount = packet.Read<uint64_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (!player->deposit_gold(amount))
        return MSG_ERROR_ARG;

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::Item(pkt,srv_pkt::ITEM_MOVE_GOLD_STORE,amount);
    m_connection->Send(pkt);

    DB::PLAYER::GoldStored query;
    query(m_server->DBConnection(),player->ID(),player->get_stored_gold());

    return MSG_SUCCESS;
}

int StateGame::OnItemMove_St_Withdraw_Gold (IPacket &packet)
{
    uint64_t amount = packet.Read<uint64_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (!player->withdraw_gold(amount))
        return MSG_ERROR_ARG;

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::Item(pkt,srv_pkt::ITEM_MOVE_GOLD_WITHDRAW,amount);
    m_connection->Send(pkt);

    DB::PLAYER::GoldStored query;
    query(m_server->DBConnection(),player->ID(),player->get_stored_gold());

    return MSG_SUCCESS;
}

int StateGame::OnItemMove_FromInvToEx (IPacket &packet)
{
    uint8_t inv_pos = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Player> explayer = get_invitation_player(player);

    if (!explayer)
        return MSG_SUCCESS;

    boost::shared_ptr<Storage> inv = player->get_storage(STORAGE_INVENTORY);

    if (inv_pos >= inv->GetCapacity())
        return MSG_ERROR_ARG;

    Item itm = inv->GetItem(inv_pos);

    boost::shared_ptr<ExchangeWnd> exchange = player->get_exchange_window();

    if (!exchange)
        return MSG_ERROR;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (!itm.is_tradable())
    {
        srv_pkt::ItemMove(pkt,srv_pkt::ITEM_ERROR_ITEM_TRADE);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    itm.set_alt_position(inv_pos);

    uint8_t new_pos = exchange->get_empty_position(0);

    if (new_pos == 255)
    {
#if defined DEBUG
       syslog(LOG_INFO,"Exchange window full.");
#endif
        return MSG_SUCCESS;
    }

    itm.set_position(new_pos);

    exchange->InsertItem(itm);

    srv_pkt::ExchangeItem(pkt,player->GetUniqueID(),exchange,true);
    m_connection->Send(pkt);

    pkt.reset(new OPacket);

    srv_pkt::ExchangeItem(pkt,player->GetUniqueID(),exchange,false);
    m_server->Send(pkt,explayer->ConnectionID());

    pkt.reset(new OPacket);

    srv_pkt::Item(pkt,srv_pkt::ITEM_MOVE_INV_TO_EX,inv_pos,new_pos);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnItemMove_FromExToInv (IPacket &packet)
{
    uint8_t pos = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Player> inv_player = get_invitation_player(player);

    if(!inv_player)
        return MSG_SUCCESS;

    boost::shared_ptr<ExchangeWnd> exchange = player->get_exchange_window();

    if (!exchange)
        return MSG_ERROR;

    exchange->RemoveItem(pos);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    srv_pkt::ExchangeItem(pkt,player->GetUniqueID(),exchange,true);
    m_connection->Send(pkt);

    pkt.reset(new OPacket);

    srv_pkt::ExchangeItem(pkt,inv_player->GetUniqueID(),exchange,false);
    m_server->Send(pkt,inv_player->ConnectionID());

    pkt.reset(new OPacket);

    srv_pkt::Item(pkt,pos,srv_pkt::ITEM_MOVE_EX_TO_INV);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnItemMove_Exchange_Gold (IPacket &packet)
{
    uint64_t amount = packet.Read<uint64_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Player> inv_player = get_invitation_player(player);

    if(!inv_player)
        return MSG_SUCCESS;

    if (amount > inv_player->get_gold())
        return MSG_ERROR_ARG;

    boost::shared_ptr<ExchangeWnd> exchange = player->get_exchange_window();

    if (!exchange)
        return MSG_ERROR;

    exchange->set_gold_amount(amount);

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::Item(pkt,srv_pkt::ITEM_MOVE_GOLD_EXCHANGE,amount);
    m_connection->Send(pkt);

    pkt.reset(new OPacket);

    srv_pkt::ExchangeGold(pkt,amount);
    m_server->Send(pkt,inv_player->ConnectionID());

    return MSG_SUCCESS;
}

int StateGame::OnItemMove_FromInvToFloor_I (IPacket &packet)
{
    uint8_t position = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Storage> inv = player->get_storage(STORAGE_INVENTORY);

    if (position > inv->GetCapacity())
        return MSG_ERROR_ARG;

    boost::shared_ptr<DropableItem> item(new DropableItem());

    item->m_OwnerID = player->GetUniqueID();
    item->m_Item = inv->GetItem(position);

    if (!item->m_Item.get_id())
        return MSG_ERROR_ARG;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (!item->m_Item.is_dropable())
    {
        srv_pkt::ItemUse(pkt,srv_pkt::ITEM_ERROR_DROP_ITEM);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    inv->RemoveItem(position);

    /// DROP ITEMS IN A VALID POSITION
    Coord new_pos;
    Coord mpos;

    do
    {
        new_pos = player->get_position();

        new_pos.set_x(new_pos.get_x() + (rand()%10)*sin((rand()%360)*3.1415/180));
        new_pos.set_y(new_pos.get_y() + (rand()%10)*sin((rand()%360)*3.1415/180));
    }
    while (!m_server->GetWorld()->resolveMotion(player->get_position(),new_pos,&mpos));

    new_pos = mpos;

    item->set_position(new_pos);

    m_server->GetWorld()->InsertItem(item);

    srv_pkt::Item(pkt,position,srv_pkt::ITEM_MOVE_DROP);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnItemMove_FromInvToFloor_G (IPacket &packet)
{
    enum
    {
        ITEM_ETC_GOLD_01 = 1,
        ITEM_ETC_GOLD_02 = 2,
        ITEM_ETC_GOLD_03 = 3
    };

    uint64_t amount = packet.Read<uint64_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (!player->spend_gold(amount))
        return MSG_ERROR_ARG;

    uint32_t id = 0;

    if ( amount <= 1000)
        id = ITEM_ETC_GOLD_01;
    else if ( amount <= 10000)
        id = ITEM_ETC_GOLD_02;
    else
        id = ITEM_ETC_GOLD_03;

    boost::shared_ptr<DropableItem> item(new DropableItem());

    item->m_GoldAmount = amount;
    item->m_OwnerID = player->GetUniqueID();
    item->m_Item.set_quantity(amount);

    try
    {
        item->m_Item = m_server->GetWorld()->GetItemFactory()->Create(id);

        /// DROP ITEMS IN A VALID POSITION
        Coord new_pos;
        Coord mpos;

        do
        {
            new_pos = player->get_position();

            new_pos.set_x(new_pos.get_x() + (rand()%10)*sin((rand()%360)*3.1415/180));
            new_pos.set_y(new_pos.get_y() + (rand()%10)*sin((rand()%360)*3.1415/180));
        }
        while (!m_server->GetWorld()->resolveMotion(player->get_position(),new_pos,&mpos));

        new_pos = mpos;

        item->set_position(new_pos);

        m_server->GetWorld()->InsertItem(item);

        boost::shared_ptr<OPacket> pkt(new OPacket);

        srv_pkt::Item(pkt,srv_pkt::ITEM_MOVE_GOLD_DROP,amount);
        m_connection->Send(pkt);
    }
    catch (const Exception::InvalidItemID &error)
    {
        syslog(LOG_INFO,"Failed to create item with ID = %i",id);
    }

    return MSG_SUCCESS;
}

int StateGame::OnItemMove_FromNpcToInv (IPacket &packet)
{
    uint8_t page = packet.Read<uint8_t>();

    uint8_t pos = packet.Read<uint8_t>();

    uint16_t quantity = packet.Read<uint16_t>();

    uint32_t npcID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<NPC> npc = m_server->GetWorld()->FindNPC_CloseToPosition(npcID,player->get_position());

    if (!npc || !IsShop(npc->get_id()))
        return MSG_ERROR_ARG;

    DB::SHOP::ItemID select_query;
    uint32_t item_id = select_query(m_server->DBConnection(),npc->get_npc_name(),page,pos);

#if defined DEBUG
    syslog(LOG_INFO,"Item ID = %i",item_id);
#endif

    if (!item_id)
    {
#if defined DEBUG
    syslog(LOG_INFO,"Invalid Item ID = %i",item_id);
#endif
        return MSG_SUCCESS;
    }

    DB::SHOP::Price price_query;
    std::vector<DB::SHOP::Price::price_type> price_req = price_query(m_server->DBConnection(),item_id);

    if (price_req.empty())
    {
#if defined DEBUG
        syslog(LOG_INFO,"Item Price for %i is empty.",item_id);
#endif
        return MSG_SUCCESS;
    }

    uint32_t gold = 0;
    uint32_t gp = 0;
    uint32_t honor = 0;

    for (size_t i = 0; i < price_req.size(); ++i)
    {
        switch(price_req[i].first)
        {
        case PRICE_GOLD:
            gold = price_req[i].second*quantity;
            break;
        case PRICE_GUILD_POINTS:
            gp = price_req[i].second*quantity;
            break;
        case PRICE_HONOR_POINTS:
            honor = price_req[i].second*quantity;
            break;
        default:
#if defined DEBUG
            syslog(LOG_INFO,"Unknow price type %i",price_req[i].first);
#endif
            break;
        }
    }

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if(!player->spend_gold(gold))
    {
#if defined DEBUG
        syslog(LOG_INFO,"Insufficient gold");
#endif
        return MSG_SUCCESS;
    }

    if (gp)
    {
        boost::shared_ptr<Guild> guild = player->get_guild();

        if (!guild)
        {
#if defined DEBUG
            syslog(LOG_INFO,"Dont belong to a guild.");
#endif
            return MSG_SUCCESS;
        }

        if (!guild->spend_gp(gp))
        {
            srv_pkt::ItemMove(pkt,srv_pkt::ITEM_ERROR_INSUFFICIENT_GP);
            m_connection->Send(pkt);
            return MSG_SUCCESS;
        }

        DB::GUILD::GP gp_query;
        gp_query(m_server->DBConnection(),guild->get_id(),guild->get_gp());
    }

    try
    {
        Item new_item = m_server->GetWorld()->GetItemFactory()->Create(item_id);

        boost::shared_ptr<Storage> inv = player->get_storage(STORAGE_INVENTORY);

        new_item.set_place(PLACE_INVENTORY);

        uint16_t item_quantity = 0;

        if (quantity > new_item.get_max_stack())
            item_quantity = new_item.get_max_stack();
        else
            item_quantity = quantity;

        new_item.set_quantity(item_quantity);

        uint8_t new_pos = inv->get_empty_position(13);

        if (new_pos == 255)
        {
            srv_pkt::ItemMove(pkt,srv_pkt::ITEM_ERROR_INVENTORY_FULL);
            m_connection->Send(pkt);
            return MSG_SUCCESS;
        }

        new_item.set_position(new_pos);

        inv->InsertItem(new_item);

        srv_pkt::Item(pkt,page,pos,new_pos,item_quantity);
        m_connection->Send(pkt);

        quantity -= item_quantity;
    }
    catch (const Exception::InvalidItemID &error)
    {
        syslog(LOG_INFO,"Failed to create Item with ID = %i",item_id);
    }

    return MSG_SUCCESS;
}

int StateGame::OnItemMove_FromInvToNpc (IPacket &packet)
{
    uint8_t src = packet.Read<uint8_t>();

    uint16_t quantity = packet.Read<uint16_t>();

    uint32_t npcID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Storage> st = player->get_storage(STORAGE_INVENTORY);

    if (!st)
        return MSG_ERROR;

    if (src >= st->GetCapacity())
        return MSG_ERROR_ARG;

    Item item = st->GetItem(src);

    if(item.get_position() != src || !item.get_id())
        return MSG_SUCCESS;

    uint32_t total = item.get_sell_price()*item.get_quantity();

    player->gain_gold(total);

    st->RemoveItem(src);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    srv_pkt::ItemSell(pkt,src,quantity,npcID);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnItemMove_FromTransportToTransport (IPacket &packet)
{
    uint32_t transportID = packet.Read<uint32_t>();

    uint8_t src = packet.Read<uint8_t>();

    uint8_t dst = packet.Read<uint8_t>();

    uint16_t quantity = packet.Read<uint16_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (!player->m_transport || player->m_transport->GetUniqueID() != transportID)
        return MSG_ERROR_ARG;

    boost::shared_ptr<Storage> st = player->m_transport->GetStorage();

    if (src >= st->GetCapacity() || dst >= st->GetCapacity())
        return MSG_ERROR_ARG;

    st->MoveItem(src,dst,quantity);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    srv_pkt::ItemMove_Transport(pkt,transportID,src,dst,quantity);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnItemMove_FromNpcToTransport (IPacket &packet)
{
    uint32_t reserv = packet.Read<uint32_t>();

    uint8_t page = packet.Read<uint8_t>();

    uint8_t pos = packet.Read<uint8_t>();

    uint16_t quantity = packet.Read<uint16_t>();

    uint32_t npcID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Transport> transport = player->GetTransport();

    if (reserv || !transport || transport->type() != ET_COS_TRANSPORT)
        return MSG_ERROR_ORDER;

    boost::shared_ptr<NPC> npc;
    npc = m_server->GetWorld()->FindNPC_CloseToPosition(npcID,player->get_position());

    if (!npc || !IsSpecialShop(npc->get_id()))
        return MSG_ERROR_ARG;

    DB::SHOP::ItemID select_query;
    uint32_t item_id = select_query(m_server->DBConnection(),npc->get_npc_name(),page,pos);

    syslog(LOG_INFO,"ItemID = %i",item_id);

    if (!item_id)
    {
#if defined DEBUG
        syslog(LOG_INFO,"Item not found.");
#endif
        return MSG_SUCCESS;
    }

    DB::SHOP::Price price_query;
    std::vector<DB::SHOP::Price::price_type> price_req = price_query(m_server->DBConnection(),item_id);

    if (price_req.empty())
    {
#if defined DEBUG
        syslog(LOG_INFO,"Item Price for %i is empty.",item_id);
#endif
        return MSG_SUCCESS;
    }

    uint32_t gold = 0;

    for (size_t i = 0; i < price_req.size(); ++i)
    {
        switch(price_req[i].first)
        {
        case PRICE_GOLD:
            gold = price_req[i].second*quantity;
            break;
        }
    }

    try
    {
        boost::shared_ptr<Storage> inv = transport->GetStorage();

        if (!inv)
            return MSG_SUCCESS;

        Item new_item = m_server->GetWorld()->GetItemFactory()->Create(item_id);

        if (quantity > new_item.get_max_stack())
            return MSG_ERROR_ARG;

        boost::shared_ptr<OPacket> pkt(new OPacket);

        if(!player->spend_gold(gold))
        {
#if defined DEBUG
            syslog(LOG_INFO,"Insufficient gold");
#endif
            return MSG_SUCCESS;
        }

        new_item.set_place(STORAGE_TRANSPORT);

        srv_pkt::TransportOperation(pkt,srv_pkt::BUYING_GOODS);
        m_connection->Send(pkt);

        new_item.set_quantity(quantity);

        uint8_t new_pos = inv->get_empty_position(0);

        if (new_pos == 255)
        {
            srv_pkt::ItemMove(pkt,srv_pkt::ITEM_ERROR_INVENTORY_FULL);
            m_connection->Send(pkt);
            return MSG_SUCCESS;
        }

        new_item.set_position(new_pos);

        inv->InsertItem(new_item);

        pkt.reset(new OPacket);

        srv_pkt::ItemMove_Transport(pkt,player->m_transport->GetUniqueID(),page,pos,new_pos,quantity);
        m_connection->Send(pkt);

        if (!transport->SpawnTaskID())
        {
            BTask task;
            task.task = BG_TASK_SPAWN_JOB_NPC;
            task.seconds = 30;
            task.entity = transport;
            task.arg = player->get_job().get_type();

            task.ID = m_server->GetWorld()->InitBackgroundTask(task);

            transport->SpawnTaskID(task.ID);
        }

        transport->CalculateTradeScale();

        pkt.reset(new OPacket);

        srv_pkt::JobTradeScale(pkt,transport->TradeScale());
        m_connection->Send(pkt);
    }
    catch (const Exception::InvalidItemID &error)
    {
        syslog(LOG_INFO,"Failed to create Item with ID = %i",item_id);
    }

    return MSG_SUCCESS;
}

int StateGame::OnItemMove_FromTransportToNpc (IPacket &packet)
{
    uint32_t transportID = packet.Read<uint32_t>();

    uint8_t pos = packet.Read<uint8_t>();

    uint16_t quantity = packet.Read<uint16_t>();

    uint32_t npcID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Transport> transport = player->GetTransport();

    if (!transport || transport->GetUniqueID() != transportID)
        return MSG_ERROR_ARG;

    boost::shared_ptr<NPC> npc;
    npc = m_server->GetWorld()->FindNPC_CloseToPosition(npcID,player->get_position());

    if (!npc || !IsSpecialShop(npc->get_id()))
        return MSG_ERROR_ARG;

    Job job = player->get_job();

    if (job.get_type() != JOB_THIEF || job.get_type() != JOB_TRADER)
        return MSG_ERROR_ORDER;

    boost::shared_ptr<Storage> st = transport->GetStorage();

    if (!st)
        return MSG_ERROR;

    if (pos >= st->GetCapacity())
        return MSG_ERROR_ARG;

    Item item = st->GetItem(pos);

    st->RemoveItem(pos);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::ItemMove_Transport(pkt,transportID,pos,quantity,npcID);
        m_connection->Send(pkt);
    }

    transport->CalculateTradeScale();

    Market market = m_server->GetWorld()->GetTradeManager()->FindMarket(npc->get_id());

    uint16_t profit = 100;

    if (market.refCharID == npc->get_id())
    {
        std::map<uint32_t,MarketStock>::const_iterator iter = market.Chart.find(item.get_id());

        if (iter != market.Chart.end())
        {
            profit = Profit(iter->second,item.get_quantity(),iter->second.QuantitySold,market.Max);
        }
        else
            profit = 50;    /// ITEM FROM THE SAME SHOP AS THE NPC
    }

    uint64_t reward = (item.get_sell_price()*item.get_quantity()*profit)/100;

    if (profit > 50)
    {
        m_server->GetWorld()->GetTradeManager()->AddQuantity(npc->get_id(),item.get_id(),item.get_quantity());

        DB::JOB::XP xp_query;
        DB::JOB::Level level_query;

        uint32_t XP = (profit*job.get_level()*item.get_quantity())/100;

        boost::shared_ptr<Party> party = player->get_party();

        if (party)
        {
            /// REMOVE THE PLAYERS THAT ARE NOT HUNTER FROM THE MEMBER LIST
            std::vector<boost::shared_ptr<Player> > members = party->get_member_list();

            Job mjob;
            std::vector<boost::shared_ptr<Player> >::iterator iter;
            for (iter = members.begin(); iter != members.end();)
            {
                mjob = (*iter)->get_job();

                if (mjob.get_type() == JOB_HUNTER)
                    ++iter;
                else
                    iter = members.erase(iter);
            }

            if (members.size())
            {
                reward /= 2;

                uint64_t hunter_reward = reward/members.size();

                for (iter = members.begin(); iter != members.end(); ++iter)
                {
                    mjob = (*iter)->get_job();

                    (*iter)->gain_gold(hunter_reward);

                    if (job.get_level() < 7)
                    {
                        uint64_t total = job.get_experience()+XP*mjob.get_level()/job.get_level();

                        LevelProperty info = m_server->GetWorld()->GetXPInformation(job.get_level()+1);

                        if (info.RequiredJobXP && total > info.RequiredJobXP)
                        {
                            job.set_level(job.get_level()+1);
                            total -= info.RequiredJobXP;

                            level_query(m_server->DBConnection(),(*iter)->ID(),job.get_level());
                        }

                        job.set_experience(total);

                        pkt.reset(new OPacket);

                        if (pkt)
                        {
                            srv_pkt::JobXP(pkt,job.get_type(),XP,job.get_level());
                            m_connection->Send(pkt);
                        }

                        xp_query(m_server->DBConnection(),(*iter)->ID(),total);
                    }
                }
            }
        }

        if (job.get_level() < 7)
        {
            uint64_t total = job.get_experience()+XP;

            LevelProperty info = m_server->GetWorld()->GetXPInformation(job.get_level()+1);

            if (info.RequiredJobXP && total > info.RequiredJobXP)
            {
                job.set_level(job.get_level()+1);
                total -= info.RequiredJobXP;

                level_query(m_server->DBConnection(),player->ID(),job.get_level());
            }

            job.set_experience(total);

            pkt.reset(new OPacket);

            if (pkt)
            {
                srv_pkt::JobXP(pkt,job.get_type(),XP,job.get_level());
                m_connection->Send(pkt);
            }

            xp_query(m_server->DBConnection(),player->ID(),total);
        }
    }

    player->gain_gold(reward);

    if (!transport->has_goods())
    {
        pkt.reset(new OPacket);

        if (pkt)
        {
            srv_pkt::TransportOperation(pkt,srv_pkt::SOLD_GOODS);
            m_connection->Send(pkt);
        }

        if (transport->SpawnTaskID())
            m_server->GetWorld()->TerminateBackgroundTask(transport->SpawnTaskID());
    }
    else
    {
        pkt.reset(new OPacket);

        srv_pkt::JobTradeScale(pkt,transport->TradeScale());
        m_connection->Send(pkt);
    }

    ///TODO:
    /*
    [S -> C][35BE][2 bytes]
    00 40
    */

    return MSG_SUCCESS;
}

int StateGame::OnItemMove_FromMallToInv (IPacket &packet)
{
    enum
    {
        SECTION_ALCHEMY = 0,
        SECTION_AVATAR = 1,
        SECTION_CONSUME = 2,
        SECTION_PET = 3,
        SECTION_PREMIUM = 4
    };

    uint8_t mall_version = packet.Read<uint8_t>();       ///ALWAYS 0x36 I THINK!!

    uint8_t page = packet.Read<uint8_t>();      ///ALWAYS 1 I THINK!!

    uint8_t section = packet.Read<uint8_t>();   ///SUB MENU IE: Expendables,Avatar,Pet,Premium,Alchemy

    uint8_t tab = packet.Read<uint8_t>();   ///SUB SUB MENU IE: (Avatar) - Stall, Hat, Dress, Balloon

    uint8_t pos = packet.Read<uint8_t>();

    std::string name = packet.Read();

    uint16_t quantity = packet.Read<uint16_t>();

    uint32_t points = packet.Read<uint32_t>();

    uint32_t bonus = packet.Read<uint32_t>();

    uint32_t pkitemID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    if (mall_version != 0x36)
        return MSG_ERROR_ARG;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    quantity = 1;

    boost::shared_ptr<Storage> inv = player->get_storage(STORAGE_INVENTORY);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    /// CHECK THATS THEIR ENOUGH SPACE IN THE INVENTORY FOR ALL ITEMS
    uint8_t new_pos = 12;
    for (uint16_t i = 0; i < quantity; ++i)
    {
        new_pos = inv->get_empty_position(new_pos+1);

        if (new_pos == 255)
        {
            std::cout << "Inventory full." << std::endl;
            return MSG_SUCCESS;
        }
    }

    DB::SHOP::ItemID select_query;
    uint32_t item_id = select_query(m_server->DBConnection(),name,pkitemID);

    if (!item_id)
    {
#if defined DEBUG
        syslog(LOG_INFO,"OnItemMove_FromMallToInv - Couldnt find Item with PKID = %i",pkitemID);
#endif
        return MSG_ERROR_ARG;
    }

    Item new_item = m_server->GetWorld()->GetItemFactory()->Create(item_id);

    uint32_t current_silk =  static_cast<srv::Connection*>(m_connection)->Silk();

    DB::SHOP::Price price_query;
    std::vector<DB::SHOP::Price::price_type> price_req = price_query(m_server->DBConnection(),item_id);

    if (price_req.empty())
    {
        syslog(LOG_INFO,"Item %i dont have an associated price.",item_id);
        return MSG_SUCCESS;
    }

    uint16_t shop_amount = m_server->GetWorld()->GetShopItemStack(item_id);

    if (!shop_amount)
    {
        syslog(LOG_INFO,"Item %i dont have a quantity associated.",item_id);
        shop_amount = 1;
    }
    else
    {
        if (shop_amount > new_item.get_max_stack())
            shop_amount = new_item.get_max_stack();
    }

    for (size_t i = 0; i < price_req.size(); ++i)
    {
        if (price_req[i].first == 2)
        {
            if (price_req[i].second*quantity > current_silk)
                return MSG_ERROR_ORDER;
            else
                current_silk -= price_req[i].second*quantity;
        }
    }

    static_cast<srv::Connection*>(m_connection)->Silk(current_silk);

    srv_pkt::PlayerSilk(pkt,current_silk);
    m_connection->Send(pkt);

    pkt.reset(new OPacket);

    uint32_t accountID = static_cast<srv::Connection*>(m_connection)->AccountID();
    clt_pkt::WriteAccountNotify32(pkt,clt_pkt::ACCOUNT_NOTIFY_SILK,accountID,current_silk);
    m_server->SendMaster(pkt);

    new_item.set_place(PLACE_INVENTORY);
    new_item.set_quantity(shop_amount);

    new_pos = 12;

    new_pos = inv->get_empty_position(new_pos+1);

    new_item.set_position(new_pos);

    inv->InsertItem(new_item);

    pkt.reset(new OPacket);

    srv_pkt::Item(pkt,page,section,tab,pos,new_pos,1);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnItemMove_FromGuildStToGuildSt (IPacket &packet)
{
    uint8_t src = packet.Read<uint8_t>();

    uint8_t dest = packet.Read<uint8_t>();

    uint16_t quantity = packet.Read<uint16_t>();

    uint32_t npcID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (player->get_selected_object() != npcID)
        return MSG_ERROR_ARG;

    boost::shared_ptr<NPC> npc = m_server->GetWorld()->FindNPC_CloseToPosition(npcID,player->get_position());

    if (!npc || !IsGuildManager(npc->get_id()))
        return MSG_ERROR_ARG;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild)
    {
        syslog(LOG_INFO,"The player dont belong to a guild.");
        return MSG_ERROR_ORDER;
    }

    if (guild->get_level() < 2)
        return MSG_ERROR_ORDER;

    boost::shared_ptr<Storage> st = guild->get_guild_storage();

    st->MoveItem(src,dest,quantity);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    srv_pkt::Item(pkt,src,dest,quantity,srv_pkt::ITEM_MOVE_GUILD);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnItemMove_FromInvToGuildSt (IPacket &packet)
{
    uint8_t src = packet.Read<uint8_t>();

    uint8_t dest = packet.Read<uint8_t>();

    uint32_t npcID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (player->get_selected_object() != npcID)
        return MSG_ERROR_ARG;

    boost::shared_ptr<NPC> npc = m_server->GetWorld()->FindNPC_CloseToPosition(npcID,player->get_position());

    if (!npc || !IsGuildManager(npc->get_id()))
        return MSG_ERROR_ARG;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild)
    {
        syslog(LOG_INFO,"The player dont belong to a guild.");
        return MSG_ERROR_ORDER;
    }

    if (guild->get_level() < 2)
    {
        syslog(LOG_INFO,"Only guilds of level 2 have storage.");
        return MSG_ERROR_ORDER;
    }

    boost::shared_ptr<Storage> inv = player->get_storage(STORAGE_INVENTORY);

    boost::shared_ptr<Storage> st = guild->get_guild_storage();

    Item itm = inv->GetItem(src);

    if (!itm.get_id())
        return MSG_ERROR_ARG;

    uint32_t total = itm.get_storage_feed()*itm.get_quantity();

    if (!total)
        total = 1;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (!player->spend_gold(itm.get_storage_feed()))
    {
        syslog(LOG_INFO,"Insufficient gold.");
        ///Packet p1;
        ///p1.WriteOpcode(0xB495);
        ///p1.Write<uint8_t>(2);
        ///p1.Write<uint8_t>(7);

        ///send(p1);

        return MSG_SUCCESS;
    }

    inv->RemoveItem(src);

    itm.set_position(dest);
    itm.set_place(PLACE_GUILD);

    st->InsertItem(itm);

    srv_pkt::Item(pkt,srv_pkt::ITEM_MOVE_GUILD_DEPOSIT,src,dest);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnItemMove_FromGuildStToInv (IPacket &packet)
{
    uint8_t src = packet.Read<uint8_t>();

    uint8_t dest = packet.Read<uint8_t>();

    uint32_t npcID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (player->get_selected_object() != npcID)
        return MSG_ERROR_ARG;

    boost::shared_ptr<NPC> npc = m_server->GetWorld()->FindNPC_CloseToPosition(npcID,player->get_position());

    if (!npc || !IsGuildManager(npc->get_id()))
        return MSG_ERROR_ARG;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild)
    {
        syslog(LOG_INFO,"The player dont belong to a guild.");
        return MSG_ERROR_ORDER;
    }

    if (guild->get_level() < 2)
    {
        syslog(LOG_INFO,"Only guilds of level 2 have storage.");
        return MSG_ERROR_ORDER;
    }

    boost::shared_ptr<Storage> st = guild->get_guild_storage();

    Item itm = st->GetItem(src);

    if (!itm.get_id())
        return MSG_ERROR_ORDER;

    st->RemoveItem(src);

    itm.set_position(dest);
    itm.set_place(PLACE_INVENTORY);

    boost::shared_ptr<Storage> inv = player->get_storage(STORAGE_INVENTORY);

    inv->InsertItem(itm);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    srv_pkt::Item(pkt,srv_pkt::ITEM_MOVE_GUILD_GOLD_WITHDRAW,src,dest);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnItemMove_GuildSt_Deposit_Gold (IPacket &packet)
{
    uint64_t amount = packet.Read<uint64_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild)
    {
#if defined DEBUG
        syslog(LOG_INFO,"The player dont belong to a guild.");
#endif
        return MSG_ERROR_ORDER;
    }

    if (player->spend_gold(amount))
    {
        guild->set_stored_gold(guild->get_stored_gold()+amount);

        boost::shared_ptr<OPacket> pkt(new OPacket);

        srv_pkt::Item(pkt,srv_pkt::ITEM_MOVE_GUILD_GOLD_DEPOSIT,amount);
        m_connection->Send(pkt);

        DB::GUILD::Gold query;
        query(m_server->DBConnection(),guild->get_id(),guild->get_stored_gold());
    }
    else
        return MSG_ERROR_ARG;

    return MSG_SUCCESS;
}

int StateGame::OnItemMove_GuildSt_Withdraw_Gold (IPacket &packet)
{
    uint64_t amount = packet.Read<uint64_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Guild> guild = player->get_guild();

    if (!guild)
    {
#if defined DEBUG
        syslog(LOG_INFO,"The player dont belong to a guild.");
#endif
        return MSG_ERROR_ORDER;
    }

    if (guild->get_stored_gold() >= amount)
    {
        guild->set_stored_gold(guild->get_stored_gold() - amount);

        player->gain_gold(amount);

        boost::shared_ptr<OPacket> pkt(new OPacket);

        srv_pkt::Item(pkt,srv_pkt::ITEM_MOVE_GUILD_WITHDRAW,amount);
        m_connection->Send(pkt);

        DB::GUILD::Gold query;
        query(m_server->DBConnection(),guild->get_id(),guild->get_stored_gold());
    }
    else
        return MSG_ERROR_ARG;

    return MSG_SUCCESS;
}

int StateGame::OnItemMove_FromInvToAva (IPacket &packet)
{
    uint8_t src = packet.Read<uint8_t>();

    uint8_t dest = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (player->IsDead())
        return MSG_SUCCESS;

    boost::shared_ptr<Storage> src_store = player->get_storage(STORAGE_INVENTORY);

    if (!src_store)
        return MSG_ERROR;

    boost::shared_ptr<Storage> dest_store = player->get_storage(STORAGE_AVATAR);

    if (!dest_store)
        return MSG_ERROR;

    Item isrc = src_store->GetItem(src);

    if (!isrc.get_id())
        return MSG_ERROR_ORDER;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (!player->check_gender(isrc))
    {
        srv_pkt::ItemMove(pkt,srv_pkt::ITEM_ERROR_DIFFERENT_SEX);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    if (player->IsTaskInterruptable())
        m_server->GetWorld()->TerminateTask(player->GetUniqueID());

    src_store->RemoveItem(src);

    isrc.set_position(dest);
    isrc.set_place(PLACE_AVATAR);

    dest_store->InsertItem(isrc);

    srv_pkt::ItemEquip(pkt,player->GetUniqueID(),dest,isrc.get_id(),0);
    m_server->SendChannel(pkt,player->get_position().get_zone());

    pkt.reset(new OPacket);

    srv_pkt::Item(pkt,src,dest,0,srv_pkt::ITEM_MOVE_AVATAR_EQUIP);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnItemMove_FromAvaToInv (IPacket &packet)
{
    uint8_t src = packet.Read<uint8_t>();

    uint8_t dest = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (player->IsDead())
        return MSG_SUCCESS;

    if (player->IsTaskInterruptable())
        m_server->GetWorld()->TerminateTask(player->GetUniqueID());

    boost::shared_ptr<Storage> dest_store = player->get_storage(STORAGE_INVENTORY);

    if (!dest_store)
        return MSG_ERROR;

    boost::shared_ptr<Storage> src_store = player->get_storage(STORAGE_AVATAR);

    if (!src_store)
        return MSG_ERROR;

    Item isrc = src_store->GetItem(src);

    if (!isrc.get_id())
        return MSG_ERROR_ORDER;

    src_store->RemoveItem(src);

    isrc.set_position(dest);
    isrc.set_place(PLACE_INVENTORY);

    dest_store->InsertItem(isrc);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    srv_pkt::ItemUnequip(pkt,player->GetUniqueID(),src,isrc.get_id());
    m_server->SendChannel(pkt,player->get_position().get_zone());

    pkt.reset(new OPacket);

    srv_pkt::Item(pkt,src,dest,0,srv_pkt::ITEM_MOVE_AVATAR_UNEQUIP);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnItemBuyBack (IPacket &packet)
{
    uint32_t npcID = packet.Read<uint32_t>();

    uint8_t slot = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    return MSG_SUCCESS;
}

int StateGame::OnItemUse (IPacket &packet)
{
    uint8_t pos = packet.Read<uint8_t>();

    uint16_t type = packet.Read<uint16_t>();

    uint8_t itypes[ITEM_TYPE_FLAG_SIZE];

    itypes[0] = type & 0x01;
    itypes[1] = (type >> 1) & 0x01;
    itypes[2] = (type >> 2) & 0x07;
    itypes[3] = (type >> 5) & 0x03;
    itypes[4] = (type >> 7) & 0x0F;
    itypes[5] = (type >> 11) & 0x0F;

    if (itypes[1] != 0 || itypes[2] != 3)
        return MSG_ERROR_ARG;

    uint8_t ret;

    switch(ItemType(itypes[3],itypes[4],itypes[5]))
    {
    case ITEM_HP_POT:
        ret = OnItemUse_HPPots(pos,packet);
        break;
    case ITEM_MP_POT:
        ret = OnItemUse_MPPots(pos,packet);
        break;
    case ITEM_ALL_POT:
        ret = OnItemUse_VigorPots(pos,packet);
        break;
    case ITEM_PURIFICATION_PILL:
        ret = OnItemUse_PurificationPill(pos,packet);
        break;
    case ITEM_BERSEK_SCROLL:
        ret = OnItemUse_ZerkPotion(pos,packet);
        break;
    case ITEM_CURE_ALL:
        ret = OnItemUse_CureAllPill(pos,packet);
        break;
    case ITEM_RETURN_SCROLL:
        ret = OnItemUse_Return(pos,packet);
        break;
    case ITEM_REVERSE_RETURN_SCROLL:
        ret = OnItemUse_ReverseReturn(pos,packet);
        break;
    case ITEM_COS_SCROLL:  
        ret = OnItemUse_TransportScroll(pos,packet);
        break;
    case ITEM_COS_HP_POT:
        ret = OnItemUse_TransportPot(pos,packet);
        break;
    case ITEM_COS_CURE_ALL:
        ret = OnItemUse_TransportCureAll(pos,packet);
        break;
    case ITEM_GLOBAL_CHAT:
        ret = OnItemUse_Global(pos,packet);
        break;
    case ITEM_STALL_AVATAR_SCROLL:
        ret = OnItemUse_StallAvatar(pos,packet);
        break;
    case ITEM_FORT_REPAIR_KIT:
        break;
    case ITEM_FORTRESS_SHOCK_BOMB:
        break;
    case ITEM_FORTRESS_MOB_SCROLL:
        break;
    case ITEM_FORTRESS_GUARD_SCROLL:
        break;
    case ITEM_FORTRESS_STRUCT_SCROLL:
        break;
    case ITEM_FORTRESS_FLAG_SCROLL:
        break;
    case ITEM_FORTRESS_SCROLL:
        break;
    case ITEM_SKILL_RESTORE_POT:
        break;
    case ITEM_GUILD_SOLDIER_SCROLL:
        break;
    case ITEM_GUILD_EMBLEM_SCROLL:
        break;
    case ITEM_GUILD_RECALL_SCROLL:
        break;
    case ITEM_PANDORA_BOX:
        break;
    case ITEM_PANDORA_PARTY_BOX:
        break;
    case ITEM_BUFF:
        ret = OnItemUse_BuffScroll(pos,packet);
        break;
    case ITEM_ETC_SCROLL:
        break;
    case ITEM_XP_SCROLL:
        break;
    case ITEM_SP_TICKET:
        break;
    case ITEM_XP_SP_TICKET:
        break;
    case ITEM_SP_EXP_SCROLL:
        break;
    case ITEM_RESURRECT_SCROLL:
        ret = OnItemUse_ResurrectScroll(pos,packet);
        break;
    case ITEM_REPAIR_SCROLL:
        ret = OnItemUse_RepairHammer(pos,packet);
        break;
    case ITEM_TRANSGENDER_SCROLL:
        break;
    case ITEM_SKIN_SCROLL:
        ret = OnItemUse_SkinChange(pos,packet);
        break;
    case ITEM_WAREHOUSE_SCROLL:
        break;
    case ITEM_ENCHANT_SCROLL:
        break;
    case ITEM_STAT_SCROLL:
        break;
    case ITEM_DEVIL_EXT_SCROLL:
        break;
    case ITEM_STORAGE_SPACE_EXPANSION:
        ret = OnItemUse_StorageSpaceExpansion(pos,packet);
        break;
    case ITEM_MONSTER_MASK:
        break;
    case ITEM_COS_PET:
        ret = OnItemUse_AttackPet(pos,packet);
        break;
    case ITEM_PET_SCROLL:
        break;
    case ITEM_HGP_POT:
        break;
    case ITEM_COS_GRAB_PET:
        break;
    case ITEM_COS_EXT_SCROLL:
        break;
    case ITEM_COS_REVIVAL:
        break;
    default:
#if defined DEBUG
        syslog(LOG_INFO,"Unhandled type %i",type);
        PrintPacketToSyslog(packet);
#endif
        break;
    }

    return MSG_SUCCESS;
}

bool StateGame::valid_item (std::map<uint8_t,Item>::iterator &iiter, const uint16_t item_type, const uint8_t level)
{
    if (!iiter->second.get_id())
        return false;

    if (iiter->second.get_type() != item_type)
    {
#if defined DEBUG
        syslog(LOG_INFO,"Invalid item type - %i %i.",iiter->second.get_type(),item_type);
#endif
        return false;
    }

    if (!iiter->second.get_quantity())
        return false;

    if (!iiter->second.check_level(level))
    {
#if defined DEBUG
        syslog(LOG_INFO,"Insufficient Level.");
#endif
        return false;
    }

    return true;
}

void StateGame::use_item (std::map<uint8_t,Item>::iterator &iiter,const uint16_t item_type)
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    boost::shared_ptr<Storage> store = player->get_storage(STORAGE_INVENTORY);

    iiter->second.set_quantity(iiter->second.get_quantity()-1);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    srv_pkt::ItemUse(pkt,iiter->second.get_position(),iiter->second.get_quantity(),item_type);
    m_connection->Send(pkt);

    pkt.reset(new OPacket);

    srv_pkt::ItemAnimation(pkt,player->GetUniqueID(),iiter->second.get_id());
    m_server->SendChannel(pkt,player->get_position().get_zone());

    if (!iiter->second.get_quantity())
    {
        pkt.reset(new OPacket);

        srv_pkt::Item(pkt,srv_pkt::ITEM_MOVE_REMOVE,iiter->second.get_position(),2);
        m_connection->Send(pkt);

        store->RemoveItem(iiter->second.get_position());
    }
    else
    {
        DB::ITEM::Quantity query;
        query(m_server->DBConnection(),iiter->second.ID(),iiter->second.get_quantity());
    }
}

int StateGame::OnItemUse_Return (const uint8_t pos, IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (player->IsDead())
    {
        srv_pkt::ItemUse(pkt,srv_pkt::ITEM_ERROR_USE_WHILE_DEAD);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    if (player->TransportID())
    {
        boost::shared_ptr<Transport> transport = player->GetTransport();

        if (transport->has_goods())
        {
#if defined DEBUG
            syslog(LOG_INFO,"Cant teleport while carrying goods in a transport.");
#endif
            return MSG_SUCCESS;
        }
    }

    if (!player->IsTaskInterruptable())
        return MSG_SUCCESS;

    boost::shared_ptr<Storage> store = player->get_storage(STORAGE_INVENTORY);

    if (!store)
        return MSG_ERROR;

    Storage::iterator iiter = store->find(pos);

    if (!valid_item(iiter,ITEM_RETURN_SCROLL,player->level()))
        return MSG_ERROR_ARG;

    boost::shared_ptr<ItemProperty> property = iiter->second.property();

    uint32_t teleportID = 0;

    if (!property->Effects[1].first)
    {
        Job job = player->get_job();

        if (job.get_type() != Job::THIEF)
        {
#if defined DEBUG
            syslog(LOG_INFO,"Need to enter thief guild to use.");
#endif
            return MSG_SUCCESS;
        }

        teleportID = THIEF_DEN_TELEPORT;
    }
    else
        teleportID = player->get_return_point();

    Teleport teleport = m_server->GetWorld()->FindTeleport(teleportID);

    if (!teleport.ID)
        return MSG_ERROR;

    use_item(iiter,USE_RETURN_SCROLL_V1);

    player->EstablishRecallPoint();

    player->SetCurrentTask(PLAYER_TASK_TELEPORT);

    m_server->GetWorld()->TerminateTask(player->GetUniqueID());

    Coord telpos = Coord(teleport.x,teleport.y,teleport.z,teleport.Zone);
    m_server->GetWorld()->InitTaskTeleport(player->GetUniqueID(),telpos,property->Effects[0].first);

    return MSG_SUCCESS;
}

int StateGame::OnItemUse_ReverseReturn (const uint8_t pos, IPacket &packet)
{
    enum
    {
        RETURN_RECALL_POINT = 2,
        RETURN_DEATH_PLACE,
        RETURN_LOCATION = 7
    };

    uint8_t option = packet.Read<uint8_t>();

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (player->IsDead())
    {
        srv_pkt::ItemUse(pkt,srv_pkt::ITEM_ERROR_USE_WHILE_DEAD);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    if (!player->IsTaskInterruptable())
        return MSG_SUCCESS;

    if (player->m_transport && player->m_transport->has_goods())
    {
#if defined DEBUG
        syslog(LOG_INFO,"Cant teleport while carrying goods in a transport.");
#endif
        return MSG_SUCCESS;
    }

    boost::shared_ptr<Storage> store = player->get_storage(STORAGE_INVENTORY);

    if (!store)
        return MSG_ERROR;

    Storage::iterator iiter = store->find(pos);

    if (!valid_item(iiter,ITEM_REVERSE_RETURN_SCROLL,player->level()))
        return MSG_ERROR;

    Coord coord;

    switch(option)
    {
    case RETURN_RECALL_POINT:
        {
            if (!packet.EndOfStream())
                return MSG_ERROR_SIZE;

            coord = player->GetRecallPoint();
        }
        break;
    case RETURN_DEATH_PLACE:
        {
            if (!packet.EndOfStream())
                return MSG_ERROR_SIZE;

            coord = player->GetDeathPoint();
        }
        break;
    case RETURN_LOCATION:
        {
            uint32_t locationID = packet.Read<uint32_t>();

            if (!packet.EndOfStream())
                return MSG_ERROR_SIZE;

            TeleportOptional trev = m_server->GetWorld()->FindTeleportOptional(locationID);

            if (!trev.id)
                return MSG_ERROR;

            if (!trev.free && (trev.minlv > player->level() || trev.maxlv < player->level()))
            {
#if defined DEBUG
                syslog(LOG_INFO,"Cant teleport to this place because player level dont meet the requirements.");
#endif
                return MSG_SUCCESS;
            }

            coord = Coord(trev.x,trev.y,trev.z,trev.zone);
        }
        break;
    default:
        return MSG_ERROR_ARG;
    }

    if (!coord.get_zone())
        return MSG_SUCCESS;

    boost::shared_ptr<ItemProperty> property = iiter->second.property();

    use_item(iiter,USE_REVERSE_RETURN_V1);

    player->SetCurrentTask(PLAYER_TASK_TELEPORT);
    m_server->GetWorld()->InitTaskTeleport(player->GetUniqueID(),coord,property->Effects[0].first);

    return MSG_SUCCESS;
}

int StateGame::OnItemUse_Global (const uint8_t pos, IPacket &packet)
{
    std::string msg = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Storage> store = player->get_storage(STORAGE_INVENTORY);

    if (!store)
        return MSG_ERROR;

    Storage::iterator iiter = store->find(pos);

    if (!valid_item(iiter,ITEM_GLOBAL_CHAT,player->level()))
        return MSG_SUCCESS;

    use_item(iiter,USE_GLOBAL_SCROLL);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    srv_pkt::Msg(pkt,srv_pkt::MSG_GLOBAL,player->get_name(),msg);
    m_server->SendChannel(pkt,player->get_position().get_zone());

    return MSG_SUCCESS;
}

int StateGame::OnItemUse_BuffScroll (const uint8_t pos, IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (player->IsDead())
    {
        srv_pkt::ItemUse(pkt,srv_pkt::ITEM_ERROR_USE_WHILE_DEAD);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    boost::shared_ptr<Storage> store = player->get_storage(STORAGE_INVENTORY);

    if (!store)
        return MSG_ERROR;

    Storage::iterator iiter = store->find(pos);

    if (!valid_item(iiter,ITEM_BUFF,player->level()))
        return MSG_SUCCESS;

    DB::ITEM::SkillID id_query;
    uint32_t ID = id_query(m_server->DBConnection(),iiter->second.get_id());

    boost::shared_ptr<Skill> skill = m_server->GetWorld()->GetSkillManager()->find(ID);

    if (!skill)
    {
#if defined DEBUG
        syslog(LOG_INFO,"Invalid Item Skill for item with ID = %i",iiter->second.get_id());
#endif
        return MSG_SUCCESS;
    }

    Buff buff;
    buff.CastID = rand();
    buff.SkillID = skill->id();
    buff.GroupID = skill->group_id();
    buff.Level = skill->level();
    buff.duration = skill->duration();
    buff.persistent = true;
    buff.passive = false;
    buff.Effects = skill->buff_list();

    if (player->is_buff_stackable(buff))
    {
        use_item(iiter,USE_BUFF_SCROLL_V1);

        BTask task;
        task.task = BG_TASK_BUFF;
        task.entity = player;
        task.persistent = false;
        task.seconds = skill->duration()/1000;
        task.arg = buff.SkillID;

        buff.taskID = m_server->GetWorld()->InitBackgroundTask(task);

        std::vector<std::pair<uint32_t,uint32_t> > term_buffs = player->RemoveLesserBuffs(buff);

        for (int i = 0; i < term_buffs.size(); ++i)
        {
            srv_pkt::TermBuff(pkt,term_buffs[i].second);
            m_server->SendChannel(pkt,player->get_position().get_zone());

            pkt.reset(new OPacket);

            m_server->GetWorld()->TerminateBackgroundTask(term_buffs[i].first);
        }

        player->insert_buff(buff);

        srv_pkt::InitBuff(pkt,player->GetUniqueID(),skill->id(),buff.CastID);
        m_server->SendChannel(pkt,player->get_position().get_zone());
    }
    else
    {
        srv_pkt::ItemUse(pkt,srv_pkt::ITEM_ERROR_BUFF_STACKING);
        m_connection->Send(pkt);
    }

    return MSG_SUCCESS;
}

int StateGame::OnItemUse_HPPots (const uint8_t pos, IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (player->IsDead())
    {
        srv_pkt::ItemUse(pkt,srv_pkt::ITEM_ERROR_USE_WHILE_DEAD);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    boost::shared_ptr<Storage> store = player->get_storage(STORAGE_INVENTORY);

    if (!store)
        return MSG_ERROR;

    Storage::iterator iiter = store->find(pos);

    if (!valid_item(iiter,ITEM_HP_POT,player->level()))
        return MSG_SUCCESS;

    if (!player->IsItemAvailable(iiter->second.get_type()))
        return MSG_SUCCESS;

    use_item(iiter,USE_HP_POTS_V1);

    uint32_t cooldown = 0;
    boost::shared_ptr<ItemProperty> property = iiter->second.property();

    bool heal = !player->hasStatus(STATUS_ZOMBIE);

    for ( int i = 0; i < 6; ++i)
    {
        if (property->Effects[i].first > 0 )
        {
            if (i % 2)
            {
                if (i < 2)
                {
                    if (heal)
                        player->heal_hp(property->Effects[i].first*player->max_hp()/100);
                    else
                        player->consume_hp(property->Effects[i].first*player->max_hp()/100);
                }

                cooldown = 3;
            }
            else
            {
                if (i < 2)
                {
                    if (heal)
                        player->heal_hp(property->Effects[i].first*4);
                    else
                        player->consume_hp(property->Effects[i].first*4);
                }

                if (player->race() == RACE_CH)
                    cooldown = 1;
                else
                    cooldown = 15;
            }
        }
    }

    std::map<uint32_t,Status> status = player->GetStatus();

    uint8_t op = heal ? srv_pkt::BASE_OP_INC_POT : srv_pkt::BASE_OP_DEC_HP;
    uint8_t type = status.empty() ? srv_pkt::STAT_HP : srv_pkt::STAT_HP | srv_pkt::STAT_STATUS;

    srv_pkt::HPMP(pkt,player->GetUniqueID(),op,status.size(),type,player->hp(),player->GetEncodedStatus(),status);
    m_server->SendChannel(pkt,player->get_position().get_zone());

    BTask task;
    task.task = BG_TASK_COOLDOWN_ITEM;
    task.seconds = cooldown;
    task.persistent = false;
    task.arg = iiter->second.get_type();
    task.entity = player;

    task.ID = m_server->GetWorld()->InitBackgroundTask(task);

    player->AddItemCooldown(task.ID,iiter->second.get_type());

    boost::shared_ptr<Party> party = player->get_party();

    if (party)
        party->SendHPMPNotification(player->GetUniqueID(),player->hp(),player->max_hp(),player->mp(),player->max_mp());

    return MSG_SUCCESS;
}

int StateGame::OnItemUse_MPPots (const uint8_t pos, IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (player->IsDead())
    {
        srv_pkt::ItemUse(pkt,srv_pkt::ITEM_ERROR_USE_WHILE_DEAD);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    boost::shared_ptr<Storage> store = player->get_storage(STORAGE_INVENTORY);

    if (!store)
        return MSG_ERROR;

    Storage::iterator iiter = store->find(pos);

    if (!valid_item(iiter,ITEM_MP_POT,player->level()))
        return MSG_SUCCESS;

    if (!player->IsItemAvailable(iiter->second.get_type()))
        return MSG_SUCCESS;

    use_item(iiter,USE_MP_POTS_V1);

    uint32_t cooldown = 0;
    boost::shared_ptr<ItemProperty> property = iiter->second.property();

    for ( int i = 0; i < 6; ++i)
    {
        if (property->Effects[i].first > 0 )
        {
            if (i % 2)
            {
                if (i >= 2)
                    player->heal_mp(property->Effects[i].first*player->max_mp()/100);

                cooldown = 3;
            }
            else
            {
                if (i >= 2)
                    player->heal_mp(property->Effects[i].first*4);

                if (player->race() == RACE_CH)
                    cooldown = 1;
                else
                    cooldown = 15;
            }
        }
    }

    std::map<uint32_t,Status> status = player->GetStatus();

    uint8_t type = status.empty() ? srv_pkt::STAT_MP : srv_pkt::STAT_MP | srv_pkt::STAT_STATUS;

    srv_pkt::HPMP(pkt,player->GetUniqueID(),srv_pkt::BASE_OP_INC_POT,status.size(),type,player->mp(),
        player->GetEncodedStatus(),status);

    m_server->SendChannel(pkt,player->get_position().get_zone());

    BTask task;
    task.task = BG_TASK_COOLDOWN_ITEM;
    task.seconds = cooldown;
    task.persistent = false;
    task.arg = iiter->second.get_type();
    task.entity = player;

    task.ID = m_server->GetWorld()->InitBackgroundTask(task);

    player->AddItemCooldown(task.ID,iiter->second.get_type());

    boost::shared_ptr<Party> party = player->get_party();

    if (party)
        party->SendHPMPNotification(player->GetUniqueID(),player->hp(),player->max_hp(),player->mp(),player->max_mp());

    return MSG_SUCCESS;
}

int StateGame::OnItemUse_VigorPots (const uint8_t pos, IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (player->IsDead())
    {
        srv_pkt::ItemUse(pkt,srv_pkt::ITEM_ERROR_USE_WHILE_DEAD);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    boost::shared_ptr<Storage> store = player->get_storage(STORAGE_INVENTORY);

    if (!store)
        return MSG_ERROR;

    Storage::iterator iiter = store->find(pos);

    if (!valid_item(iiter,ITEM_ALL_POT,player->level()))
        return MSG_SUCCESS;

    if (!player->IsItemAvailable(iiter->second.get_type()))
        return MSG_SUCCESS;

    use_item(iiter,USE_VIGOR_POTS_V1);

    uint32_t cooldown = 0;
    boost::shared_ptr<ItemProperty> property = iiter->second.property();

    bool heal = !player->hasStatus(STATUS_ZOMBIE);

    for ( int i = 0; i < 6; ++i)
    {
        if (property->Effects[i].first > 0 )
        {
            if (i % 2)
            {
                if (i < 2)
                {
                    if (heal)
                        player->heal_hp(property->Effects[i].first*player->max_hp()/100);
                    else
                        player->consume_hp(property->Effects[i].first*player->max_hp()/100);
                }
                else
                    player->heal_mp(property->Effects[i].first*player->max_mp()/100);

                cooldown = 3;
            }
            else
            {
                if (i < 2)
                {
                    if (heal)
                        player->heal_hp(property->Effects[i].first*4);
                    else
                        player->consume_hp(property->Effects[i].first*4);
                }
                else
                    player->heal_mp(property->Effects[i].first*4);

                if (player->race() == RACE_CH)
                    cooldown = 1;
                else
                    cooldown = 15;
            }
        }
    }

    std::map<uint32_t,Status> status = player->GetStatus();

    uint8_t op = heal ? srv_pkt::BASE_OP_INC_POT : srv_pkt::BASE_OP_DEC_HP;
    uint8_t type = status.empty() ? srv_pkt::STAT_HP : srv_pkt::STAT_HP | srv_pkt::STAT_STATUS;

    srv_pkt::HPMP(pkt,player->GetUniqueID(),op,status.size(),type,player->hp(),player->GetEncodedStatus(),status);
    m_server->SendChannel(pkt,player->get_position().get_zone());

    op = srv_pkt::BASE_OP_INC_POT;
    type = status.empty() ? srv_pkt::STAT_MP : srv_pkt::STAT_MP | srv_pkt::STAT_STATUS;

    pkt.reset(new OPacket);

    srv_pkt::HPMP(pkt,player->GetUniqueID(),op,status.size(),type,player->mp(),player->GetEncodedStatus(),status);
    m_server->SendChannel(pkt,player->get_position().get_zone());

    BTask task;
    task.task = BG_TASK_COOLDOWN_ITEM;
    task.seconds = cooldown;
    task.persistent = false;
    task.arg = iiter->second.get_type();
    task.entity = player;

    task.ID = m_server->GetWorld()->InitBackgroundTask(task);

    player->AddItemCooldown(task.ID,iiter->second.get_type());

    boost::shared_ptr<Party> party = player->get_party();

    if (party)
        party->SendHPMPNotification(player->GetUniqueID(),player->hp(),player->max_hp(),player->mp(),player->max_mp());

    return MSG_SUCCESS;
}

int StateGame::OnItemUse_PurificationPill (const uint8_t pos, IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (player->IsDead())
    {
        srv_pkt::ItemUse(pkt,srv_pkt::ITEM_ERROR_USE_WHILE_DEAD);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    boost::shared_ptr<Storage> store = player->get_storage(STORAGE_INVENTORY);

    if (!store)
        return MSG_ERROR;

    Storage::iterator iiter = store->find(pos);

    if (!valid_item(iiter,ITEM_PURIFICATION_PILL,player->level()))
        return MSG_SUCCESS;

    if (!player->IsItemAvailable(iiter->second.get_type()))
        return MSG_SUCCESS;

    use_item(iiter,USE_PURIFICATION_PILL);

    BTask task;
    task.task = BG_TASK_COOLDOWN_ITEM;
    task.seconds = 20;
    task.persistent = false;
    task.arg = iiter->second.get_type();
    task.entity = player;

    task.ID = m_server->GetWorld()->InitBackgroundTask(task);

    player->AddItemCooldown(task.ID,iiter->second.get_type());

    return MSG_SUCCESS;
}

int StateGame::OnItemUse_CureAllPill (const uint8_t pos, IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (player->IsDead())
    {
        srv_pkt::ItemUse(pkt,srv_pkt::ITEM_ERROR_USE_WHILE_DEAD);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    boost::shared_ptr<Storage> store = player->get_storage(STORAGE_INVENTORY);

    if (!store)
        return MSG_ERROR;

    Storage::iterator iiter = store->find(pos);

    if (!valid_item(iiter,ITEM_CURE_ALL,player->level()))
        return MSG_SUCCESS;

    use_item(iiter,USE_SPECIAL_PILL);

    return MSG_SUCCESS;
}

int StateGame::OnItemUse_TransportScroll (const uint8_t pos, IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (player->IsDead())
    {
        srv_pkt::ItemUse(pkt,srv_pkt::ITEM_ERROR_USE_WHILE_DEAD);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    if (player->hasWall())
    {
        return MSG_SUCCESS;
    }

    if (player->TransportID())
    {
        srv_pkt::ItemUse(pkt,srv_pkt::ITEM_ERROR_SUMMON_LIMIT);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    if (player->get_attack_state() != ATTACK_ACTION_STOP)
    {
        srv_pkt::ItemUse(pkt,srv_pkt::ITEM_ERROR_SUMMON_ATTACK_STATE);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    boost::shared_ptr<Storage> store = player->get_storage(STORAGE_INVENTORY);

    if (!store)
        return MSG_ERROR;

    Storage::iterator iiter = store->find(pos);

    if (!valid_item(iiter,ITEM_COS_SCROLL,player->level()))
        return MSG_SUCCESS;

    World *world = m_server->GetWorld();
    boost::shared_ptr<ItemProperty> property = iiter->second.property();

    std::string nameID = property->Effects[0].second;

    if (property->Effects[1].second != "xxx")
    {
        std::string lv = horse_level(property->Effects[1].second,player->level());

        if (!lv.empty())
            nameID = nameID + "_" + lv;
    }

    DB::ITEM::TransportID query;
    uint32_t refTransportID = query(m_server->DBConnection(),nameID);
    
    boost::shared_ptr<Transport> transport;
    transport = boost::shared_dynamic_cast<Transport,NPC>(world->GetCharacterFactory()->Create(refTransportID));

    if (!transport)
    {
#if defined DEBUG
        syslog(LOG_INFO,"Failed to create transport with ID = %i",refTransportID);
#endif
        return MSG_SUCCESS;
    }

    if (transport->type() == ET_COS_TRANSPORT)
    {
        Job job = player->get_job();

        if (job.get_type() == JOB_NONE)
        {
#if defined DEBUG
            syslog(LOG_INFO,"The player %s, cant summon a transport because dont have a job.",player->get_name().c_str());
#endif
            return MSG_SUCCESS;
        }

        if (!player->IsJobbing())
        {
#if defined DEBUG
            syslog(LOG_INFO,"Cant summon a transport when not equipping a job suit.");
#endif
            return MSG_SUCCESS;
        }

        switch(job.get_type())
        {
        case JOB_HUNTER:
#if defined DEBUG
            syslog(LOG_INFO,"Hunters cant summon a transport.");
#endif
            return MSG_SUCCESS;
        case JOB_THIEF:
            if (world->IsInsideTown(player->get_position()))
            {
#if defined DEBUG
                syslog(LOG_INFO,"Thief cant summon a transport inside town.");
#endif
                return MSG_SUCCESS;
            }
        default:
            break;
        }
    }

    use_item(iiter,USE_TRANSPORT_SCROLL_V1);

    transport->SetOwner(player);

    transport->set_position(player->get_position());

    player->m_transport = transport;

    world->InsertNPC(transport);

    srv_pkt::TransportStats(pkt,transport,player->GetUniqueID());
    m_connection->Send(pkt);

    pkt.reset(new OPacket);

    srv_pkt::TransportAction(pkt,player->GetUniqueID(),srv_pkt::TRANSPORT_ACTION_MOUNT,transport->GetUniqueID());
    m_server->SendChannel(pkt,player->get_position().get_zone());

    return MSG_SUCCESS;
}

int StateGame::OnItemUse_TransportPot (const uint8_t pos, const IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (player->IsDead())
    {
        srv_pkt::ItemUse(pkt,srv_pkt::ITEM_ERROR_USE_WHILE_DEAD);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    if (!player->TransportID())
    {
        return MSG_SUCCESS;
    }

    boost::shared_ptr<Transport> transport = player->GetTransport();

    if (!transport)
        return MSG_SUCCESS;

    boost::shared_ptr<Storage> store = player->get_storage(STORAGE_INVENTORY);

    if (!store)
        return MSG_ERROR;

    Storage::iterator iiter = store->find(pos);

    if (!valid_item(iiter,ITEM_COS_HP_POT,player->level()))
        return MSG_SUCCESS;

    boost::shared_ptr<ItemProperty> property = iiter->second.property();

    bool heal = !transport->hasStatus(STATUS_ZOMBIE);

    for ( int i = 0; i < 6; ++i)
    {
        if (property->Effects[i].first > 0 )
        {
            if (i % 2)
            {
                if (i < 2)
                {
                    if (heal)
                        transport->heal_hp(property->Effects[i].first*transport->max_hp()/100);
                    else
                        transport->consume_hp(property->Effects[i].first*transport->max_hp()/100);
                }
            }
            else
            {
                if (i < 2)
                {
                    if (heal)
                        transport->heal_hp(property->Effects[i].first*4);
                    else
                        transport->consume_hp(property->Effects[i].first*4);
                }
            }
        }
    }

    std::map<uint32_t,Status> status = transport->GetStatus();

    uint8_t op = heal ? srv_pkt::BASE_OP_INC_POT : srv_pkt::BASE_OP_DEC_HP;
    uint8_t type = status.empty() ? srv_pkt::STAT_HP : srv_pkt::STAT_HP | srv_pkt::STAT_STATUS;

    srv_pkt::HPMP(pkt,transport->GetUniqueID(),op,status.size(),type,transport->hp(),transport->GetEncodedStatus(),status);
    m_server->SendChannel(pkt,transport->get_position().get_zone());

    BTask task;
    task.task = BG_TASK_COOLDOWN_ITEM;
    task.seconds = 3;
    task.persistent = false;
    task.arg = iiter->second.get_type();
    task.entity = player;

    task.ID = m_server->GetWorld()->InitBackgroundTask(task);

    player->AddItemCooldown(task.ID,iiter->second.get_type());

    return MSG_SUCCESS;
}

int StateGame::OnItemUse_TransportCureAll (const uint8_t pos, const IPacket &packet)
{
    return MSG_SUCCESS;
}

int StateGame::OnItemUse_AttackPet (const uint8_t pos, IPacket &packet)
{
//    if (!packet.EndOfStream())
//        throw Exception::InvalidPacketSize();
//
//    boost::shared_ptr<Storage> store = m_player->get_storage(STORAGE_INVENTORY);
//
//    Storage::iterator iiter = store->find(pos);
//
//    if (!valid_item(iiter,ITEM_COS_PET))
//        return;
//
//    boost::shared_ptr<NPC> pet;
//
//    use_item(iiter,USE_ATTACK_PET);

    return MSG_SUCCESS;
}

int StateGame::OnItemUse_StallAvatar (const uint8_t pos, IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (player->IsDead())
        return MSG_SUCCESS;

    boost::shared_ptr<Storage> store = player->get_storage(STORAGE_INVENTORY);

    if (!store)
        return MSG_ERROR;

    Storage::iterator iiter = store->find(pos);

    if (!valid_item(iiter,ITEM_STALL_AVATAR_SCROLL,player->level()))
        return MSG_SUCCESS;

    use_item(iiter,USE_STALL_AVATAR);

    player->set_stall_avatar(iiter->second.get_id());

    DB::PLAYER::StallAvatar query;
    query(m_server->DBConnection(),player->ID(),iiter->second.get_id());

    return MSG_SUCCESS;
}

int StateGame::OnItemUse_ZerkPotion (const uint8_t pos, IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (player->IsDead())
    {
        srv_pkt::ItemUse(pkt,srv_pkt::ITEM_ERROR_USE_WHILE_DEAD);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    boost::shared_ptr<Storage> store = player->get_storage(STORAGE_INVENTORY);

    if (!store)
        return MSG_ERROR;

    Storage::iterator iiter = store->find(pos);

    if (!valid_item(iiter,ITEM_XP_SCROLL,player->level()))
        return MSG_SUCCESS;

    use_item(iiter,USE_ZERK_POT);

    /// TODO: INCREAMENT BERSERK TIME IF IT NEEDS
    player->set_berserk_count(MAX_PLAYER_BERSERK_COUNT);
    //m_player->inc_berserk_time(20);   ///ZERK POTS FROM SHOP INCREASE THE BERSERK TIME!!

    srv_pkt::PlayerBerserkGauge(pkt,player->get_berserk_count(),iiter->second.get_id());
    m_connection->Send(pkt);

    DB::PLAYER::Bersek query;
    query(m_server->DBConnection(),player->ID(),player->get_berserk_count());

    return MSG_SUCCESS;
}

int StateGame::OnItemUse_ResurrectScroll (const uint8_t pos, IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (!player->IsDead())
        return MSG_SUCCESS;

    boost::shared_ptr<Storage> store = player->get_storage(STORAGE_INVENTORY);

    if (!store)
        return MSG_ERROR;

    Storage::iterator iiter = store->find(pos);

    if (!valid_item(iiter,ITEM_RESURRECT_SCROLL,player->level()))
        return MSG_SUCCESS;

    use_item(iiter,USE_RESURRECT_SCROLL);

    boost::shared_ptr<ItemProperty> property = iiter->second.property();

    ///XP LOST RECOVERED = effect[2] (%)
    player->resurrect(true,true,0,true);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    std::map<uint32_t,Status> status = player->GetStatus();

    uint8_t op = srv_pkt::BASE_OP_INC_SELF;
    uint8_t type = status.empty() ? srv_pkt::STAT_HP : srv_pkt::STAT_HP | srv_pkt::STAT_STATUS;

    srv_pkt::HPMP(pkt,player->GetUniqueID(),op,status.size(),type,player->hp(),player->GetEncodedStatus(),status);
    m_server->SendChannel(pkt,player->get_position().get_zone());

    type = status.empty() ? srv_pkt::STAT_MP : srv_pkt::STAT_MP | srv_pkt::STAT_STATUS;

    pkt.reset(new OPacket);

    srv_pkt::HPMP(pkt,player->GetUniqueID(),op,status.size(),type,player->mp(),player->GetEncodedStatus(),status);
    m_server->SendChannel(pkt,player->get_position().get_zone());

    STask task;
    task.task = STATE_TASK_INVUNERABLE;
    task.seconds = PLAYER_INVUNERABLE_STATE_TIME;
    task.playerID = player->GetUniqueID();

    m_server->GetWorld()->InitStateTask(task);

    boost::shared_ptr<Party> party = player->get_party();

    if (party)
        party->SendHPMPNotification(player->GetUniqueID(),player->hp(),player->max_hp(),player->mp(),player->max_mp());

    return MSG_SUCCESS;
}

int StateGame::OnItemUse_RepairHammer (const uint8_t pos, IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Storage> store = player->get_storage(STORAGE_INVENTORY);

    if (!store)
        return MSG_ERROR;

    Storage::iterator iiter = store->find(pos);

    if (!valid_item(iiter,ITEM_REPAIR_SCROLL,player->level()))
        return MSG_SUCCESS;

    use_item(iiter,USE_REPAIR_HAMMER_V1);

    DB::ITEM::Durability query;
    boost::shared_ptr<OPacket> pkt;

    for (Storage::iterator i = store->begin(); i != store->end(); ++i)
    {
        uint32_t max_dur = i->second.get_max_durability();

        if (!max_dur)
            continue;

        i->second.set_durability(max_dur);

        pkt.reset(new OPacket);

        srv_pkt::ItemDurability(pkt,i->second.get_position(),max_dur);
        m_connection->Send(pkt);

        query(m_server->DBConnection(),iiter->second.ID(),max_dur);
    }

    return MSG_SUCCESS;
}

int StateGame::OnItemUse_StorageSpaceExpansion (uint8_t pos, IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (player->IsDead())
        return MSG_SUCCESS;

    boost::shared_ptr<Storage> inv = player->get_storage(STORAGE_INVENTORY);

    if (!inv)
        return MSG_ERROR;

    Storage::iterator iiter = inv->find(pos);

    if (!valid_item(iiter,ITEM_STORAGE_SPACE_EXPANSION,player->level()))
        return MSG_SUCCESS;

    boost::shared_ptr<ItemProperty> property = iiter->second.property();

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (property->Effects[1].first)
    {
        ///STORAGE EXPANSION
        if (!player->IsStorageExpanded())
        {
            use_item(iiter,USE_STORAGE_SPACE_EXPANSION);

            player->IncStorageSlots(property->Effects[0].first);

            boost::shared_ptr<Storage> st = player->get_storage(STORAGE_MAIN);

            DB::PLAYER::StorageCapacity query;
            query(m_server->DBConnection(),player->ID(),st->GetCapacity());
        }
        else
        {
            if (pkt)
            {
                srv_pkt::ItemMove(pkt,srv_pkt::ITEM_ERROR_LIMIT_REACHED);
                m_connection->Send(pkt);
            }
        }
    }
    else
    {
        if (!player->IsInvExpanded())
        {
            use_item(iiter,USE_STORAGE_SPACE_EXPANSION);

            player->IncInventorySlots(property->Effects[0].first);

            if (pkt)
            {
                srv_pkt::InventorySlots(pkt,inv->GetCapacity());
                m_connection->Send(pkt);
            }

            DB::PLAYER::InventoryCapacity query;
            query(m_server->DBConnection(),player->ID(),inv->GetCapacity());
        }
        else
        {
            if (pkt)
            {
                srv_pkt::ItemMove(pkt,srv_pkt::ITEM_ERROR_LIMIT_REACHED);
                m_connection->Send(pkt);
            }
        }
    }

    return MSG_SUCCESS;
}

int StateGame::OnItemUse_SkinChange (uint8_t pos, IPacket &packet)
{
    uint32_t refCharID = packet.Read<uint32_t>();

    uint8_t volume = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Storage> inv = player->get_storage(STORAGE_INVENTORY);

    if (!inv)
        return MSG_ERROR;

    Storage::iterator iiter = inv->find(pos);

    if (!valid_item(iiter,ITEM_SKIN_SCROLL,player->level()))
        return MSG_SUCCESS;

    ///CHECK IF THE PLAYER HAVE ANY ITEM EQUIPPED
    boost::shared_ptr<Storage> ava = player->get_storage(STORAGE_AVATAR);

    if (inv->begin()->first < Item::MAX_EQUIPMENT_SLOT || !ava->empty())
    {
        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::ItemUse(pkt,srv_pkt::ITEM_ERROR_TRANSGEND_ITEM_EQUIPPED);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    Teleport tp = m_server->GetWorld()->FindTeleport(player->get_return_point());

    if (!tp.ID)
        return MSG_SUCCESS;

    use_item(iiter,USE_SKIN_CHANGER);

    boost::shared_ptr<EntityProperty> pr = m_server->GetWorld()->GetCharacterFactory()->GetProperty(refCharID);

    player->Transform(pr);
    player->set_volume(volume);
    player->set_model(refCharID);

    player->SetCurrentTask(PLAYER_TASK_TELEPORT);

    m_server->GetWorld()->TerminateTask(player->GetUniqueID());
    m_server->GetWorld()->InitTaskTeleport(player->GetUniqueID(),Coord(tp.x,tp.y,tp.z,tp.Zone),0);

    DB::PLAYER::Skin query;
    query(m_server->DBConnection(),player->ID(),refCharID,volume);

    return MSG_SUCCESS;
}

int StateGame::OnItemRepair (IPacket &packet)
{
    enum
    {
        REPAIR_SINGLE = 1,
        REPAIR_ALL
    };

    uint32_t npcID = packet.Read<uint32_t>();

    uint8_t action = packet.Read<uint8_t>();

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<NPC> npc;
    npc = m_server->GetWorld()->FindNPC_CloseToPosition(npcID,player->get_position());

    if (!npc || (!IsBlacksmithShop(npc->get_id()) && !IsProtectorShop(npc->get_id())))
        return MSG_ERROR_ARG;

    /*
    CH BOW D1 - 10/47 repair cost = 171
    CH BOW D1 - 0/47 repair cost = 540
    CH BOW D1 - 46/47 repair cost = 4
    CH BOW D1 - 45/47 repair cost = 9
    */

    uint8_t ret;

    switch(action)
    {
    case REPAIR_SINGLE:
        ret = OnItemRepair_Single(packet);
        break;
    case REPAIR_ALL:
        ret = OnItemRepair_All(packet);
        break;
    default:
        ret = MSG_ERROR_ARG;
        break;
    }

    if (ret == MSG_SUCCESS)
    {
        boost::shared_ptr<OPacket> pkt(new OPacket);

        srv_pkt::ItemRepair(pkt);
        m_connection->Send(pkt);
    }

    return ret;
}

int StateGame::OnItemRepair_Single (IPacket &packet)
{
    uint8_t pos = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Storage> inv = player->get_storage(STORAGE_INVENTORY);

    if (!inv)
        return MSG_ERROR;

    Storage::iterator iter = inv->find(pos);

    if (iter == inv->end())
        return MSG_ERROR_ARG;

    if (iter->second.IsRepairable())
    {
        boost::shared_ptr<OPacket> pkt(new OPacket);

        if (!player->spend_gold(iter->second.RepairCost()))
        {
#if defined DEBUG
            syslog(LOG_INFO,"not enough gold to repair item.");
#endif
            return MSG_SUCCESS;
        }

        iter->second.Repair();

        srv_pkt::ItemDurability(pkt,pos,iter->second.get_durability());
        m_connection->Send(pkt);

        DB::ITEM::Durability query;
        query(m_server->DBConnection(),iter->second.ID(),iter->second.get_durability());
    }

    return MSG_SUCCESS;
}

int StateGame::OnItemRepair_All (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Storage> inv = player->get_storage(STORAGE_INVENTORY);

    if (!inv)
        return MSG_ERROR;

    DB::ITEM::Durability query;
    boost::shared_ptr<OPacket> pkt(new OPacket);

    for (Storage::iterator i = inv->begin(); i != inv->end(); ++i)
    {
        if (i->second.IsRepairable())
        {
            if (!player->spend_gold(i->second.RepairCost()))
                break;

            i->second.Repair();

            srv_pkt::ItemDurability(pkt,i->second.get_position(),i->second.get_durability());
            m_connection->Send(pkt);

            query(m_server->DBConnection(),i->second.ID(),i->second.get_durability());
        }
    }

    return MSG_SUCCESS;
}

int StateGame::OnItemAvatarMagicOption (IPacket &packet)
{
    uint8_t pos = packet.Read<uint8_t>();

    std::string name = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Storage> inv = player->get_storage(STORAGE_INVENTORY);

    if (!inv)
        return MSG_ERROR;

    Storage::iterator iter = inv->find(pos);

    if (iter == inv->end())
        return MSG_ERROR_ARG;

    /// AVATAR ITEMS ARE ALWAYS DEGREE 1
    MagicOptionProperty opt = m_server->GetWorld()->GetMagicOptionProperty(name,1);

    if (!opt.ID)
        return MSG_ERROR_ARG;

    /// CHECK THAT THE MAGIC OPTION CAN BE ASSIGNED TO THIS ITEM.
    bool valid = false;

    for (int i = 0; i < MAGIC_OPT_MAX_REQ_COUNT; ++i)
    {
        if (opt.Requirement[i] == "avatar_helm" && iter->second.get_type() == ITEM_AVATAR_HAT)
        {
            valid = true;
            break;
        }
        else if (opt.Requirement[i] == "avatar_mail" && iter->second.get_type() == ITEM_AVATAR_ARMOR)
        {
            valid = true;
            break;
        }
        else if (opt.Requirement[i] == "avatar_mail_addon" && iter->second.get_type() == ITEM_AVATAR_ATTACH)
        {
            valid = true;
            break;
        }
    }

    if (!valid)
        return MSG_ERROR_ARG;

    if (opt.Values.empty())
    {
#if defined DEBUG
        syslog(LOG_INFO,"Option = %s dont have a value list.",name.c_str());
#endif
        return MSG_SUCCESS;
    }

    if (!iter->second.is_opts_full())
    {
        Item::attribute new_attr = std::make_pair(opt.ID,opt.Values.begin()->Val);

        iter->second.insert_magic_option(new_attr);

        boost::shared_ptr<OPacket> pkt(new OPacket);

        srv_pkt::ItemAvatar(pkt,iter->second);
        m_connection->Send(pkt);

        DB::ITEM::MagicOption query;
        query(m_server->DBConnection(),iter->second.ID(),new_attr.first,new_attr.second);
    }

    return MSG_SUCCESS;
}

void StateGame::connect_storage_signals (boost::shared_ptr<Storage> storage)
{
    if (storage)
    {
        storage->signal_insert.connect(boost::bind(&StateGame::OnStorageEvent_Insert,this,_1,_2));

        storage->signal_position.connect(boost::bind(&StateGame::OnStorageEvent_Position,this,_1,_2));

        storage->signal_remove.connect(boost::bind(&StateGame::OnStorageEvent_Remove,this,_1));

        storage->signal_quantity.connect(boost::bind(&StateGame::OnStorageEvent_Quantity,this,_1,_2));
    }
}

void StateGame::disconnect_storage_signals (boost::shared_ptr<Storage> storage)
{
    if (storage)
    {
        storage->signal_insert.disconnect(boost::bind(&StateGame::OnStorageEvent_Insert,this,_1,_2));

        storage->signal_position.disconnect(boost::bind(&StateGame::OnStorageEvent_Position,this,_1,_2));

        storage->signal_remove.disconnect(boost::bind(&StateGame::OnStorageEvent_Remove,this,_1));

        storage->signal_quantity.disconnect(boost::bind(&StateGame::OnStorageEvent_Quantity,this,_1,_2));
    }
}

uint32_t StateGame::OnStorageEvent_Insert (const Item &item, const STORAGE_TYPE &type)
{
    uint32_t ID;
    uint32_t itemID = 0;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return itemID;

    boost::shared_ptr<Storage> storage;

    switch(type)
    {
    case STORAGE_GUILD:
        ID = player->get_guild()->get_id();
        storage = player->get_guild()->get_guild_storage();
        break;
    case STORAGE_MAIN:
        ID = static_cast<srv::Connection*>(m_connection)->AccountID();
        storage = player->get_storage(type);
        break;
    default:
        ID = player->ID();
        storage = player->get_storage(type);
        break;
    }

    if (storage)
    {
        DB::ITEM::Create query;
        itemID = query(m_server->DBConnection(),ID,item);
    }

    return itemID;
}

void StateGame::OnStorageEvent_Position (const uint32_t itemID, const uint8_t pos)
{
    DB::ITEM::Position query;
    query(m_server->DBConnection(),itemID,pos);
}

void StateGame::OnStorageEvent_Remove (const uint32_t itemID)
{
    DB::ITEM::Delete query;
    query(m_server->DBConnection(),itemID);
}

void StateGame::OnStorageEvent_Quantity (const uint32_t itemID, const uint16_t quantity)
{
    DB::ITEM::Quantity query;
    query(m_server->DBConnection(),itemID,quantity);
}

std::string horse_level (const std::string &str, const int level)
{
    std::string t1,t2;

    int prev_lv = 0, next_lv = 0;
    size_t beg = 0, end = 0;

    do
    {
        end = str.find_first_of(',',beg);

        t1 = str.substr(beg,end-beg);

        next_lv = atoi(t1.c_str());

        if (level == next_lv || (end == std::string::npos && level > next_lv))
            return t1;
        else if (prev_lv < level && level <= next_lv)
            return t2;

        beg = end+1;
        prev_lv = next_lv;
        t2 = t1;


    } while (end != std::string::npos);

    return std::string();
}

bool IsPositionValid (Item &item, const uint8_t pos)
{
    set_default_position(item);

    if (item.get_position() != pos)
    {
        bool valid = false;

        if (item.get_type() != ITEM_CH_RING && item.get_type() != ITEM_EU_RING)
            return false;

        if (item.get_position() != Item::LEFT_RING_SLOT && item.get_position() != Item::RIGHT_RING_SLOT)
            return false;
    }

    return true;
}

int IsItemEquipable (const boost::shared_ptr<Player> &player, const Item &item,
    const boost::shared_ptr<Storage> &store)
{
    /// CHECK THAT THE ITEM ISNT BROKEN.
    if (item.get_position() <= Item::SHIELD_SLOT && item.get_max_durability() && !item.get_durability())
        return srv_pkt::ITEM_ERROR_BROKEN_ITEM;

    /// CHECK IF THE ITEM RACE AND CHARACTER RACE IS THE SAME
    if (item.get_req_race() != player->race() && item.get_req_race() != RACE_ANY)
        return srv_pkt::ITEM_ERROR_DIFFERENT_RACE;

    /// CHECK IF THE ITEM GENDER AND THE CHARACTER GENDER IS THE SAME
    if(item.get_gender() != GENDER_ANY && item.get_gender() != player->get_gender())
        return srv_pkt::ITEM_ERROR_DIFFERENT_SEX;

    /// CHECK IF THE PLAYER MASTERY LEVEL AND CHARACTER LEVEL IS EQUAL OR ABOVE THE ITEM REQUIREMENTS
    if (!player->check_mastery(item) && !item.check_level(player->level()))
        return srv_pkt::ITEM_ERROR_LOWER_LV;

    if (item.get_position() <= Item::FOOT_SLOT)
    {
        /// CHECK IF THE ITEM TYPE CAN BE EQUIPED TOGETHER.
        for (Storage::iterator i = store->begin(); i != store->end(); ++i)
        {
            if (i->second.get_position() > Item::FOOT_SLOT)
                break;

            if (!i->second.check_combination(item))
                return srv_pkt::ITEM_ERROR_DIFFERENT_TYPE;
        }
    }

    return 0;
}

bool StateGame::CheckWeaponShieldSlots (const Item &item, const boost::shared_ptr<Player> &player,
    const boost::shared_ptr<Storage> &store)
{
    uint8_t pos = item.get_position();
    boost::shared_ptr<OPacket> pkt(new OPacket);

    /// CHECK IF THE ITEM IS A TWO HANDED WEAPON
    if (pos == Item::WEAPON_SLOT)
    {
        Item itm_shield_ammo = store->GetItem(Item::SHIELD_SLOT);

        if (itm_shield_ammo.get_id())
        {
            bool need_space = false;

            switch(item.get_type())
            {
            case ITEM_CH_SWORD:
            case ITEM_CH_BLADE:
                if (itm_shield_ammo.get_type() != ITEM_CH_SHIELD)
                    need_space = true;
                break;
            case ITEM_CH_BOW:
                if (itm_shield_ammo.get_type() != ITEM_ARROW)
                    need_space = true;
                break;
            case ITEM_EU_SWORD:
            case ITEM_EU_STAFF:
            case ITEM_EU_DARKSTAFF:
                if (itm_shield_ammo.get_type() != ITEM_EU_SHIELD)
                    need_space = true;
                break;
            case ITEM_EU_CROSSBOW:
                if (itm_shield_ammo.get_type() != ITEM_BOLT)
                    need_space = true;
                break;
            default:
                need_space = true;
                break;
            }

            if (need_space)
            {
                uint8_t empty_pos = store->get_empty_position(13);

                if (empty_pos == pos)
                    empty_pos = store->get_empty_position(pos+1);

                if (empty_pos == 255)
                {
                    srv_pkt::ItemMove(pkt,srv_pkt::ITEM_ERROR_INVENTORY_FULL);
                    m_connection->Send(pkt);
                    return false;
                }

                itm_shield_ammo.set_position(empty_pos);

                store->MoveItem(Item::SHIELD_SLOT,empty_pos,itm_shield_ammo.get_quantity());

                srv_pkt::Item(pkt,Item::SHIELD_SLOT,empty_pos,itm_shield_ammo.get_quantity(),
                    srv_pkt::ITEM_MOVE_INV_TO_INV);
                m_connection->Send(pkt);

                pkt.reset(new OPacket);

                if (itm_shield_ammo.get_type() != ITEM_ARROW && itm_shield_ammo.get_type() != ITEM_BOLT)
                {
                    srv_pkt::ItemUnequip(pkt,player->GetUniqueID(),Item::SHIELD_SLOT,itm_shield_ammo.get_id());
                    m_server->SendChannel(pkt,player->get_position().get_zone());
                }
            }
        }
    }

    /// CHECK IF UR EQUIPING A SHIELD OR AMMO
    else if (item.get_position() == Item::SHIELD_SLOT)
    {
        Item weapon = store->GetItem(Item::WEAPON_SLOT);

        if (weapon.get_id())
        {
            bool need_space = true;

            switch(weapon.get_type())
            {
            case ITEM_CH_SWORD:
            case ITEM_CH_BLADE:
                if (item.get_type() == ITEM_CH_SHIELD)
                    need_space = false;
                break;
            case ITEM_CH_BOW:
                if (item.get_type() == ITEM_ARROW)
                    need_space = false;
                break;
            case ITEM_EU_CROSSBOW:
                if (item.get_type() == ITEM_BOLT)
                    need_space = false;
                break;
            case ITEM_EU_SWORD:
            case ITEM_EU_DARKSTAFF:
            case ITEM_EU_STAFF:
                if (item.get_type() == ITEM_EU_SHIELD)
                    need_space = false;
                break;
            default:
                break;
            }

            if (need_space)
            {
                uint8_t empty_pos = store->get_empty_position(13);

                if (empty_pos == pos)
                    empty_pos = store->get_empty_position(pos+1);

                if (empty_pos == 255)
                {
                    srv_pkt::ItemMove(pkt,srv_pkt::ITEM_ERROR_INVENTORY_FULL);
                    m_connection->Send(pkt);
                    return false;
                }

                weapon.set_position(empty_pos);

                store->MoveItem(Item::WEAPON_SLOT,empty_pos,weapon.get_quantity());

                srv_pkt::Item(pkt,Item::WEAPON_SLOT,empty_pos,weapon.get_quantity(),srv_pkt::ITEM_MOVE_INV_TO_INV);
                m_connection->Send(pkt);
            }
        }
    }

    return true;
}
