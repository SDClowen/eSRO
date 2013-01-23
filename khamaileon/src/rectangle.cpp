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

#include "rectangle.hpp"

Rect::Rect (float x1, float y1, float x2, float y2)
    :   minX(x1), maxX(x2), minY(y1), maxY(y2)
{
}

bool Rect::Contains (const Rect &rect) const
{
    if (minX > rect.minX || maxX < rect.maxX || minY > rect.minY || maxY < rect.maxY)
        return false;

    return true;
}

bool Rect::Contains (const float X, const float Y) const
{
    if (minX - 1E-6 > X)
        return false;

    if (maxX + 1E-6 < X)
        return false;

    if (minY - 1E-6 > Y)
        return false;

    if (maxY + 1E-6 < Y)
        return false;

    return true;
}

bool Rect::Intersects (const Rect &rect) const
{
    float xsum = Width() + rect.Width();
    float ysum = Height() + rect.Height();

    float xcenter = (rect.minX+rect.maxX) - (minX+maxX);
    float ycenter = (rect.minY+rect.maxY) - (minY+maxY);

    return std::abs((double)xcenter) < xsum && std::abs((double)ycenter) < ysum;
}

float Rect::CenterX () const
{
    return (minX+maxX)/2;
}

float Rect::CenterY () const
{
    return (minY+maxY)/2;
}

float Rect::Width () const
{
    return maxX - minX;
}

float Rect::Height () const
{
    return maxY - minY;
}
