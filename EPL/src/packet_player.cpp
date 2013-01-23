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

#include "packet_player.hpp"
#include "packet_base_item.hpp"
#include "opcodes_shard_server.hpp"

#include <packet.hpp>

/// SRLF INCLUDES
#include <player.hpp>
#include <skill.hpp>
#include <storage.hpp>

/// GLOBAL INCLUDES
#include <boost/bind.hpp>

namespace srv_pkt
{

void WriteStorage (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Storage> &storage)
{
    pkt->Write<uint8_t>(storage->GetCapacity());
    pkt->Write<uint8_t>(storage->size());

    Storage::const_iterator iter;
    for (iter = storage->begin(); iter != storage->end(); ++iter )
        WriteItem(pkt,iter->second);
}

void WriteMastery (const boost::shared_ptr<OPacket> &pkt, const Mastery &mastery)
{
    pkt->Write<uint8_t>(0x01);
    pkt->Write<uint32_t>(mastery.ID());
    pkt->Write<uint8_t>(mastery.Level());
}

void WriteHotkey (const boost::shared_ptr<OPacket> &pkt, const Hotkey &hotkey)
{
    pkt->Write<uint8_t>(hotkey.pos);
    pkt->Write<uint8_t>(hotkey.type);
    pkt->Write<uint32_t>(hotkey.id);
}

void InitScreen (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_PLAYER_INFO_BEGIN);
}

void EndScreen (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_PLAYER_INFO_END);
}

