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

#include "skill_query.hpp"

namespace DB {
namespace SKILL {

void Create::operator() (mysqlpp::Connection& conn, const uint32_t CharID, const uint32_t SkillID)
{
    using mysqlpp::quote;

    mysqlpp::Query query = conn.query();
    query << "insert into skills values ("
            << quote << CharID << "," << quote << (mysqlpp::sql_int)SkillID << ")";

    query.exec();
}

std::set<uint32_t> Select::operator() (mysqlpp::Connection& conn, const uint32_t CharID)
{
    using mysqlpp::quote;

    mysqlpp::Query query = conn.query();

    query << "select * from skills where CharID =" << quote << CharID;

    mysqlpp::StoreQueryResult res = query.store();

    std::set<uint32_t> list;

    for (size_t j=0; j<res.num_rows(); ++j)
            list.insert(res[j]["ID"]);

    return list;
}

void Delete::operator() (mysqlpp::Connection& conn, const uint32_t CharID)
{
    mysqlpp::Query query = conn.query();

    query << "delete from skills where CharID =" << mysqlpp::quote << CharID;

    query.exec();
}

void Delete::operator() (mysqlpp::Connection& conn, const uint32_t CharID, const uint32_t SkillID)
{
    mysqlpp::Query query = conn.query();

    query << "delete from skills where CharID = " << mysqlpp::quote << CharID
            << " and ID = " << SkillID;

    query.exec();
}

}   }
