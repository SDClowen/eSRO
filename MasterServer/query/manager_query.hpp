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

#ifndef __SRDL_MANAGER_QUERY_HPP__
#define __SRDL_MANAGER_QUERY_HPP__

#include <stdint.h>
#include <mysql++/mysql++.h>
#include <boost/tuple/tuple.hpp>

struct ServConfig
{
    bool ch_char;
    bool eu_char;
};

namespace DB {
namespace MANAGER {

class Config
{
public:

    ServConfig operator() (mysqlpp::Connection *conn, const std::string &name);
};

class NameFilter
{
public:

    typedef boost::tuple<std::string,std::vector<std::string> > return_type;

    return_type operator () (mysqlpp::Connection *conn);
};

}   }

#endif
