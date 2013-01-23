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

#ifndef __SRDL_GUILD_MEMBER_QUERY_HPP__
#define __SRDL_GUILD_MEMBER_QUERY_HPP__

/// GLOBAL INCLUDES
#include <mysql++/mysql++.h>
#include <boost/shared_ptr.hpp>

class GuildMember;

namespace DB { namespace GUILD_MEMBER {

class Insert
{
public:

    uint32_t operator() (mysqlpp::Connection& conn, const boost::shared_ptr<GuildMember> member);
};

class Remove
{
public:

    void operator() (mysqlpp::Connection& conn, const uint32_t memberID);
};

class Master
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t memberID, const bool is_master);
};

class Grant
{
public:

    void operator() (mysqlpp::Connection& conn, const uint32_t memberID, const std::string& grant);
};

class GP
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t memberID, const uint32_t GP);
};

class Combat
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t memberID, const uint32_t pts);
};

class Position
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t memberID, const uint8_t position);
};

class Zone
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t memberID, const int16_t zoneID);
};

class Authority
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t memberID, const uint32_t authority);
};

}   }

#endif //__SRDL_GUILD_QUERY_HPP__
