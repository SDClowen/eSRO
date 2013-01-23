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

#ifndef __SRDL_SHOP_QUERY_HPP__
#define __SRDL_DATABASE_SHOP_QUERY_HPP__

#include "connection.hpp"

#include <types.hpp>
#include <boost/unordered_map.hpp>

namespace DB {
namespace SHOP {

class ItemID
{
public:

    uint32_t operator() (mysqlpp::Connection& conn, const std::string &npc, uint8_t page, uint8_t pos);

    uint32_t operator() (mysqlpp::Connection& conn, const std::string &pkName, const uint32_t pkID);
};

class Price
{
public:

	typedef std::pair<uint32_t,uint32_t> price_type;

    std::vector<price_type> operator() (mysqlpp::Connection &conn, const uint32_t item_id);
};

class Stacks
{
public:

    boost::unordered_map<uint32_t,uint16_t> operator() (mysqlpp::Connection &conn);
};

}   }

#endif //__SRDL_SHOP_QUERY_HPP__
