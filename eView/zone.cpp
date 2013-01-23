#include "zone.hpp"
#include <navigation_mesh.hpp>

/// SRLF INCLUDES
//#include <player.hpp>
//#include <dropable_item.hpp>

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
    : m_NavMesh(navmesh)
{
}

int16_t Zone::ID() const
{
    return m_NavMesh->ID();
}

boost::shared_ptr<Channel> Zone::GetChannel()
{
    return m_channel;
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
}/*********************************************************************************
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



bool Zone::IsPositionValid (const Coord &position) const
{
//    if (position.get_x() < 0 || position.get_x() > 1920)
//        return false;
//
//    if (position.get_y() < 0 || position.get_y() > 1920)
//        return false;
//
//    /// ALLOW A LITTLE TOLERANCE FOR Z-COORD.
//
//    if (abs(position.get_z() - GetHeightAt(position.get_x(),position.get_y())) > 100)
//    {
//        syslog(LOG_INFO,"Z Coord = %f",GetHeightAt(position.get_x(),position.get_y()));
////        return false;
//    }

    return true;
}

boost::shared_ptr<NavigationMesh> Zone::getNavigationMesh () const
{
    return m_NavMesh;
}

float Zone::GetHeightAt(float x, float y) const
{
    x /= 20.0f;
    y /= 20.0f;

    return m_NavMesh->getHeightAt(x,y);
}

bool Zone::ResolveMotion (const Coord &src, const Coord &dest, const boost::shared_ptr<ModelManager> &modelMgr,
    Coord *end) const
{
    return m_NavMesh->resolveMotion(src,dest,modelMgr,end);
}

int16_t Zone::Width () const
{
    return m_NavMesh->Width();
}

int16_t Zone::Length () const
{
    return m_NavMesh->Length();
}

void Zone::InsertDelimitedAreas (const ZoneArea &area)
{
    DelimitedAreas.push_back(area);
}
