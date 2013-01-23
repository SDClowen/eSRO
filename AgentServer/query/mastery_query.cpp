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

#include "mastery_query.hpp"

namespace DB {
namespace MASTERY {

void Create::operator() (mysqlpp::Connection &conn, const uint32_t ID, const Mastery& mastery)
{
    using mysqlpp::quote;

    mysqlpp::Query query = conn.query();
    query << "insert into masteries values ("
            << ID << "," << mastery.ID() << "," << quote << mastery.Level() << ")";

    query.exec();
}

void Update::operator() (mysqlpp::Connection &conn, const uint32_t ID, const Mastery& mastery)
{
    mysqlpp::Query query = conn.query();
    query << "update masteries set "
            << "Level = " << mysqlpp::quote << mastery.Level() << " "
            << "where CharID =" << ID << " "
            << "and ID = " << (mysqlpp::sql_int)mastery.ID();

    query.exec();
}

MasteryTree Select::operator() (mysqlpp::Connection &conn, const uint32_t ID, const uint8_t Level,
    const RACE race)
{
    using mysqlpp::quote;

    mysqlpp::Query query = conn.query();

    query << "select * from masteries where CharID = " << ID;

    mysqlpp::StoreQueryResult res = query.store();

    MasteryTree tree(race,Level);
    for (size_t j = 0; j < res.num_rows(); ++j)
            tree.insert(Mastery(res[j]["ID"],res[j]["Level"]));

    return tree;
}

void Delete::operator() (mysqlpp::Connection &conn, const uint32_t ID)
{
    mysqlpp::Query query = conn.query();

    query << "delete from masteries where CharID =" << ID;

    query.exec();
}

}   }
