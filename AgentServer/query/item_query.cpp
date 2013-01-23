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

#include "item_query.hpp"

namespace DB {
namespace ITEM {

uint32_t Create::operator() (mysqlpp::Connection& conn, const uint32_t ID, const Item& item)
{
    if (!item.get_id())
        return 0;

    using mysqlpp::quote;

    mysqlpp::Query query = conn.query();
    query << "insert into items (charID,refItemID, EnchantLevel, Quantity, Location, "
        << "Position, Durability, Phy, Mag, Dur, AP, CB, PhyReinf, MagReinf) values ("
            << quote << ID << ","
            << quote << (mysqlpp::sql_int)item.get_id() << ","
            << quote << item.get_enchant_lv() << ","
            << quote << (mysqlpp::sql_int)item.get_quantity() << ","
            << quote << item.get_place() << ","
            << quote << item.get_position() << ","
            << quote << (mysqlpp::sql_int)item.get_durability() << ","
            << quote << item.get_physical_percent() << ","
            << quote << item.get_magical_percent() << ","
            << quote << item.get_durability_percent() << ","
            << quote << item.get_ap_ratio_percent() << ","
            << quote << item.get_cb_ratio_percent() << ","
            << quote << item.get_physical_reinf_percent() << ","
            << quote << item.get_magical_reinf_percent() << ")";

    query.exec();

    uint32_t itemID = query.insert_id();

    std::vector<Item::attribute> attr = item.get_attr_list();

    for ( size_t i = 0; i < attr.size(); ++i)
    {
        query.reset();

        query << "insert into items_magicoptions values ("
            << itemID << "," << attr[i].first << "," << attr[i].second << ")";

        query.exec();
    }

    return itemID;
}

std::vector<Item::extended_type> Select::operator () (mysqlpp::Connection& conn, const uint32_t CharID,
    const uint8_t location)
{
    mysqlpp::Query query = conn.query();

    query << "select * from items where CharID =" << CharID << " and Location = " << mysqlpp::quote << location;

    mysqlpp::StoreQueryResult res = query.store();

    Item::extended_type attr;
    std::vector<Item::extended_type> v;


    for (size_t j=0; j < res.num_rows(); ++j)
    {
        attr.dbid = res[j]["ID"];
            attr.id = res[j]["refItemID"];
            attr.enchant_lv = res[j]["EnchantLevel"];
            attr.quantity = res[j]["Quantity"];
            attr.pos = res[j]["Position"];
            attr.place = res[j]["Location"];
            attr.durability = res[j]["Durability"];
            attr.phy_percent = res[j]["Phy"];
            attr.mag_percent = res[j]["Mag"];
            attr.dur_percent = res[j]["Dur"];
            attr.ap_percent = res[j]["AP"];
            attr.cb_percent = res[j]["CB"];
            attr.phyreinf_percent = res[j]["PhyReinf"];
            attr.magreinf_percent = res[j]["MagReinf"];

    query.reset();

    query << "select * from items_magicoptions where itemID = " << attr.dbid;

    mysqlpp::StoreQueryResult ires = query.store();

    for (size_t i = 0; i < ires.num_rows(); ++i)
        attr.attr_list.push_back(std::make_pair(ires[i]["optionID"],ires[i]["Value"]));

            v.push_back(attr);

            attr.attr_list.clear();
    }

    return v;
}

void Position::operator() (mysqlpp::Connection &conn, const uint32_t itemID, const uint8_t pos)
{
    mysqlpp::Query query = conn.query();

    query << "update items set Position = " << mysqlpp::quote << pos << " where ID = " << itemID;

    query.exec();
}

void EnchantLv::operator() (mysqlpp::Connection& conn, const uint32_t itemID, const uint8_t level)
{
    mysqlpp::Query query = conn.query();

    query << "update items set EnchantLevel = " << mysqlpp::quote << level << " where ID = " << itemID;

    query.exec();
}

void Quantity::operator() (mysqlpp::Connection& conn, const uint32_t itemID, const uint16_t quantity)
{
    mysqlpp::Query query = conn.query();

    query << "update items set Quantity = " << quantity << " where ID =" << itemID;

    query.exec();
}

void Durability::operator() (mysqlpp::Connection& conn, const uint32_t itemID, const uint32_t durability)
{
    mysqlpp::Query query = conn.query();

    query << "update items set Durability ="  << durability << " where ID = " << itemID;

    query.exec();
}

void Percent::operator() (mysqlpp::Connection& conn, const std::string& name,
		const Item& item)
{
    using mysqlpp::quote;

    mysqlpp::Query query = conn.query();
    query << "update items set "
            << "Phy=" << quote << item.get_physical_percent() << ","
            << "Mag=" << quote << item.get_magical_percent() << ","
            << "Dur=" << quote << item.get_durability_percent() << ","
            << "AP=" << quote << item.get_ap_ratio_percent() << ","
            << "CB=" << quote << item.get_cb_ratio_percent() << ","
            << "PhyReinf=" << quote << item.get_physical_reinf_percent() << ","
            << "MagReinf=" << quote << item.get_magical_reinf_percent() << " "
            << "where name=" << quote << name << " "
            << "and Position =" << quote << item.get_position() << " "
            << "and Location =" << quote << item.get_place();

    query.exec();
}

void Delete::operator() (mysqlpp::Connection& conn, const uint32_t itemID)
{
    mysqlpp::Query query = conn.query();

    query << "delete from items where ID =" << itemID;

    query.exec();

    query.reset();

    query << "delete from items_magicoptions where itemID = " << itemID;

    query.exec();
}

uint32_t SkillID::operator() (mysqlpp::Connection &conn, const uint32_t ID)
{
    mysqlpp::Query query = conn.query();

    query << "select arg1 from refitemdata where id = " << ID;

    mysqlpp::StoreQueryResult res = query.store();

    if (res.num_rows())
    {
            query.reset();

            query << "select id from refskilldata where name = " << mysqlpp::quote << res[0]["arg1"];

            mysqlpp::StoreQueryResult res = query.store();

            if (res.num_rows())
                    return res[0]["id"];
    }

    return 0;
}

uint32_t TransportID::operator() (mysqlpp::Connection &conn, const std::string &name)
{
    mysqlpp::Query query = conn.query();

    query << "select ID from refcharacterdata where NameID = " << mysqlpp::quote << name;

    mysqlpp::StoreQueryResult res = query.store();

    if (res.num_rows())
        return res[0]["ID"];

    return 0;
}

uint32_t ID::operator() (mysqlpp::Connection &conn, const std::string &name)
{
    mysqlpp::Query query = conn.query();

    query << "select id from refitemdata where name = " << mysqlpp::quote << name;

    mysqlpp::StoreQueryResult res = query.store();

    if (res.num_rows())
            return res[0]["id"];

    return 0;
}

void MagicOption::operator() (mysqlpp::Connection &conn, const uint32_t itemID, const uint32_t optionID,
        const uint32_t value)
{
    mysqlpp::Query query = conn.query();

    query << "insert into items_magicoptions values (" << itemID << "," << optionID << "," << value << ")";

    query.exec();
}

}   }
