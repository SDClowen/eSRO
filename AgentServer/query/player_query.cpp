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

#include "player_query.hpp"
#include "mastery_query.hpp"
#include "skill_query.hpp"
#include "item_query.hpp"

/// SRLF INCLUDES
#include <player.hpp>
#include <storage.hpp>

namespace DB {
namespace PLAYER {

bool Check::operator() (mysqlpp::Connection &conn, const std::string& name)
{
        mysqlpp::Query query = conn.query();
	query << "select Name from characters where Name = " << mysqlpp::quote << name;

	mysqlpp::StoreQueryResult res = query.store();

	return !res.num_rows();
}

std::vector<PlayerProperty> Select::operator() (mysqlpp::Connection &conn, const uint32_t ID)
{
    mysqlpp::Query query = conn.query();

    query << "delete from characters where AccountID = " << ID << " and ExpirationDate >= now()";

    query.exec();

    query.reset();

    query << "select * from characters where AccountID = " << ID;

    mysqlpp::StoreQueryResult res = query.store();

    PlayerProperty base;
    std::vector<PlayerProperty> v;

    for (size_t i = 0; i < res.num_rows(); ++i )
    {
        base.ID = res[i]["ID"];
        base.RefCharID = res[i]["refCharID"];
        base.Name = std::string(res[i]["Name"]);
        base.Volume = res[i]["Volume"];
        base.Level = res[i]["Level"];
        base.MaxLevel = res[i]["MaxLevel"];
        base.Str = res[i]["Strength"];
        base.Int = res[i]["Intelligence"];
        base.HP = res[i]["HP"];
        base.MP = res[i]["MP"];
        base.Stats = res[i]["Stats"];
        base.SP = res[i]["SP"];
        base.Exp = res[i]["Exp"];
        base.SPExp = res[i]["SPExp"];
        base.Gold = res[i]["Gold"];
        base.GoldStored = res[i]["GoldStored"];
        base.BskCount = res[i]["BerserkCount"];
        base.PK = res[i]["PK"];
        base.PKLevel = res[i]["PKLvl"];
        base.Zone = res[i]["Zone"];
        base.X = res[i]["X"];
        base.Y = res[i]["Y"];
        base.Z = res[i]["Z"];
        base.Angle = res[i]["Angle"];
        base.Rank = res[i]["Rank"];
        base.InvSlots = res[i]["InventorySlots"];
        base.InvSlotsIncreased = res[i]["InventorySlotsInc"];
        base.StoreSlots = res[i]["StorageSlots"];
        base.StorageSlotsIncreased = res[i]["StorageSlotsInc"];
        base.GuildID = res[i]["GuildID"];
        base.AcademyID = res[i]["AcademyID"];
        base.ReturnID = res[i]["ReturnID"];
        base.StallAvatarID = res[i]["StallAvatarID"];
        base.AbilityPetID = res[i]["AbilityPetID"];
        base.AttackPetID = res[i]["AttackPetID"];
        base.TransportID = res[i]["TransportID"];
        base.AutopotHPActive = res[i]["AutopotHPActive"];
        base.AutopotHPValue = res[i]["AutopotHPValue"];
        base.AutopotHPBar = res[i]["AutopotHPBar"];
        base.AutopotHPSlot = res[i]["AutopotHPSlot"];
        base.AutopotMPActive = res[i]["AutopotMPActive"];
        base.AutopotMPValue = res[i]["AutopotMPValue"];
        base.AutopotMPBar = res[i]["AutopotMPBar"];
        base.AutopotMPSlot = res[i]["AutopotMPSlot"];
        base.AutopotPillActive = res[i]["AutopotPillActive"];
        base.AutopotPillBar = res[i]["AutopotPillBar"];
        base.AutopotPillSlot = res[i]["AutopotPillSlot"];
        base.AutopotDelay = res[i]["AutopotDelay"];
        base.ExpirationMark = res[i]["ExpirationMark"];

        time_t actual_time;
        time(&actual_time);
        mysqlpp::DateTime dt = res[i]["ExpirationDate"];

        if (actual_time > dt)
        {
            base.ExpirationTime = static_cast<uint32_t>(difftime(actual_time,dt)/60);

            v.push_back(base);
        }
    }

    return v;
}

void Create::operator() (mysqlpp::Connection &conn, const uint32_t AccountID, const boost::shared_ptr<Player> &player,
    const std::string &name)
{
    using mysqlpp::quote;
    using mysqlpp::sql_int;

    mysqlpp::Query query = conn.query();

    Coord pos = player->get_position();

    query << "insert into characters (AccountID,refCharID,Name,Volume,Zone,X,Y,Z,Angle,GM,ReturnID,CreationDate)"
        << " values (" << AccountID << "," << player->get_model() << "," << quote << name << ","
        << quote << player->get_volume() << "," << pos.get_zone() << "," << pos.get_x() << ","
        << pos.get_y() << "," << pos.get_z() << "," << player->get_angle() << "," << player->is_gm() << ","
        << player->get_return_point() << ", now())";

    query.exec();

    player->ID(query.insert_id());

    //Save Character Items
    DB::ITEM::Create iobj;
    boost::shared_ptr<Storage> store = player->get_storage(STORAGE_INVENTORY);

    for (Storage::iterator i = store->begin(); i != store->end(); ++i)
        i->second.ID(iobj(conn,player->ID(),i->second));

    MasteryTree tree = player->get_mastery_tree();

    //Save Mastery Information
    DB::MASTERY::Create mobj;
    MasteryTree::const_iterator mi;

    for (mi = tree.begin(); mi != tree.end(); ++mi)
        mobj(conn,player->ID(),mi->second);
}

void Delete::operator() (mysqlpp::Connection &conn, const uint32_t ID)
{
    mysqlpp::Query query = conn.query();

    query << "delete from characters where CharID = " << ID;
    query.exec();

    DB::ITEM::Delete iobj;
    iobj(conn,ID);

    DB::MASTERY::Delete mobj;
    mobj(conn,ID);

    DB::SKILL::Delete sobj;
    sobj(conn,ID);
}

void Position::operator() (mysqlpp::Connection& conn, const uint32_t CharID, const Coord &pos)
{
    using mysqlpp::quote;

    mysqlpp::Query query = conn.query();

    query << "update characters set "
            << "Zone=" << quote << pos.get_zone() << ","
            << "X=" << quote << pos.get_x() << ","
            << "Y=" << quote << pos.get_y() << ","
            << "Z=" << quote << pos.get_z() << " "
            << "where ID =" << CharID;

    query.exec();
}

void Angle::operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint16_t angle)
{
	mysqlpp::Query query = conn.query();

	query << "update characters set Angle = " << angle << " where ID = " << CharID;

	query.exec();
}

void SP::operator() (mysqlpp::Connection& conn, const uint32_t CharID, const uint32_t points)
{
	using mysqlpp::quote;
	using mysqlpp::sql_int;

	mysqlpp::Query query = conn.query();

	query << "update characters set SP =" << (sql_int)points << " where ID =" << CharID;

	query.exec();
}

void Exp::operator() (mysqlpp::Connection& conn, const boost::shared_ptr<Player> player)
{
    mysqlpp::Query query = conn.query();

    query << "update characters set Exp = " << player->get_exp()
        << " where ID = " << mysqlpp::quote << player->ID();

    query.exec();
}

void Stats::operator() (mysqlpp::Connection& conn, const boost::shared_ptr<Player> player)
{
	mysqlpp::Query query = conn.query();

	query << "update characters set "
		<< "Strength=" << mysqlpp::quote << player->get_str() << " , "
		<< "Intelligence=" << mysqlpp::quote << player->get_int() << " , "
		<< "Stats=" << mysqlpp::quote << player->get_stat() << " "
		<< "where ID = " << player->ID();

	query.exec();
}

void Lvl::operator() (mysqlpp::Connection& conn, const boost::shared_ptr<Player> player)
{
	mysqlpp::Query query = conn.query();

	query << "update characters set "
		<< "Level = " << mysqlpp::quote << player->level() << " , "
		<< "MaxLevel = " << mysqlpp::quote << player->get_maxlv() << " "
		<< "where ID = " << player->ID();

	query.exec();
}

void Gold::operator() (mysqlpp::Connection& conn, const boost::shared_ptr<Player> player)
{
	mysqlpp::Query query = conn.query();

	query << "update characters set "
		<< "Gold = " << player->get_gold() << " "
		<< "where ID = " << player->ID();

	query.exec();
}

void GoldStored::operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint64_t gold)
{
    mysqlpp::Query query = conn.query();

    query << "update characters set GoldStored = " << gold << " where ID = " << CharID;

    query.exec();
}

