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

#include "block_query.hpp"

namespace DB {
namespace BLOCK {

bool Valid::operator() (mysqlpp::Connection &conn, const std::string &name)
{
    mysqlpp::Query query = conn.query();

    query << "select * from characters where Name = " << mysqlpp::quote << name;

    mysqlpp::StoreQueryResult res = query.store();

    return res.num_rows() > 0;
}

bool Check::operator() (mysqlpp::Connection &conn, const uint32_t CharID, const std::string &name)
{
    mysqlpp::Query query = conn.query();

    query << "select * from block_chat where CharID = " << CharID
            << " and CharName = " << mysqlpp::quote << name;

    mysqlpp::StoreQueryResult res = query.store();

    return res.num_rows() > 0;
}

void Insert::operator() (mysqlpp::Connection &conn, const uint32_t CharID, const std::string &name)
{
    mysqlpp::Query query = conn.query();

    query << "insert into block_chat values ("
            << mysqlpp::quote << CharID << "," << mysqlpp::quote << name << ")";

    query.exec();
}

void Remove::operator() (mysqlpp::Connection &conn, const uint32_t CharID, const std::string &name)
{
    mysqlpp::Query query = conn.query();

    query << "delete from block_chat where CharID = " << CharID
            << " and CharName = " << mysqlpp::quote << name;

    query.exec();
}

std::vector<std::string> Select::operator() (mysqlpp::Connection &conn, const uint32_t CharID)
{
    mysqlpp::Query query = conn.query();

    query << "select * from block_chat where CharID = " << CharID;

    mysqlpp::StoreQueryResult res = query.store();

    std::vector<std::string> v;
    for (size_t i = 0; i < res.num_rows(); ++i)
        v.push_back(static_cast<std::string>(res[i]["CharName"]));

    return v;
}

}   }
