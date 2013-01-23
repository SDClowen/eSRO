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

#ifndef __SR_GAME_SERVER_CHARACTER_FACTORY_HPP__
#define __SR_GAME_SERVER_CHARACTER_FACTORY_HPP__

#include <item.hpp>
#include <player_property.hpp>

#include <map>
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

class NPC;
class Player;
class EntityProperty;

class CharacterFactory
{
public:

    typedef boost::unordered_map<uint32_t,boost::shared_ptr<EntityProperty> > property_map;
    typedef std::multimap<uint32_t,std::pair<uint8_t,uint8_t> > quality_map;

    CharacterFactory (const property_map &property_list);

    CharacterFactory (const property_map &property_list, const quality_map &quality_chart);

    boost::shared_ptr<NPC> Create (const uint32_t ID) const;

    boost::shared_ptr<NPC> Create (const uint32_t refCharID, uint8_t quality) const;

    boost::shared_ptr<Player> Create (const PlayerProperty &property) const;

    boost::shared_ptr<Player> Create (const uint32_t ID, const uint8_t volume, const std::string &name,
        const std::vector<Item> &items) const;

    boost::shared_ptr<EntityProperty> GetProperty (const uint32_t refCharID) const;

private:

    mutable boost::mutex m_mutex;
    quality_map m_quality_chart;
    property_map m_property_list;
};

#endif
