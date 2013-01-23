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

#ifndef __SLES_DATABASE_ACCOUNT_QUERY_HPP__
#define __SLES_DATABASE_ACCOUNT_QUERY_HPP__

#include <mysql++/mysql++.h>

#include <stdint.h>
#include <boost/tuple/tuple.hpp>

namespace DB {
namespace Account {

enum LOGIN_RET
{
    LOGIN_ACCEPTED,
    LOGIN_REJECTED,
    LOGIN_MAX_ATTEMPTS,
    LOGIN_BANNED,
    LOGIN_ALREDY_LOGGED
};

struct AccountExt
{
    uint8_t acc;    ///Access Level.
    uint8_t sec;    ///Security Level.
    uint32_t silk;  ///Current Silk.
};

class Login
{
public:

    LOGIN_RET operator () (mysqlpp::Connection *conn, const std::string &name, const std::string &pass,
        const std::string &IP);
};

class Valid
{
public:

	bool operator() (mysqlpp::Connection *conn, const std::string& name,
		const std::string& pass);
};

class IsLogged
{
public:

	bool operator() (mysqlpp::Connection *conn, const std::string& name);
};

class SetLogged
{
public:

	void operator() (mysqlpp::Connection *conn, const uint32_t accountID, bool status);
};

class Clear
{
public:

	void operator() (mysqlpp::Connection *conn);
};

class Silk
{
public:

	uint32_t operator() (mysqlpp::Connection *conn, const uint32_t accountID);

	void operator () (mysqlpp::Connection *conn, const uint32_t accountID, const uint32_t silk);
};

class Attempt
{
public:

    int32_t operator () (mysqlpp::Connection *conn, const std::string &usr);
};

class BanExpirationDate
{
public:

    mysqlpp::DateTime operator () (mysqlpp::Connection *conn, const std::string &usr);
};

class Ext
{
public:

    typedef boost::tuple<uint32_t,uint8_t,uint8_t,uint32_t> return_type;

    return_type operator() (mysqlpp::Connection *conn, const std::string &usr);
};

}   }

#endif //__SLES_DATABASE_ACCOUNT_QUERY_HPP__
