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

#ifndef DROP_QUERY_HPP
#define DROP_QUERY_HPP

#include "drop.hpp"

#include <mysql++/mysql++.h>
#include <boost/unordered_map.hpp>

namespace DB {
namespace DROP {

class Info
{
public:

    boost::unordered_multimap<uint32_t,Drop> operator() (mysqlpp::Connection &conn);
};

class Weight
{
public:

    boost::unordered_multimap<uint32_t,DropWeight> operator() (mysqlpp::Connection &conn);
};

}   }
#endif // DROP_QUERY_HPP
