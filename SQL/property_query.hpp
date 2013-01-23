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

#ifndef __SRLF_DB_PROPERTY_QUERY_HPP__
#define __SRLF_DB_PROPERTY_QUERY_HPP__

#include <npc_task.hpp>
#include <magicoption_property.hpp>

#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>

#include <mysql++/mysql++.h>

namespace DB {
namespace PROPERTY {

class CharacterQuality
{
public:

    std::multimap<uint32_t,std::pair<uint8_t,uint8_t> > operator() (mysqlpp::Connection &conn);
};

class NPC
{
public:

    std::multimap<uint32_t,NPCTask> operator () (mysqlpp::Connection &conn);
};

class MagicOption
{
public:

    std::vector<MagicOptionProperty> operator() (mysqlpp::Connection &conn);
};


}   }

#endif
