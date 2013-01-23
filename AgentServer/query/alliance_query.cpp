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

#include "alliance_query.hpp"

using namespace DB::ALLIANCE;

uint32_t Create::operator() (mysqlpp::Connection &conn, const uint32_t guildID)
{
    mysqlpp::Query query = conn.query();

    query << "insert into guild_alliance (headGuildID) values (" << guildID << ")";

    query.exec();

    return query.insert_id();
}

std::pair<uint32_t,uint32_t> Select::operator() (mysqlpp::Connection &conn, const uint32_t unionID)
{
    mysqlpp::Query query = conn.query();

    query << "select * from guild_alliance where ID = " << unionID;

    mysqlpp::StoreQueryResult res = query.store();

    std::pair<uint32_t,uint32_t> data;

    if (res.num_rows())
        data = std::make_pair(res[0]["EmblemIndex"],res[0]["HeadGuildID"]);

    return data;
}

std::vector<uint32_t> List::operator() (mysqlpp::Connection &conn, const uint32_t unionID)
{
    mysqlpp::Query query = conn.query();

    query << "select ID from guild where UnionID = " << unionID;

    mysqlpp::StoreQueryResult res = query.store();
    std::vector<uint32_t> list;

    for (size_t i = 0; i < res.num_rows(); ++i)
        list.push_back(res[i]["ID"]);

    return list;
}

void Delete::operator() (mysqlpp::Connection &conn, const uint32_t unionID)
{
    mysqlpp::Query query = conn.query();

    query << "delete from guild_alliance where ID = " << unionID;

    query.exec();
}
