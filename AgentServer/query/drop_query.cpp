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

#include "drop_query.hpp"

namespace DB {
namespace DROP {

boost::unordered_multimap<uint32_t,Drop> Info::operator() (mysqlpp::Connection &conn)
{
    boost::unordered_multimap<uint32_t,Drop> ret;

    mysqlpp::Query query = conn.query();

    query << "select * from refcharacter_drop";

    mysqlpp::StoreQueryResult res = query.store();

    Drop drop;
    for (size_t i = 0; i < res.num_rows(); ++i)
    {
        drop.refCharID = res[i]["refCharID"];
        drop.refItemID = res[i]["refItemID"];
        drop.Weight = static_cast<float>(res[i]["Weight"]);
        drop.Quality = static_cast<int>(res[i]["Quality"]);

        ret.insert(std::make_pair(drop.refCharID,drop));
    }

    return ret;
}

boost::unordered_multimap<uint32_t,DropWeight> Weight::operator() (mysqlpp::Connection &conn)
{
    boost::unordered_multimap<uint32_t,DropWeight> ret;

    mysqlpp::Query query = conn.query();

    query << "select * from refcharacter_drop_quality";

    mysqlpp::StoreQueryResult res = query.store();

    DropWeight dq;
    for (size_t i = 0; i < res.num_rows(); ++i)
    {
        dq.refCharID = res[i]["refCharID"];
        dq.Weight = res[i]["Weight"];
        dq.Quality = static_cast<int>(res[i]["Quality"]);

        ret.insert(std::make_pair(dq.refCharID,dq));
    }

    return ret;
}

}   }
