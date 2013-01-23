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

#ifndef __SRDL_MESSAGE_QUERY_HPP__
#define __SRDL_MESSAGE_QUERY_HPP__

#include "connection.hpp"

#include <types.hpp>

namespace DB {
namespace MESSAGE {

class Valid
{
public:

	bool operator() (SLES::DB::Connection &conn, const std::string &name);
};

class Max
{
public:

	bool operator() (SLES::DB::Connection &conn, const std::string &name);
};

class Insert
{
public:

	void operator() (SLES::DB::Connection &conn, const std::string &name, const std::string &sender,
		const std::string &msg, const uint32_t model);
};

class Select
{
public:

	std::string operator() (SLES::DB::Connection &conn, const std::string &name, const uint8_t msg_number);
};

class Delete
{
public:

	void operator() (SLES::DB::Connection &conn, const std::string &name, const uint8_t msg_number);
};

}	}

#endif //__SRDL_MESSAGE_QUERY_HPP__
