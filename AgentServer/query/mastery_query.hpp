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

#ifndef __SRDL_MASTERY_QUERY_HPP__
#define __SRDL_MASTERY_QUERY_HPP__

#include "connection.hpp"

#include <mastery_tree.hpp>

namespace DB {
namespace MASTERY {

class Create
{
public:

    /**
     *
     *  Save character mastery information into the database.
     *
     *  @param conn Database Connection.
     *  @param ID CharacterID to update.
     *  @param mastery Mastery information to update.
     *
     */

    void operator() (mysqlpp::Connection &conn, const uint32_t ID, const Mastery& mastery);
};

class Update
{
public:

    /**
     *
     *  @param conn Database Connection.
     *  @param name Character name to update.
     *  @param mastery Mastery information to update.
     *
     */

    void operator() (mysqlpp::Connection& conn, const uint32_t ID, const Mastery& mastery);
};

class Select
{
public:

    MasteryTree operator() (mysqlpp::Connection& conn, const uint32_t ID, const uint8_t Level,
        const RACE race);
};

class Delete
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t ID);
};

}   }

#endif //__SRDL_MASTERY_QUERY_HPP__
