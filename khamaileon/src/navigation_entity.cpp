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

#include "navigation_entity.hpp"

Point NavigationEntity::TransformIntoObjectCoordinates(const Point &point) const
{
    double acos = cos(angle);
    double asin = sin(angle);

    return Point(acos*point.X + asin*point.Y,-asin*point.X + acos*point.Y);
}

Line NavigationEntity::TransformIntoObjectCoordinates(const Line &line) const
{
    Line ret;

    double acos = cos(angle);
    double asin = sin(angle);

    ret.x1 = acos*line.x1 + asin*line.y1;
    ret.y1 = -asin*line.x1 + acos*line.y1;
    ret.z1 = line.z1;
    ret.x2 = acos*line.x2 + asin*line.y2;
    ret.y2 = -asin*line.x2 + acos*line.y2;
    ret.z2 = line.z2;

    return ret;
}

Line NavigationEntity::TransformIntoWorldCoordinates(const Line &line) const
{
    Line ret;

    double acos = cos(angle);
    double asin = sin(angle);

    ret.x1 = acos*line.x1 - asin*line.y1;
    ret.y1 = asin*line.x1 + acos*line.y1;
    ret.z1 = line.z1;
    ret.x2 = acos*line.x2 - asin*line.y2;
    ret.y2 = asin*line.x2 + acos*line.y2;
    ret.z2 = line.z2;

    return ret;
}
