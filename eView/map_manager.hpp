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

#ifndef MAP_MANAGER_HPP
#define MAP_MANAGER_HPP

#include "coord.hpp"

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/unordered_map.hpp>

class Zone;
class ModelManager;

class MapManager
{
public:

    typedef boost::unordered_map<int16_t,boost::shared_ptr<Zone> >::iterator iterator;
    typedef boost::unordered_map<int16_t,boost::shared_ptr<Zone> >::const_iterator const_iterator;

    MapManager (const std::string &path);

    ~MapManager ();

    bool LoadZones (int mode);

    bool LoadZone (const char *name, int mode);

    boost::shared_ptr<Zone> Find (const int16_t zoneID) const;

    bool IsInsideTown (const Coord &position) const;

    bool IsPositionValid (const Coord &position) const;

    const_iterator begin () const;

    const_iterator end () const;

    bool ResolveMotion (const Coord &src, const Coord &dest, const boost::shared_ptr<ModelManager> &modelMgr,
        Coord *end) const;

private:

    std::string m_Path;
    mutable boost::mutex m_mutex;
    boost::unordered_map<int16_t,boost::shared_ptr<Zone> > m_ZoneTable;
};

#endif // MAP_MANAGER_HPP
