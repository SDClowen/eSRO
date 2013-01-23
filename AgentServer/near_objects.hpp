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

#ifndef __SR_GAMESERVER_NEAR_OBJECTS_HPP__
#define __SR_GAMESERVER_NEAR_OBJECTS_HPP__

#include <stdint.h>

class NPC;
class Player;
class DropableItem;

struct NearObjects
{
    typedef std::vector<boost::shared_ptr<DropableItem> >::const_iterator item_const_iterator;

    std::vector<boost::shared_ptr<TeleportBuilding> > building_list;
    std::vector<boost::shared_ptr<NPC> > npc_list;
    std::vector<boost::shared_ptr<Player> > player_list;
    std::vector<boost::shared_ptr<DropableItem> > item_list;
};

#endif
