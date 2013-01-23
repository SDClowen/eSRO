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

#ifndef __SR_GAME_SERVER_ZONE_HPP__
#define __SR_GAME_SERVER_ZONE_HPP__

#include <coord.hpp>

#include <stdint.h>
#include <vector>
#include <set>
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

/**
 *
 *  THE WORLD IS DIVIDED IN ZONES (LIKE A GRID), EACH ZONE HAVE A CORRESPONDING X and Y.
 *  EACH ZONE IS 1920 X 1920 (FOR NON-INSTANCE AREAS) THE ZONES CAN BE TOWN (SAFE AREAS) and FIELDS (NON-SAFE AREAS).
 *  A ZONE CAN BE SPLITED IN DIFFERENT TYPES DELIMITED BY A RECT.
 *
 *  ( Y Direction)
 *  ^
 *  |
 *  ----------------
 *  | Z1 | Z2 | Z3 |
 *  ----------------
 *  | Z4 | Z5 | Z6 |
 *  ----------------
 *  | Z7 | Z8 | Z9 |
 *  ---------------------- > (X Direction)
 *
 **/

#define MAX_VISIBLE_DISTANCE 1000

enum ZONE_TYPE
{
    ZONE_TOWN,
    ZONE_FIELD
};

struct ZoneArea
{
    bool belongs (const Coord &pos) const;

    ZONE_TYPE type;                 ///< CORRESPONDING ZONE TYPE
    int left,top,right,bottom;      ///< AREA RECT
};

struct AdjacentZone
{
    uint8_t min_x;
    uint8_t max_x;
    uint8_t min_y;
    uint8_t max_y;
};

struct ZoneInfo
{
    int16_t ID;                ///< ZONE ID
    ZoneArea area;
};

class NPC;
class Player;
class NearObjects;
class DropableItem;
class TeleportBuilding;

class Channel;
class NavigationMesh;
class ModelManager;

AdjacentZone GetAdjacentZones (int16_t zoneID);

struct Zone
{
    typedef std::vector<boost::shared_ptr<TeleportBuilding> >::iterator blg_iterator;
    typedef std::vector<boost::shared_ptr<TeleportBuilding> >::const_iterator blg_const_iterator;

    typedef boost::unordered_map<uint32_t,boost::shared_ptr<NPC> >::iterator npc_iterator;
    typedef boost::unordered_map<uint32_t,boost::shared_ptr<NPC> >::const_iterator npc_const_iterator;

    typedef boost::unordered_map<uint32_t,boost::shared_ptr<Player> >::iterator player_iterator;
    typedef boost::unordered_map<uint32_t,boost::shared_ptr<Player> >::const_iterator player_const_iterator;

    typedef boost::unordered_map<uint32_t,boost::shared_ptr<DropableItem> >::iterator item_iterator;
    typedef boost::unordered_map<uint32_t,boost::shared_ptr<DropableItem> >::const_iterator item_const_iterator;

    Zone (const boost::shared_ptr<NavigationMesh> &navmesh);

    int16_t ID () const;

    boost::shared_ptr<Channel> GetChannel ();

    bool IsInsideTown (const Coord &position) const;

    void InsertPlayer (const boost::shared_ptr<Player> &player);

    boost::shared_ptr<Player> FindPlayer (const uint32_t playerID) const;

    bool RemovePlayer (const uint32_t playerID);

    void InsertNPC (const boost::shared_ptr<NPC> &npc);

    boost::shared_ptr<NPC> FindNPC (const uint32_t npcID) const;

    bool RemoveNPC (const uint32_t npcID);

    void InsertItem (const boost::shared_ptr<DropableItem> &item);

    boost::shared_ptr<DropableItem> FindItem (const uint32_t itemID) const;

    bool RemoveItem (const uint32_t itemID);

    void InsertBuilding (const boost::shared_ptr<TeleportBuilding> &blg);

    boost::shared_ptr<TeleportBuilding> FindBuilding (const uint32_t buildingID) const;

    boost::shared_ptr<TeleportBuilding> FindBuilding () const;

    void RemoveBuilding (const uint32_t buildingID);

    void GetNearObjectIDs (std::set<uint32_t> &ids) const;

    bool IsPositionValid (const Coord &position) const;

    boost::shared_ptr<NavigationMesh> getNavigationMesh () const;

    float GetHeightAt(float x, float y) const;

    bool ResolveMotion (const Coord &src, const Coord &dest, const boost::shared_ptr<ModelManager> &modelMgr,
        Coord *end) const;

    int16_t Width () const;

    int16_t Length () const;
    
    void InsertDelimitedAreas (const ZoneArea &area);

    /**
     *
     *  @brief Number of players in the surroundings.
     *  @return player count.
     *
     **/

    size_t Count () const;

private:

    std::vector<ZoneArea> DelimitedAreas;
    boost::shared_ptr<NavigationMesh> m_NavMesh;

    std::vector<boost::shared_ptr<TeleportBuilding> > building_list;
    boost::unordered_map<uint32_t,boost::shared_ptr<NPC> > npc_list;
    boost::unordered_map<uint32_t,boost::shared_ptr<Player> > player_list;
    boost::unordered_map<uint32_t,boost::shared_ptr<DropableItem> > item_list;

    mutable boost::mutex m_mutex;
    boost::shared_ptr<Channel> m_channel;
};

#endif
