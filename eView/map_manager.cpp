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

#include "map_manager.hpp"
#include "zone.hpp"

#include <navigation_mesh.hpp>

#include <dirent.h>
#include <string.h>
#include <syslog.h>
#include <fstream>
#include <iostream>

int nvm (const dirent *ep)
{
    return strstr(ep->d_name,".nvm") != NULL;
}

MapManager::MapManager (const std::string &path)
    : m_Path(path+"/Data")
{
}

MapManager::~MapManager()
{
}

bool MapManager::LoadZones (int mode)
{
    std::string nvm_path = m_Path + "/navmesh";

    dirent **eps;
    int n;

    n = scandir(nvm_path.c_str(),&eps,nvm,alphasort);

    if (n > 0)
    {
        for (int cnt = 0; cnt < n; ++cnt)
            if (!LoadZone(eps[cnt]->d_name,mode))
                return false;
    }
    else
        return false;

    /// LOAD ZONE AREA DATA!
    std::string filename = m_Path + "/regioninfo.txt";
    std::ifstream file(filename.c_str());

    if (!file)
        return false;

    int x,y;
    std::string box;
    ZoneArea area;
    iterator iter;

    while (!file.eof())
    {
        std::string line;
        getline(file,line);

        if (*line.begin() == '\t')
            continue;

        if (*line.begin() == '#')
        {
            if (line.find("#TOWN") != std::string::npos)
                area.type = ZONE_TOWN;
            else
                area.type = ZONE_FIELD;

            continue;
        }

        std::stringstream s(line), query;
        s >> x >> y >> box;

        int16_t zoneID = (y << 8) + x;
        iter = m_ZoneTable.find(zoneID);

        if (iter != m_ZoneTable.end())
        {
            if (box == "RECT")
                s >> area.left >> area.top >> area.right >> area.bottom;
            else
                area.left = area.top = area.right = area.bottom = -1;

            iter->second->InsertDelimitedAreas(area);
        }
    }

    file.close();

    return true;
}

bool MapManager::LoadZone (const char *name, int mode)
{
    int16_t zoneID = strtol(name+3,NULL,16);

    std::string filename = m_Path + "/navmesh/" + name;

    boost::shared_ptr<NavigationMesh> navmesh(new NavigationMesh(zoneID));

    if (!navmesh || !navmesh->Read(filename,static_cast<NavigationMesh::READ_MODE>(mode)))
        return false;

    if (mode & NavigationMesh::MODE_NAV_MESH && !navmesh->buildNavigationMesh())
        return false;

    boost::shared_ptr<Zone> zone(new Zone(navmesh));

    if (!zone)
        return false;

    m_ZoneTable.insert(std::make_pair(zoneID,zone));

    return true;
}

boost::shared_ptr<Zone> MapManager::Find (const int16_t zoneID) const
{
    boost::mutex::scoped_lock lock(m_mutex);

    boost::shared_ptr<Zone> zone;

    const_iterator iter = m_ZoneTable.find(zoneID);

    if (iter != m_ZoneTable.end())
        zone = iter->second;

    return zone;
}

bool MapManager::IsInsideTown (const Coord &position) const
{
    const_iterator iter = m_ZoneTable.find(position.get_zone());

    if (iter == m_ZoneTable.end())
    {
        syslog(LOG_INFO,"Zone %i dont exist.",position.get_zone());
        return false;
    }

    return iter->second->IsInsideTown(position);
}

bool MapManager::IsPositionValid (const Coord &position) const
{
    const_iterator iter = m_ZoneTable.find(position.get_zone());

    if (iter == m_ZoneTable.end())
    {
        syslog(LOG_INFO,"Zone %i dont exist.",position.get_zone());
        return false;
    }

    return iter->second->IsPositionValid(position);
}

MapManager::const_iterator MapManager::begin() const
{
    return m_ZoneTable.begin();
}

MapManager::const_iterator MapManager::end() const
{
    return m_ZoneTable.end();
}

bool MapManager::ResolveMotion (const Coord &src, const Coord &dest,
    const boost::shared_ptr<ModelManager> &modelMgr, Coord *end) const
{
    assert(end);

    Coord current = src;

    while(1)
    {
        const_iterator iter = m_ZoneTable.find(current.get_zone());

        if (iter == m_ZoneTable.end())
        {
//            std::cout << "Couldnt find Zone = " << current.get_zone() << std::endl;
            return false;
        }

        int8_t zdx = (dest.get_zone() & 0xFF) - (current.get_zone() & 0xFF);
        int8_t zdy = (dest.get_zone() >> 8) - (current.get_zone() >> 8);

        Coord relative(zdx*1920+dest.get_x(),zdy*1920+dest.get_y(),dest.get_z(),current.get_zone());

//        std::cout << "Rel Point = " << relative.get_x() << "," << relative.get_y() << " - " << relative.get_zone() << std::endl;

        iter->second->ResolveMotion(current,relative,modelMgr,end);

        if (!end->get_zone())
            break;

        current = *end;
        end->set_zone(0);

//        std::cout << "Entering Zone = " << current.get_zone() << std::endl;
    }

    end->set_zone(current.get_zone());

    return true;
}
