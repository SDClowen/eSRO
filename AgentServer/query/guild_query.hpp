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

#ifndef __SRDL_GUILD_QUERY_HPP__
#define __SRDL_GUILD_QUERY_HPP__

#include "connection.hpp"

/// GLOBAL INCLUDES
#include <boost/shared_ptr.hpp>

class Guild;
class GuildMember;

namespace DB { namespace GUILD {

class Create
{
public:

    uint32_t operator() (mysqlpp::Connection &conn, const std::string& name);
};

class Valid
{
public:

    bool operator() (mysqlpp::Connection& conn, const std::string& name);
};

class Delete
{
public:

    ///CONVERT THIS QUERIES INTO A STORAGE PROCEDURES!!

    void operator() (mysqlpp::Connection &conn, const uint32_t guildID);
};

class Select
{
public:

    boost::shared_ptr<Guild> operator() (mysqlpp::Connection &conn, const uint32_t guildID);
};

class Notice
{
public:

    void operator() (mysqlpp::Connection& conn, const uint32_t guildID,
		const std::string& title, const std::string& notice);
};

class GP
{
public:

    void operator() (mysqlpp::Connection& conn, const uint32_t guildID, uint32_t gp);
};

class Level
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t guildID, uint8_t lv);
};

class Gold
{
public:

    typedef void result_type;

    void operator() (mysqlpp::Connection &conn, const uint32_t guildID, const uint32_t gold);
};

class Union
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t guildID, const uint32_t unionID);
};

class StorageCapacity
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t guildID, const uint8_t capacity);
};

class PromotePolicy
{
public:

    std::map<uint8_t,std::pair<uint16_t,uint16_t> > operator() (mysqlpp::Connection &conn);
};

}   }

#endif //__SRDL_GUILD_QUERY_HPP__