void PlayerInfo (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Player> &player,
    const uint32_t accountID)
{
    pkt->WriteOpcode(SERVER_PLAYER_INFO);
    pkt->Write<uint16_t>(0xEA49);
    pkt->Write<uint16_t>(0x25AB);
    pkt->Write<uint32_t>(player->get_model());
    pkt->Write<uint8_t>(player->get_volume());
    pkt->Write<uint8_t>(player->level());
    pkt->Write<uint8_t>(player->get_maxlv());
    pkt->Write<uint64_t>(player->get_exp());
    pkt->Write<uint32_t>(player->get_spexp());
    pkt->Write<uint64_t>(player->get_gold());
    pkt->Write<uint32_t>(player->get_sp());
    pkt->Write<uint16_t>(player->get_stat());
    pkt->Write<uint8_t>(player->get_berserk_count());
    pkt->Write<uint32_t>(0);  //UNK!!
    pkt->Write<uint32_t>(player->hp());
    pkt->Write<uint32_t>(player->mp());
    pkt->Write<uint8_t>((player->help_mark() << 1) | (player->level() < 20));
    pkt->Write<uint8_t>(0);  //PK LEVEL
    pkt->Write<uint16_t>(0);
    pkt->Write<uint32_t>(0);
    pkt->Write<uint8_t>(player->get_rank());

    //Item Information
    WriteStorage(pkt,player->get_storage(STORAGE_INVENTORY));
    WriteStorage(pkt,player->get_storage(STORAGE_AVATAR));

    pkt->Write<uint8_t>(0);

    //Mastery Information
    MasteryTree tree = player->get_mastery_tree();

    std::for_each(tree.begin(),tree.end(),
        boost::bind(WriteMastery,pkt,boost::bind(&MasteryTree::value_type::second,_1)));

    //Skill Information
    //Beggining of skill list
    pkt->Write<uint16_t>(2);

    std::set<uint32_t> slist = player->get_skill_list();

    for (std::set<uint32_t>::const_iterator siter = slist.begin(); siter != slist.end(); ++siter)
    {
            pkt->Write<uint8_t>(0x01);
            pkt->Write<uint32_t>(*siter);
            pkt->Write<uint8_t>(0x01);
    }

    //End of skill list
    pkt->Write<uint8_t>(2);

    pkt->Write<uint16_t>(1);
    pkt->Write<uint32_t>(1);
    /*
    TODO: CHECK!!!
    //Finished Quest Information
    std::vector<Quest> ql = m_Characters[m_CurrentChar].GetCompletedQuests();

    p1.Write<uint16_t>(ql.size());
    for (int l = 0; l < ql.size(); ++l)
    {
            p1.Write<uint32_t>(ql[l].GetID());
    }*/

    //On progress quest information
    pkt->Write<uint8_t>(0);
    /*ql = m_Characters[m_CurrentChar].GetPendingQuests();

    p1.Write<uint8_t>(ql.size());
    for (int h = 0; h < ql.size(); ++h )
    {
        p1.Write<uint32_t>(ql[h].GetID());

        //From packets!! unknown meaning!!
        p1.Write<uint8_t>(0x10);
        p1.Write<uint8_t>(0x00);
        p1.Write<uint8_t>(0x18);
        p1.Write<uint8_t>(0x01);
        p1.Write<uint8_t>(0x01);
        p1.Write<uint8_t>(0x01);
        p1.Write<uint8_t>(0x01);

        p1.Write<uint16_t>(ql[h].GetName().length());
        p1.AppendString(ql[h].GetName().c_str());

        //From packets!! unknown meaning!!
        p1.Write<uint8_t>(0x01);
        p1.Write<uint32_t>(0);
    }*/

    pkt->Write<uint8_t>(0);	//End of quest list

    pkt->Write<uint32_t>(player->GetUniqueID());

    //Position Information
    Coord pos = player->get_position();

    pkt->Write<uint16_t>(pos.get_zone());
    pkt->Write(pos.get_x());
    pkt->Write(pos.get_z());
    pkt->Write(pos.get_y());
    pkt->Write<uint16_t>(player->get_angle());
    pkt->Write<uint8_t>(0);

    pkt->Write<uint8_t>(1);
    /*if (player->get_move_state() == STATE_RUNNING)
            Write<uint8_t>(1);
    else
            Write<uint8_t>(0);

    if (player->is_moving())
    {
    }
    else
    {
            Write<uint8_t>(0);
            Write<uint16_t>(player->get_angle());
    }*/

    pkt->Write<uint8_t>(0);
    pkt->Write<uint16_t>(player->get_angle());

    if (player->get_status_state() == STATUS_ACTION_DEAD)
        pkt->Write<uint16_t>(2);
    else
        pkt->Write<uint16_t>(0);

    //State Berserker
    pkt->Write<uint8_t>(player->IsBerserk());

    //Speed Information
    pkt->Write(player->walking_speed());
    pkt->Write(player->running_speed());
    pkt->Write(player->berseker_speed());
    pkt->Write<uint8_t>(0);

    pkt->Write(player->get_name());

    //Job Information

    Job job = player->get_job();

    pkt->Write(job.get_alias());
    pkt->Write<uint8_t>(job.get_type());
    pkt->Write<uint8_t>(job.get_level());
    pkt->Write<uint64_t>(job.get_experience());
    pkt->Write<uint16_t>(0);
    pkt->Write<uint32_t>(0);
    pkt->Write<uint8_t> (0);

    pkt->Write<uint8_t>(0xff);		//PK STATUS!! (02 enable)
    pkt->Write<uint64_t>(1);        //Tutorial flags
    pkt->Write<uint32_t>(accountID);
    pkt->Write<uint8_t>(player->is_gm());   //GM FLAG!!

    pkt->Write<uint8_t>(7);

    pkt->Write<uint8_t>(player->m_Hotkeys.size());
    std::for_each(player->m_Hotkeys.begin(),player->m_Hotkeys.end(),boost::bind(WriteHotkey,pkt,_1));

    pkt->Write<uint8_t>((player->m_Autopot.hp_bar << 4) | player->m_Autopot.hp_slot);
    pkt->Write<uint8_t>((player->m_Autopot.hp_active << 7) | player->m_Autopot.hp_value);
    pkt->Write<uint8_t>((player->m_Autopot.mp_bar << 4) | player->m_Autopot.mp_slot);
    pkt->Write<uint8_t>((player->m_Autopot.mp_active << 7) | player->m_Autopot.mp_value);
    pkt->Write<uint8_t>((player->m_Autopot.pill_bar << 4) | player->m_Autopot.pill_slot);
    pkt->Write<uint8_t>(player->m_Autopot.pill_active << 7);
    pkt->Write<uint8_t>((player->m_Autopot.delay_active << 7) | player->m_Autopot.delay_value);

    pkt->Write<uint8_t>(player->m_BlockList.size());
    for ( std::vector<std::string>::const_iterator i = player->m_BlockList.begin(); i != player->m_BlockList.end(); ++i)
        pkt->Write(*i);

    pkt->Write<uint16_t>(1);
    pkt->Write<uint16_t>(1);
    pkt->Write<uint16_t>(0);
}

void PlayerID (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Player> &player)
{
    pkt->WriteOpcode(SERVER_PLAYER_ID);
    pkt->Write<uint32_t>(player->GetUniqueID());
    pkt->Write<uint32_t>(rand());		//WORLD TIME
}

void PlayerStats (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Player> &player)
{
    pkt->WriteOpcode(SERVER_PLAYER_STATS);
    pkt->Write<uint32_t>(player->physical_attack().first);
    pkt->Write<uint32_t>(player->physical_attack().second);
    pkt->Write<uint32_t>(player->magical_attack().first);
    pkt->Write<uint32_t>(player->magical_attack().second);
    pkt->Write<uint16_t>(player->physical_defense());
    pkt->Write<uint16_t>(player->magical_defense());
    pkt->Write<uint16_t>(player->hit_ratio());
    pkt->Write<uint16_t>(player->parry_ratio());
    pkt->Write<uint32_t>(player->max_hp());
    pkt->Write<uint32_t>(player->max_mp());
    pkt->Write<uint16_t>(player->get_str());
    pkt->Write<uint16_t>(player->get_int());
}

void PlayerSP (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Player> &player)
{
    pkt->WriteOpcode(SERVER_PLAYER_EXTRA_STATS);
    pkt->Write<uint8_t>(PLAYER_EXTRA_STATS_SP);
    pkt->Write<uint32_t>(player->get_sp());
    pkt->Write<uint8_t>(0);
}