void ReturnPoint::operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint32_t teleport_id)
{
    mysqlpp::Query query = conn.query();

    query << "update characters set ReturnID = " << teleport_id << " where ID = " << CharID;

    query.exec();
}

void HPMP::operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint32_t HP, const uint32_t MP)
{
    mysqlpp::Query query = conn.query();

    query << "update characters set HP = " << HP << ", MP = " << MP
            << " where ID = " << CharID;

    query.exec();
}

void Bersek::operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint8_t count)
{
	mysqlpp::Query query = conn.query();

	query << "update characters set BerserkCount = " << mysqlpp::quote << count
		<< " where ID = " << CharID;

	query.exec();
}

void Erase::operator() (mysqlpp::Connection &conn, const uint32_t CharID)
{
    mysqlpp::Query query = conn.query();

    query << "update characters set ExpirationMark = 1, ExpirationDate = now() + interval 7 day "
        << "where ID = " << CharID;

    query.exec();
}

void Restore::operator() (mysqlpp::Connection &conn, const uint32_t CharID)
{
    mysqlpp::Query query = conn.query();

    query << "update characters set ExpirationMark = 0, ExpirationDate = '0000-00-00 00:00:00' "
        << "where ID = " << CharID;

    query.exec();
}

void State::operator () (mysqlpp::Connection &conn, const uint32_t CharID, bool state)
{
    mysqlpp::Query query = conn.query();

    query << "update characters set State = " << state << " where ID = " << CharID;

    query.exec();
}

