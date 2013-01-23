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

#include "zone.hpp"
#include "near_objects.hpp"
#include "server_channel.hpp"

/// SOL INCLUDES
#include <player.hpp>
#include <navigation_mesh.hpp>
#include <dropable_item.hpp>

#include <cassert>
#include <syslog.h>
#include <boost/bind.hpp>

bool ZoneArea::belongs (const Coord &pos) const
{
    bool valid = false;

    if ( left == -1 && top == -1 && right == -1 && bottom == -1)
        valid = true;
    else
    {
        if ( pos.get_x() >= left && pos.get_x() <= right && pos.get_y() >= bottom && pos.get_y() <= top)
            valid = true;
    }

    return valid;
}

AdjacentZone GetAdjacentZones (int16_t zoneID)
{
    uint8_t SectorX = (zoneID & 0x00FF);
    uint8_t SectorY = (zoneID & 0xFF00) >> 8;

    AdjacentZone area;
    area.min_x = SectorX;
    area.max_x = SectorX;
    area.min_y = SectorY;
    area.max_y = SectorY;

    if ( SectorY == 128)
    {
        ///IN THE CAVES OR ANY INSTANCE OF A MAP THE MOBS ARE IN THE SAME ZONE.
    }
    else
    {
        --area.min_x;
        --area.min_y;
        ++area.max_x;
        ++area.max_y;
    }

    return area;
}

Zone::Zone (const boost::shared_ptr<NavigationMesh> &navmesh)
    : m_NavMesh(navmesh), m_channel(new Channel(navmesh->ID()))
{
}

int16_t Zone::ID() const
{
    return m_NavMesh->ID();
}

float Zone::minX () const
{
    return m_NavMesh->minX();
}

float Zone::maxX () const
{
    return m_NavMesh->maxX();
}

float Zone::minZ () const
{
    return m_NavMesh->minZ();
}

float Zone::maxZ () const
{
    return m_NavMesh->maxZ();
}

int16_t Zone::Width () const
{
    return m_NavMesh->Width();
}

int16_t Zone::Length () const
{
    return m_NavMesh->Length();
}

boost::shared_ptr<Channel> Zone::GetChannel()
{
    return m_channel;
}

void Zone::setModelManager (const boost::shared_ptr<ModelManager> &modelMgr)
{
    m_modelMgr = modelMgr;
}

bool Zone::IsInsideTown (const Coord &position) const
{
    bool inside = false;

    for (std::vector<ZoneArea>::const_iterator i = DelimitedAreas.begin(); i != DelimitedAreas.end(); ++i)
    {
        if (i->belongs(position))
        {
            inside = i->type == ZONE_TOWN;
            break;
        }
    }

    return inside;
}

void Zone::InsertPlayer (const boost::shared_ptr<Player> &player)
{
    boost::mutex::scoped_lock lock(m_mutex);

    player_list.insert(std::make_pair(player->GetUniqueID(),player));
}

boost::shared_ptr<Player> Zone::FindPlayer (const uint32_t playerID) const
{
    boost::mutex::scoped_lock lock(m_mutex);

    boost::shared_ptr<Player> player;

    player_const_iterator iter = player_list.find(playerID);

    if (iter != player_list.end())
        player = iter->second;

    return player;
}

bool Zone::RemovePlayer (const uint32_t playerID)
{
    boost::mutex::scoped_lock lock(m_mutex);

    return player_list.erase(playerID);
}

void Zone::InsertNPC (const boost::shared_ptr<NPC> &npc)
{
    boost::mutex::scoped_lock lock(m_mutex);

    npc_list.insert(std::make_pair(npc->GetUniqueID(),npc));
}

boost::shared_ptr<NPC> Zone::FindNPC (const uint32_t npcID) const
{
    boost::mutex::scoped_lock lock(m_mutex);

    boost::shared_ptr<NPC> npc;

    npc_const_iterator iter = npc_list.find(npcID);

    if (iter != npc_list.end())
        npc = iter->second;

    return npc;
}

bool Zone::RemoveNPC (const uint32_t npcID)
{
    boost::mutex::scoped_lock lock(m_mutex);

    return npc_list.erase(npcID);
}

void Zone::InsertItem (const boost::shared_ptr<DropableItem> &item)
{
    boost::mutex::scoped_lock lock(m_mutex);

    item_list.insert(std::make_pair(item->GetUniqueID(),item));
}

boost::shared_ptr<DropableItem> Zone::FindItem (const uint32_t itemID) const
{
    boost::mutex::scoped_lock lock(m_mutex);

    boost::shared_ptr<DropableItem> itm;

    item_const_iterator iter = item_list.find(itemID);

    if (iter != item_list.end())
        itm = iter->second;

    return itm;
}

