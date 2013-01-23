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

#include "packet_spawn.hpp"
#include "opcodes_shard_server.hpp"

#include <packet.hpp>

/// SRLF INCLUDES
#include <mob.hpp>
#include <player.hpp>
#include <dropable_item.hpp>
#include <interactive_npc.hpp>
#include <teleport_building.hpp>
#include <stall.hpp>
#include <guild.hpp>
#include <guild_member.hpp>
#include <transport.hpp>

/// GLOBAL INCLUDES
#include <boost/bind.hpp>

namespace srv_pkt
{

void WritePosition (const boost::shared_ptr<OPacket> &pkt, const Coord &position)
{
    pkt->Write<int16_t>(position.get_zone());
    pkt->Write(position.get_x());
    pkt->Write(position.get_z());
    pkt->Write(position.get_y());
}

void WriteBuffs (const boost::shared_ptr<OPacket> &pkt, const std::map<uint32_t,Buff> &buffs)
{
    uint8_t count = 0;
    std::map<uint32_t,Buff>::const_iterator siter;

    for (siter = buffs.begin(); siter != buffs.end(); ++siter)
    {
        if (!siter->second.passive)
            ++count;
    }

    pkt->Write<uint8_t>(count);
    for (siter = buffs.begin(); siter != buffs.end(); ++siter)
    {
        if (!siter->second.passive)
        {
            pkt->Write<uint32_t>(siter->first);
            pkt->Write<uint32_t>(siter->second.CastID);
        }
    }
}

void WriteNPC (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<NPC> &npc)
{
    pkt->Write<uint32_t>(npc->get_id());
    pkt->Write<uint32_t>(npc->GetUniqueID());

    WritePosition(pkt,npc->get_position());

    pkt->Write<uint16_t>(npc->get_angle());
    pkt->Write<uint8_t>(npc->is_moving());
    pkt->Write<uint8_t>(npc->get_movement_state() & 0x01);

    ///NO DESTINATION
    if (!npc->is_moving())
    {
        pkt->Write<uint8_t>(0);
        pkt->Write<uint16_t>(npc->get_angle());
    }
    else
    {
        Coord dst = npc->get_destination();

        pkt->Write<uint16_t>(dst.get_zone());
        pkt->Write<uint16_t>(dst.get_x());
        pkt->Write<uint16_t>(dst.get_z());
        pkt->Write<uint16_t>(dst.get_y());
    }

    pkt->Write<uint8_t>(npc->IsDead()+1);

    if (!npc->is_moving())
        pkt->Write<uint8_t>(npc->get_stance_state());
    else
        pkt->Write<uint8_t>(npc->get_movement_state());

    pkt->Write<uint8_t>(0);
    pkt->Write(npc->walking_speed());
    pkt->Write(npc->running_speed());
    pkt->Write(npc->berseker_speed());

    WriteBuffs(pkt,npc->get_buff_list());

    switch(npc->type())
    {
    case ET_MOB:
    case ET_MOB_EVENT:
    case ET_MOB_QUEST:
        {
            pkt->Write<uint8_t>(2);
            pkt->Write<uint8_t>(1);
            pkt->Write<uint8_t>(5);

            boost::shared_ptr<Mob> mob = boost::shared_dynamic_cast<Mob,NPC>(npc);

            if (mob)
                pkt->Write<uint8_t>(mob->Quality());
        }
        break;
    case ET_MOB_THIEF:
    case ET_MOB_HUNTER:
        pkt->Write<uint8_t>(2);
        pkt->Write<uint8_t>(1);
        pkt->Write<uint8_t>(5);
        pkt->Write<uint8_t>(0);
        pkt->Write<uint8_t>(0x59);  /// GENDER (0x59 MALE - 0x18 FEMALE)
        break;
    case ET_COS_TRANSPORT:
        {
            pkt->Write<uint8_t>(0);

            boost::shared_ptr<Transport> tr = boost::shared_dynamic_cast<Transport,NPC>(npc);

            if (tr)
            {
                boost::shared_ptr<Player> owner = tr->GetOwner();

                if (owner)
                {
                    pkt->Write(owner->get_job().get_alias());
                    pkt->Write<uint8_t>(0x01);
                    pkt->Write<uint8_t>(0x00);
                    pkt->Write<uint32_t>(owner->GetUniqueID());
                }
            }
        }
        break;
    case ET_INTERACTIVE:
        {
            pkt->Write<uint8_t>(2);

            boost::shared_ptr<InteractiveNPC> itv = boost::shared_dynamic_cast<InteractiveNPC,NPC>(npc);

            std::vector<uint8_t> tasks = itv->GetAvailableTasks(100);   /// TODO: A HACK DEPENDING ON EACH CHAR LV

            pkt->Write<uint8_t>(tasks.size());
            for (std::vector<uint8_t>::const_iterator i = tasks.begin(); i != tasks.end(); ++i)
                pkt->Write<uint8_t>(*i);
        }
        break;
    default:
        pkt->Write<uint8_t>(0);
        break;
    }
}

void WritePlayer (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Player> &player)
{
    pkt->Write<uint32_t>(player->get_id());
    pkt->Write<uint8_t>(player->get_volume());
    pkt->Write<uint8_t>(player->get_rank());
    pkt->Write<uint8_t>((player->help_mark() << 1) | (player->level() < 20));

    /// WRITE ITEM INFORMATION

    uint8_t equip_size = 0;
    boost::shared_ptr<Storage> il = player->get_storage(STORAGE_INVENTORY);

    for (Storage::const_iterator iter = il->begin(); iter != il->end() && iter->first < ::Item::EARRING_SLOT; ++iter)
        if (iter->second.get_type() != ITEM_ARROW && iter->second.get_type() != ITEM_BOLT)
            ++equip_size;

    pkt->Write<uint8_t>(il->GetCapacity());
    pkt->Write<uint8_t>(equip_size);

    for (Storage::const_iterator iter = il->begin(); iter != il->end() && iter->first < ::Item::EARRING_SLOT; ++iter)
    {
        if (iter->second.get_type() != ITEM_ARROW && iter->second.get_type() != ITEM_BOLT)
        {
            pkt->Write<uint32_t>(iter->second.get_id());
            pkt->Write<uint8_t>(iter->second.get_enchant_lv());
        }
    }

    il = player->get_storage(STORAGE_AVATAR);

    pkt->Write<uint8_t>(il->GetCapacity());
    pkt->Write<uint8_t>(il->size());

    for (Storage::const_iterator iter = il->begin(); iter != il->end(); ++iter)
    {
        pkt->Write<uint32_t>(iter->second.get_id());
        pkt->Write<uint8_t>(0);
    }

    pkt->Write<uint8_t>(0);  ///DUPLICATE ITEM LIST

/// WRITE PLAYER POSITION AND MOVEMENT INFORMATION
    pkt->Write<uint32_t>(player->GetUniqueID());

    WritePosition(pkt,player->get_position());

    pkt->Write<uint16_t>(player->get_angle());
    pkt->Write<uint8_t>(player->is_moving());
    pkt->Write<uint8_t>(player->get_movement_state() & 0x01);

    ///NO DESTINATION
    if (!player->is_moving())
    {
        pkt->Write<uint8_t>(0);
        pkt->Write<uint16_t>(player->get_angle());
    }
    else
    {
        Coord dst = player->get_destination();

        pkt->Write<uint16_t>(dst.get_zone());
        pkt->Write<uint16_t>(dst.get_x());
        pkt->Write<uint16_t>(dst.get_z());
        pkt->Write<uint16_t>(dst.get_y());
    }

    pkt->Write<uint8_t>(player->IsDead()+1);

    if (!player->is_moving())
        pkt->Write<uint8_t>(player->get_stance_state());
    else
        pkt->Write<uint8_t>(player->get_movement_state());

    pkt->Write<uint8_t>(player->IsBerserk());
    pkt->Write(player->walking_speed());
    pkt->Write(player->running_speed());
    pkt->Write(player->berseker_speed());

    WriteBuffs(pkt,player->get_buff_list());

    std::string name;
    Job job = player->get_job();

    if (player->IsJobbing())
        name = job.get_alias();
    else
        name = player->get_name();

    pkt->Write(name);
    pkt->Write<uint8_t>(job.get_type());
    pkt->Write<uint8_t>(job.get_level());
    pkt->Write<uint8_t>(0);              //PK Status (00 - Normal) (01-Purple) (02-Red)
    pkt->Write<uint8_t>(player->m_transport ? 1 : 0 );
    pkt->Write<uint8_t>(0);

    if (player->m_transport)
        pkt->Write<uint32_t>(player->m_transport->GetUniqueID());

    pkt->Write<uint8_t>(0);
    pkt->Write<uint8_t>(player->is_stalling() << 2);
    pkt->Write<uint8_t>(0);

/// WRITE GUILD AND UNION INFORMATION
    boost::shared_ptr<Guild> guild = player->get_guild();

    std::string guild_name;

    if (guild)
        guild_name = guild->get_name();

    pkt->Write(guild_name);

    if (!player->IsJobbing())
    {
        if (guild)
        {
            pkt->Write<uint32_t>(guild->get_id());

            Guild::const_iterator iter = guild->find(player->get_name());

            if (iter != guild->end())
            {
                pkt->Write(iter->second->GrantName);
                pkt->Write<uint32_t>(guild->get_emblem_index());
                pkt->Write<uint32_t>(guild->get_union_id());
                pkt->Write<uint32_t>(0);
                pkt->Write<uint8_t>(guild->is_at_war());
                pkt->Write<uint8_t>(iter->second->Position);
            }
        }
        else
        {
            pkt->Write<uint32_t>(0);
            pkt->Write<uint16_t>(0);
            pkt->Write<uint64_t>(0);
            pkt->Write<uint32_t>(0);
            pkt->Write<uint16_t>(0);
        }
    }

    /// WRITE STALL INFORMATION
    if (player->is_stalling())
    {
        pkt->Write(player->get_stall()->get_name());
        pkt->Write<uint32_t>(player->get_stall_avatar());
    }

    pkt->Write<uint8_t>(0);      //(UN)Equip Timer (00 - 0A)
    pkt->Write<uint8_t>(0xFF);   //PK Flag (FF - Disabled) (01 - CTF) (02 - Enabled)
}

void SpawnBegin (const boost::shared_ptr<OPacket> &pkt, uint16_t count,const SPAWN_TYPE type)
{
    pkt->WriteOpcode(SERVER_MULTI_SPAWN_BEGIN);
    pkt->Write<uint8_t>(type);
    pkt->Write<uint16_t>(count);
}

void Spawn (const boost::shared_ptr<OPacket> &pkt, NPCVector::const_iterator begin, NPCVector::const_iterator end)
{
    pkt->WriteOpcode(SERVER_MULTI_SPAWN);
    std::for_each(begin,end,boost::bind(WriteNPC,pkt,_1));
}

void Spawn (const boost::shared_ptr<OPacket> &pkt, const NPCVector &list)
{
    pkt->WriteOpcode(SERVER_MULTI_SPAWN);

    std::for_each(list.begin(),list.end(),boost::bind(WriteNPC,pkt,_1));
}

void Spawn (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<NPC> &npc)
{
    pkt->WriteOpcode(SERVER_MULTI_SPAWN);
    WriteNPC(pkt,npc);
}

void Spawn (const boost::shared_ptr<OPacket> &pkt, const TeleportVector &list)
{
    pkt->WriteOpcode(SERVER_MULTI_SPAWN);

    for (size_t i = 0; i < list.size(); ++i)
    {
        pkt->Write<uint32_t>(list[i]->m_Property.refID);
        pkt->Write<uint32_t>(list[i]->GetUniqueID());
        pkt->Write<int16_t>(list[i]->m_Property.Zone);
        pkt->Write(list[i]->m_Property.x);
        pkt->Write(list[i]->m_Property.z);
        pkt->Write(list[i]->m_Property.y);
        pkt->Write<uint16_t>(0);
    }
}

void Spawn (const boost::shared_ptr<OPacket> &pkt, const PlayerVector &list)
{
    pkt->WriteOpcode(SERVER_MULTI_SPAWN);

    for (PlayerVector::const_iterator it = list.begin(); it != list.end(); ++it)
        WritePlayer(pkt,*it);
}

void Spawn (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<DropableItem> &item)
{
    pkt->WriteOpcode(SERVER_SINGLE_SPAWN);

    pkt->Write<uint32_t>(item->m_Item.get_id());

    if (!item->m_OwnerAlias.empty())
        pkt->Write(item->m_OwnerAlias);

    if (item->m_Item.get_type() == ITEM_GOLD)
    {
        pkt->Write<uint32_t>(item->m_GoldAmount);
        pkt->Write<uint32_t>(item->GetUniqueID());

        WritePosition(pkt,item->get_position());

        pkt->Write<uint16_t>(item->get_angle());
        pkt->Write<uint16_t>(0);
    }
    else
    {
        if (item->m_Item.IsWeapon() || item->m_Item.IsShield() || item->m_Item.IsAccessory()
            || item->m_Item.IsArmor() || item->m_Item.IsCape())
        {
            pkt->Write<uint8_t>(0);
        }

        pkt->Write<uint32_t>(item->GetUniqueID());

        WritePosition(pkt,item->get_position());

        pkt->Write<uint16_t>(item->get_angle());
        pkt->Write<uint8_t>(1);
        pkt->Write<uint32_t>(item->m_OwnerAuth);
        pkt->Write<uint8_t>(item->m_Item.get_attr_list().size());
    }

    pkt->Write<uint8_t>(6);
    pkt->Write<uint32_t>(item->m_OwnerID); //OWNER ID
}

void Spawn (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Player> &player)
{
    pkt->WriteOpcode(SERVER_MULTI_SPAWN);

    WritePlayer(pkt,player);
}

void Spawn (const boost::shared_ptr<OPacket> &pkt, const std::vector<uint32_t> &list)
{
    pkt->WriteOpcode(SERVER_MULTI_SPAWN);

    for ( std::vector<uint32_t>::const_iterator i = list.begin(); i != list.end(); ++i)
        pkt->Write<uint32_t>(*i);
}

void Spawn (const boost::shared_ptr<OPacket> &pkt,
    std::set<uint32_t>::const_iterator begin, std::set<uint32_t>::const_iterator end)
{
    pkt->WriteOpcode(SERVER_MULTI_SPAWN);

    for (std::set<uint32_t>::const_iterator i = begin; i != end; ++i)
        pkt->Write<uint32_t>(*i);
}

void Spawn (const boost::shared_ptr<OPacket> &pkt, const std::set<std::pair<uint32_t,std::string> > &list)
{
    pkt->WriteOpcode(SERVER_MULTI_SPAWN);

    for (std::set<std::pair<uint32_t,std::string> >::const_iterator i = list.begin(); i != list.end(); ++i)
        pkt->Write<uint32_t>(i->first);
}

void Despawn (const boost::shared_ptr<OPacket> &pkt, uint32_t ID)
{
    pkt->WriteOpcode(SERVER_SINGLE_DESPAWN);
    pkt->Write<uint32_t>(ID);
}

void SpawnEnd (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_MULTI_SPAWN_END);
}

}
