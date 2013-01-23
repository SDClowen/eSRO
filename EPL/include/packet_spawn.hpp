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

#ifndef __SLES_GAME_SERVER_SPAWN_PACKETS_HPP__
#define __SLES_GAME_SERVER_SPAWN_PACKETS_HPP__

#include <set>
#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <vector>

class OPacket;
class NPC;
class Player;
class DropableItem;
class TeleportBuilding;

namespace srv_pkt
{

enum SPAWN_TYPE
{
    SPAWN_OBJECT = 1,
    DESPAWN_OBJECT
};

typedef std::vector<boost::shared_ptr<NPC> > NPCVector;
typedef std::vector<boost::shared_ptr<Player> > PlayerVector;
typedef std::vector<boost::shared_ptr<TeleportBuilding> > TeleportVector;

void SpawnBegin (const boost::shared_ptr<OPacket> &pkt, uint16_t count, const SPAWN_TYPE type);

void Spawn (const boost::shared_ptr<OPacket> &pkt, NPCVector::const_iterator begin, NPCVector::const_iterator end);

void Spawn (const boost::shared_ptr<OPacket> &pkt, const NPCVector& object_list);

void Spawn (const boost::shared_ptr<OPacket> &pkt, const  TeleportVector &gateway_list);

void Spawn (const boost::shared_ptr<OPacket> &pkt, const PlayerVector &player_list);

void Spawn (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<DropableItem> &item);

void Spawn (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Player> &player);

void Spawn (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<NPC> &npc);

/**
 *
 *  @brief Build packet for despawning mobs.
 *
 */

void Spawn (const boost::shared_ptr<OPacket> &pkt, const std::vector<uint32_t>& id_list);

void Spawn(const boost::shared_ptr<OPacket> &pkt,
    std::set<uint32_t>::const_iterator begin, std::set<uint32_t>::const_iterator end);

void Spawn (const boost::shared_ptr<OPacket> &pkt, const std::set<std::pair<uint32_t,std::string> > &player_list);

void Despawn (const boost::shared_ptr<OPacket> &pkt, uint32_t ID);

void SpawnEnd (const boost::shared_ptr<OPacket> &pkt);

}

#endif //__SLES_GAME_SERVER_SPAWN_PACKETS_HPP__
