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

#include "shop_query.hpp"

namespace DB {
namespace SHOP {

uint32_t ItemID::operator() (mysqlpp::Connection& conn, const std::string& npc,
						  uint8_t page, uint8_t pos)
{
    mysqlpp::Query query = conn.query();

    query << "select ShopGroup from refshopgroup where NPC = " << mysqlpp::quote << npc;

    mysqlpp::StoreQueryResult gres = query.store();

    std::string group;

    if (gres.num_rows())
        group = (std::string)gres[0]["ShopGroup"];
    else
        return 0;

    query.reset();

    query << "select Shop from refmappingshopgroup where ShopGroup = " << mysqlpp::quote << group;

    mysqlpp::StoreQueryResult sres = query.store();

    if (!sres.num_rows())
        return 0;

    std::string store = (std::string)sres[0]["Shop"];

    query.reset();

    query << "select GroupTab from refmappingshopwithtab where Shop = " << mysqlpp::quote << store;

    mysqlpp::StoreQueryResult nres = query.store();

    if (!nres.num_rows())
        return 0;

    std::string group_num;

    if (store.find("STABLE",0) != std::string::npos)
        group_num = (std::string)nres[0]["GroupTab"];
    else
        group_num = (std::string)nres[(int)(page/3)]["GroupTab"];

    query.reset();

    query << "select Tab from refshoptab where GroupTab = " << mysqlpp::quote << group_num;

    mysqlpp::StoreQueryResult tres = query.store();

    if (!tres.num_rows())
        return 0;

    std::string tab;

    if (store.find("STABLE",0) != std::string::npos)
        tab = (std::string)tres[page%4]["Tab"];
    else
        tab = (std::string)tres[page%3]["Tab"];

    query.reset();

    query << "select PKItem from refshopgoods where Tab = " << mysqlpp::quote << tab
        << " and Position = " << mysqlpp::quote << pos;

    mysqlpp::StoreQueryResult pkres = query.store();

    if (!pkres.num_rows())
        return 0;

    std::string pkitem = (std::string)pkres[0]["PKItem"];

    query.reset();

    query << "select refItemName from refscrapofpackageitem where PKItem = " << mysqlpp::quote << pkitem;

    mysqlpp::StoreQueryResult res = query.store();

    if (!res.num_rows())
        return 0;

    std::string iname = (std::string)res[0]["refItemName"];

    query.reset();

    query << "select refID from refitemdata where ItemName = " << mysqlpp::quote << iname;

    res = query.store();

    if (!res.num_rows())
        return 0;

    return (mysqlpp::sql_int)res[0]["refID"];
}

uint32_t ItemID::operator() (mysqlpp::Connection& conn, const std::string &pkName, const uint32_t pkID)
{
	mysqlpp::Query query = conn.query();

    query << "select refItemName from refscrapofpackageitem where PKItem = " << mysqlpp::quote << pkName;

    mysqlpp::StoreQueryResult res = query.store();

    if (!res.num_rows())
        return 0;

    std::string iname = (std::string)res[0]["refItemName"];

    query.reset();

    query << "select refID from refitemdata where ItemName = " << mysqlpp::quote << iname;

    res = query.store();

    if (!res.num_rows())
        return 0;

    return (mysqlpp::sql_int)res[0]["refID"];
}

std::vector<Price::price_type> Price::operator() (mysqlpp::Connection &conn, const uint32_t itemID)
{
    std::vector<Price::price_type> v;

	mysqlpp::Query query = conn.query();

    query << "select ItemName from refitemdata where refID = " << itemID;

    mysqlpp::StoreQueryResult res = query.store();

    if (!res.num_rows())
        return v;

    std::string name = (std::string)res[0]["ItemName"];

    query.reset();

    query << "select PKItem from refscrapofpackageitem where refItemName = " << mysqlpp::quote << name;

    res = query.store();

    if (!res.num_rows())
        return v;

	std::string pkitem = static_cast<std::string>(res[0]["PKItem"]);

	query.reset();

	query << "select * from refpricepolicyofitem where PKItem = " << mysqlpp::quote << pkitem;

	res = query.store();

	for (size_t i = 0; i < res.num_rows(); ++i)
		v.push_back(std::make_pair(res[i]["Type"],res[i]["Price"]));

	return v;
}

boost::unordered_map<uint32_t,uint16_t> Stacks::operator() (mysqlpp::Connection &conn)
{
    mysqlpp::Query query = conn.query();

    query << "select refitemdata.refID, refscrapofpackageitem.Quantity from refitemdata, refpackageitem,"
        << "refscrapofpackageitem where refpackageitem.PKItem = refscrapofpackageitem.PKItem and "
        << "refscrapofpackageitem.refItemName = refitemdata.ItemName";

    mysqlpp::StoreQueryResult res = query.store();

    boost::unordered_map<uint32_t,uint16_t> v;
    for (size_t i = 0; i < res.num_rows(); ++i)
        v.insert(std::make_pair(res[i]["refID"],res[i]["Quantity"]));

    return v;
}

}   }
