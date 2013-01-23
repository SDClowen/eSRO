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

#ifndef __SRDL_ALLIANCE_QUERY_HPP__
#define __SRDL_ALLIANCE_QUERY_HPP__

#include <stdint.h>
#include <mysql++/mysql++.h>

namespace DB {
namespace ALLIANCE {

class Create
{
public:

    uint32_t operator() (mysqlpp::Connection &conn, const uint32_t guildID);
};

class Select
{
public:

    std::pair<uint32_t,uint32_t> operator() (mysqlpp::Connection &conn, const uint32_t unionID);
};

class List
{
public:

    std::vector<uint32_t> operator() (mysqlpp::Connection &conn, const uint32_t unionID);
};

class Delete
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t unionID);
};

}   }

#endif
