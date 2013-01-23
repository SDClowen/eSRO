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

#ifndef __SRDL_ITEM_QUERY_HPP__
#define __SRDL_ITEM_QUERY_HPP__

#include "connection.hpp"

#include <item.hpp>

#include <vector>

namespace DB {
namespace ITEM {

class Create
{
public:

    uint32_t operator() (mysqlpp::Connection &conn, const uint32_t ID, const Item& item);
};

class Select
{
public:

    std::vector<Item::extended_type> operator() (mysqlpp::Connection& conn, const uint32_t CharID,
        const uint8_t location);
};

class Position
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t itemID, const uint8_t pos);
};

class EnchantLv
{
public:

    void operator() (mysqlpp::Connection& conn, const uint32_t itemID, const uint8_t level);
};

class Quantity
{
public:

    void operator() (mysqlpp::Connection& conn, const uint32_t itemID, const uint16_t quantity);
};

class Durability
{
public:

    void operator() (mysqlpp::Connection& conn, const uint32_t itemID, const uint32_t durability);
};

class Percent
{
public:

    void operator() (mysqlpp::Connection& conn, const std::string& name, const Item& item);
};

class Delete
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t itemID);
};

class ID
{
public:

    uint32_t operator() (mysqlpp::Connection &conn, const std::string &name);
};

class MagicOption
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t itemID, const uint32_t optionID,
        const uint32_t value);
};

class SkillID
{
public:

        uint32_t operator() (mysqlpp::Connection &conn, const uint32_t ID);
};

class TransportID
{
public:

    uint32_t operator() (mysqlpp::Connection &conn, const std::string &name);
};

}   }

#endif //__SRDL_ITEM_QUERY_HPP__
