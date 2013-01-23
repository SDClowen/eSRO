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

#ifndef __SRDL_FRIEND_QUERY_HPP__
#define __SRDL_FRIEND_QUERY_HPP__

#include "connection.hpp"

#include <types.hpp>
#include <friend.hpp>

#include <vector>

namespace DB {
namespace FRIEND {

class Select
{
public:

    FriendBook operator() (mysqlpp::Connection &conn, const uint32_t charID);
};

class Delete
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t CharID_A, const uint32_t CharID_B);
};

class Insert
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t CharID_A, const uint32_t CharID_B);
};

class GroupValid
{
public:

    bool operator () (mysqlpp::Connection &conn, const std::string &name);
};

class GroupCreate
{
public:

    uint32_t operator () (mysqlpp::Connection &conn, const uint32_t charID, const uint16_t groupID,
        const std::string &name);
};

class GroupDelete
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t refgroupID);
};

class GroupEntryInsert
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t charID, const uint32_t refgroupID);
};

class GroupEntryRemove
{
public:

    void operator () (mysqlpp::Connection &conn, const uint32_t charID, const uint32_t refgroupID);
};

}	}

#endif //__SRDL_FRIEND_QUERY_HPP__