void PlayerGold (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Player> &player)
{
    pkt->WriteOpcode(SERVER_PLAYER_EXTRA_STATS);
    pkt->Write<uint8_t>(PLAYER_EXTRA_STATS_GOLD);
    pkt->Write<uint64_t>(player->get_gold());
    pkt->Write<uint8_t>(0);
}

void PlayerBerserkGauge (const boost::shared_ptr<OPacket> &pkt, const uint8_t count, const uint32_t objID)
{
    pkt->WriteOpcode(SERVER_PLAYER_EXTRA_STATS);
    pkt->Write<uint8_t>(PLAYER_EXTRA_STATS_BERSERK_GAUGE);
    pkt->Write<uint8_t>(count);
    pkt->Write<uint32_t>(objID);
}

void PlayerAction (const boost::shared_ptr<OPacket> &pkt, PLAYER_ACTION_TYPE type)
{
    pkt->WriteOpcode(SERVER_PLAYER_ACTION);
    pkt->Write<uint16_t>(type);

    if (type == PLAYER_ACTION_ERROR)
    {
        pkt->Write<uint8_t>(4);
        pkt->Write<uint8_t>(0x40);
    }
}

void PlayerPickup (const boost::shared_ptr<OPacket> &pkt, const uint32_t ID)
{
    pkt->WriteOpcode(SERVER_PLAYER_PICKUP);
    pkt->Write<uint32_t>(ID);
    pkt->Write<uint8_t>(0xE6);
}

void PlayerLevel (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID)
{
    pkt->WriteOpcode(SERVER_PLAYER_LEVELUP_ANIMATION);
    pkt->Write<uint32_t>(playerID);
}

void PlayerEmote (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Player> &player, uint8_t action)
{
    pkt->WriteOpcode(SERVER_PLAYER_EMOTE);
    pkt->Write<uint32_t>(player->GetUniqueID());
    pkt->Write<uint8_t>(action);
}

void PlayerHelp (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Player> &player, uint8_t action)
{
    pkt->WriteOpcode(SERVER_PLAYER_HELP_ICON);
    pkt->Write<uint32_t>(player->GetUniqueID());
    pkt->Write<uint8_t>(action);
}

void PlayerXPSP (const boost::shared_ptr<OPacket> &pkt, const uint32_t id, const uint64_t xp, const uint64_t spxp)
{
    pkt->WriteOpcode(SERVER_PLAYER_XP_SP);
    pkt->Write<uint32_t>(id);
    pkt->Write<uint64_t>(xp);
    pkt->Write<uint64_t>(spxp);
    pkt->Write<uint8_t>(0);
}


void PlayerXPSP (const boost::shared_ptr<OPacket> &pkt, const uint32_t ID, const uint64_t xp, const uint64_t spxp,
    const uint16_t stats)
{
    pkt->WriteOpcode(SERVER_PLAYER_XP_SP);
    pkt->Write<uint32_t>(ID);
    pkt->Write<uint64_t>(xp);
    pkt->Write<uint64_t>(spxp);
    pkt->Write<uint8_t>(0);
    pkt->Write<uint16_t>(stats);
}

void PlayerSilk (const boost::shared_ptr<OPacket> &pkt, const uint32_t amount)
{
    pkt->WriteOpcode(SERVER_PLAYER_SILK);
    pkt->Write<uint32_t>(amount);
    pkt->Write<uint64_t>(0);
}

void PlayerStatus (const boost::shared_ptr<OPacket> &pkt, const uint32_t EncStatus)
{
    pkt->WriteOpcode(SERVER_PLAYER_STATUS);
    pkt->Write<uint32_t>(EncStatus);
}

void PlayerStatus (const boost::shared_ptr<OPacket> &pkt, const uint32_t EncStatus, const uint32_t Duration,
    const uint16_t Level)
{
    pkt->WriteOpcode(SERVER_PLAYER_STATUS);
    pkt->Write<uint32_t>(EncStatus);
    pkt->Write<uint32_t>(Duration/10);
    pkt->Write<uint16_t>(0);            /// ELAPSED TIME (IS USED IF THIS PACKET SENT AGAIN WITH A NEW STATUS)
    pkt->Write<uint16_t>(Level);
}

void PlayerDeath (const boost::shared_ptr<OPacket> &pkt, const PLAYER_DEATH_TYPE type)
{
    pkt->WriteOpcode(SERVER_PLAYER_DEATH);
    pkt->Write<uint8_t>(type);
}

void IncInt (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_INC_INT,ANSWER_ACCEPT);
}

void IncStr (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_INC_STR,ANSWER_ACCEPT);
}

void InventorySlots (const boost::shared_ptr<OPacket> &pkt, const uint8_t size)
{
    pkt->WriteOpcode(SERVER_PLAYER_INVENTORY_SLOTS,ANSWER_ACCEPT);
    pkt->Write<uint8_t>(size);
}

}
