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

#ifndef __SLES_DATABASE_ACADEMY_QUERY_HPP__
#define __SLES_DATABASE_ACADEMY_QUERY_HPP__

#include "academy.hpp"
#include "connection.hpp"

namespace DB {
namespace ACADEMY {

class Check
{
public:

	bool operator() (SLES::DB::Connection &conn, const std::string &name);
};

class Select
{
public:

	Academy operator() (SLES::DB::Connection &conn, const std::string &name);
};

/**
 *
 *	brief Obtain online academy members names.
 *
 **/

class Members
{
public:

	std::vector<std::string> operator() (SLES::DB::Connection &conn, const std::string &name);
};

class Notice
{
public:

	void operator() (SLES::DB::Connection &conn, const std::string &name, const std::string &title,
		const std::string &notice);
};

}	}

#endif //__SLES_DATABASE_ACADEMY_QUERY_HPP__