void GuildID::operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint32_t guildID)
{
    mysqlpp::Query query = conn.query();

    query << "update characters set GuildID = " << guildID << " where ID = " << CharID;

    query.exec();
}

void StallAvatar::operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint32_t avatarID)
{
    mysqlpp::Query query = conn.query();

    query << "update characters set StallAvatarID = " << avatarID << " where ID = " << CharID;

    query.exec();
}

AutopotSetting Autopot::operator() (mysqlpp::Connection &conn, const uint32_t CharID)
{
    mysqlpp::Query query = conn.query();

    query << "select AutopotHPActive,AutopotHPValue,AutopotHPBar,AutopotHPSlot,"
        << "AutopotMPActive,AutopotMPValue,AutopotMPSlot,AutopotMPBar,"
        << "AutopotPillActive,AutopotPillBar,AutopotPillSlot,AutopotDelayActive,AutopotDelay from characters "
        << "where ID = " << CharID;

    AutopotSetting settings;
    mysqlpp::StoreQueryResult res = query.store();

    if (res.num_rows())
    {
        settings.hp_active = res[0]["AutopotHPActive"];
        settings.hp_bar = res[0]["AutopotHPBar"];
        settings.hp_slot = res[0]["AutopotHPSlot"];
        settings.hp_value = res[0]["AutopotHPValue"];
        settings.mp_active = res[0]["AutopotMPActive"];
        settings.mp_bar = res[0]["AutopotMPBar"];
        settings.mp_slot = res[0]["AutopotMPSlot"];
        settings.mp_value = res[0]["AutopotMPValue"];
        settings.pill_active = res[0]["AutopotPillActive"];
        settings.pill_bar = res[0]["AutopotPillBar"];
        settings.pill_slot = res[0]["AutopotPillSlot"];
        settings.delay_active = res[0]["AutopotDelayActive"];
        settings.delay_value = res[0]["AutopotDelay"];
    }

    return settings;
}

void Autopot::operator() (mysqlpp::Connection &conn, const uint32_t CharID, const AutopotSetting &settings)
{
    using mysqlpp::quote;

    mysqlpp::Query query = conn.query();

    query << "update characters set AutopotHPActive = " << quote << settings.hp_active << ","
        << "AutopotHPValue = " << quote << settings.hp_value << ","
        << "AutopotHPBar = " << quote << settings.hp_bar << ","
        << "AutopotHPSlot = " << quote << settings.hp_slot << ","
        << "AutopotMPActive = " << quote << settings.mp_active << ","
        << "AutopotMPValue = " << quote << settings.mp_value << ","
        << "AutopotMPBar = " << quote << settings.mp_bar << ","
        << "AutopotMPSlot = " << quote << settings.mp_slot << ","
        << "AutopotPillActive = " << quote << settings.pill_active << ","
        << "AutopotPillBar = " << quote << settings.pill_bar << ","
        << "AutopotPillSlot = " << quote << settings.pill_slot << ","
        << "AutopotDelayActive = " << quote << settings.delay_active << ","
        << "AutopotDelay = " << quote << settings.delay_value
        << " where ID = " << CharID;

    query.exec();
}

void InventoryCapacity::operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint8_t size)
{
    mysqlpp::Query query = conn.query();

    query << "update characters set InventorySlotsInc = TRUE, InventorySlots = " << mysqlpp::quote << size
        << " where ID = " << CharID;

    query.exec();
}

void StorageCapacity::operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint8_t size)
{
    mysqlpp::Query query = conn.query();

    query << "update characters set StorageSlotsInc = TRUE, StorageSlots = " << mysqlpp::quote << size
        << " where ID = " << CharID;

    query.exec();
}

void Skin::operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint32_t refCharID,
    const uint8_t vol)
{
    mysqlpp::Query query = conn.query();

    query << "update characters set refCharID = " << refCharID << ","
        << "Volume = " << mysqlpp::quote << vol << " where ID = " << CharID;

    query.exec();
}

void Buffs::operator() (mysqlpp::Connection &conn, const uint32_t charID, const uint32_t refSkillID,
    const uint32_t time)
{
    mysqlpp::Query query = conn.query();

    query << "insert into character_buff values (" << charID << "," << refSkillID << "," << time << ")";

    query.exec();
}

std::vector<std::pair<uint32_t,uint32_t> > Buffs::operator() (mysqlpp::Connection &conn, const uint32_t charID)
{
    std::vector<std::pair<uint32_t,uint32_t> > v;

    mysqlpp::Query query = conn.query();

    query << "select * from character_buff where charID = " << charID;

    mysqlpp::StoreQueryResult res = query.store();

    for (size_t i = 0; i < res.num_rows(); ++i)
        v.push_back(std::make_pair(res[i]["refSkillID"],res[i]["Time"]));

    return v;
}

void RemoveBuffs::operator() (mysqlpp::Connection &conn, const uint32_t charID)
{
    mysqlpp::Query query = conn.query();

    query << "delete from character_buff where charID = " << charID;

    query.exec();
}

}   }
