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

#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include <stdint.h>
#include <cmath>

enum RECT_EDGE
{
    RECT_EDGE_TOP,
    RECT_EDGE_LEFT,
    RECT_EDGE_BOTTOM,
    RECT_EDGE_RIGHT,
    RECT_EDGE_NONE = 255
};

struct Rect
{
    Rect (float x1 = 0, float y1 = 0, float x2 = 0, float y2 = 0);

    bool Contains (const Rect &rect) const;

    bool Contains (const float X, const float Y) const;

    bool Intersects (const Rect &rect) const;

    float CenterX () const;

    float CenterY () const;

    float Width () const;

    float Height () const;

    float minX, maxX;
    float minY, maxY;
};

#endif // RECTANGLE_HPP
