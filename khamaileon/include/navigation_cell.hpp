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

#ifndef NAVIGATIONCELL_HPP
#define NAVIGATIONCELL_HPP

#include "line.hpp"
#include "rectangle.hpp"

#include <coord.hpp>

#include <stdint.h>
#include <vector>

/**
 *
 *  @NavigationLink Represents the connection between cells of the map navigation mesh.
 *  The links can between cells of the same area of between cells of different areas adjecents to each other.
 *
 *  When cell are linked they define the source and destination edge with index
 *  (1 = LEFT, 2 = TOP, 3 = RIGHT, 4 = BOTTOM, 255 = NONE)
 *
 **/

struct NavigationLink
{
    enum LINK_TYPE
    {
        LINK_WALL = 0x02,
        LINK_CELL = 0x04,
        LINK_ZONE = 0x08
    };

    NavigationLink ();

    Line edge;
    uint8_t type;
    uint8_t edgeSrc;            /// Which edge of the cell originates.
    uint8_t edgeDest;           /// Which edge of the cell ends.
    uint16_t cellSrc;           /// Source NavCell.
    uint16_t cellDest;          /// Destination NavCell. (If zoneID dest != 0 its from zoneDest)
    int16_t zoneSrc;            /// ZoneID source
    int16_t zoneDest;           /// ZoneID destination
};

struct NavigationCell
{
    typedef std::vector<NavigationLink>::const_iterator link_const_iterator;

    enum PATH_RESULT
    {
        NO_RELATIONSHIP,
        ENDING_CELL,
        EXITING_CELL,
        EXITING_ZONE
    };

    /**
     *
     *  @brief Classify the motion path across the navigation cell.
     *  @path: Entity motion path.
     *  @return Path clasification.
     *
     **/

    std::pair<PATH_RESULT,link_const_iterator> ClassifyPath (const size_t prevCell, const size_t curCell,
        const Line &path, bool dirVDown, bool dirHLeft, bool dirPrevH, Coord *inter) const;

    Rect rect;
    std::vector<uint32_t> entities;     /// OBJECT IDS THAT ARE CONTAINED IN AREA
    std::vector<NavigationLink> links;  /// CELL LINKS (INNER ZONE AND OUTER ZONE)
};

#endif // NAVIGATIONCELL_HPP