bool Zone::RemoveItem (const uint32_t itemID)
{
    boost::mutex::scoped_lock lock(m_mutex);

    return item_list.erase(itemID);
}

void Zone::InsertBuilding (const boost::shared_ptr<TeleportBuilding> &blg)
{
    boost::mutex::scoped_lock lock(m_mutex);

    building_list.push_back(blg);
}

boost::shared_ptr<TeleportBuilding> Zone::FindBuilding (const uint32_t buildingID) const
{
    boost::mutex::scoped_lock lock(m_mutex);

    boost::shared_ptr<TeleportBuilding> blg;

    for (size_t idx = 0; idx < building_list.size(); ++idx)
    {
        if (building_list[idx]->GetUniqueID() == buildingID)
        {
            blg = building_list[idx];
            break;
        }
    }

    return blg;
}

boost::shared_ptr<TeleportBuilding> Zone::FindBuilding () const
{
    boost::mutex::scoped_lock lock(m_mutex);

    boost::shared_ptr<TeleportBuilding> blg;

    if (!building_list.empty())
        blg = *building_list.begin();

    return blg;
}

void Zone::RemoveBuilding (const uint32_t buildingID)
{
    boost::mutex::scoped_lock lock(m_mutex);

    for (blg_iterator j = building_list.begin(); j != building_list.end(); ++j)
    {
        if ((*j)->GetUniqueID() == buildingID)
        {
            building_list.erase(j);
            break;
        }
    }
}

void Zone::GetNearObjects (const uint32_t playerID, NearObjects *objs) const
{
    boost::mutex::scoped_lock lock(m_mutex);

    for (blg_const_iterator iter = building_list.begin(); iter != building_list.end(); ++iter)
        objs->building_list.push_back(*iter);

    for (npc_const_iterator iter = npc_list.begin(); iter != npc_list.end(); ++iter)
        objs->npc_list.push_back(iter->second);

    for (player_const_iterator iter = player_list.begin(); iter != player_list.end(); ++iter)
        if (iter->second->GetUniqueID() != playerID)
            objs->player_list.push_back(iter->second);

    for (item_const_iterator iter = item_list.begin(); iter != item_list.end(); ++iter)
        objs->item_list.push_back(iter->second);
}

void Zone::GetNearObjectIDs (std::set<uint32_t> &ids) const
{
    boost::mutex::scoped_lock lock(m_mutex);

    for (blg_const_iterator iter = building_list.begin(); iter != building_list.end(); ++iter)
        ids.insert((*iter)->GetUniqueID());

    for (npc_const_iterator iter = npc_list.begin(); iter != npc_list.end(); ++iter)
        ids.insert(iter->second->GetUniqueID());

    for (player_const_iterator iter = player_list.begin(); iter != player_list.end(); ++iter)
        ids.insert(iter->second->GetUniqueID());

    for (item_const_iterator iter = item_list.begin(); iter != item_list.end(); ++iter)
        ids.insert(iter->second->GetUniqueID());
}

float Zone::getHeightAt(float x, float z, float y) const
{
    assert(x >= m_NavMesh->minX() && x <= m_NavMesh->maxX() && z >= m_NavMesh->minZ() && z <= m_NavMesh->maxZ());

    float h = getTerrainHeight(static_cast<int>(x),static_cast<int>(z));

    if (y > h)
    {
        float hobj = getObjectHeight(static_cast<int>(x),static_cast<int>(z));

        if (hobj != -1e4 && y >= hobj)
            h = hobj;
    }

    return h;
}

float Zone::getTerrainHeight (float x, float z) const
{
    assert(x >= m_NavMesh->minX() && x <= m_NavMesh->maxX() && z >= m_NavMesh->minZ() && z <= m_NavMesh->maxZ());

    x /= 20.0f;
    z /= 20.0f;

    return m_NavMesh->getHeightAt((int)x,(int)z);
}

float Zone::getObjectHeight (float x, float z) const
{
    if (!m_NavMesh)
        return -1e4;

    return m_NavMesh->getObjectHeight(x,z,m_modelMgr);
}

bool Zone::ResolveMotion (const Coord &src, const Coord &dest, Coord *end) const
{
    return m_NavMesh->resolveMotion(src,dest,m_modelMgr,end);
}

void Zone::InsertDelimitedAreas (const ZoneArea &area)
{
    DelimitedAreas.push_back(area);
}

size_t Zone::Count () const
{
    return m_channel->size();
}
