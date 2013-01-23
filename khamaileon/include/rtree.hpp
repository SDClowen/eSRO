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

#ifndef RTREE_HPP
#define RTREE_HPP

#include <stdint.h>
#include <vector>
#include <cmath>

namespace SR {

struct Region
{
    Region (float x1 = 0, float y1 = 0, float x2 = 0, float y2 = 0)
        :   minX(x1), maxX(x2), minY(y1), maxY(y2),refID(0)
    {
    }

    bool Contains (const Region &region) const
    {
        if (minX > region.maxX || maxX < region.minX || minY > region.maxY || maxY < region.minY)
            return false;

        return true;
    }

    bool Contains (const float X, const float Y) const
    {
        if (minX > X || maxX < X || minY > Y || maxY < Y)
            return false;

        return true;
    }

    bool Intersects (const Region &region) const
    {
        float xsum = Width() + region.Width();
        float ysum = Height() + region.Height();

        float xcenter = (region.minX+region.maxX) - (minX+maxX);
        float ycenter = (region.minY+region.maxY) - (minY+maxY);

        return std::abs((double)xcenter) < xsum && std::abs((double)ycenter) < ysum;
    }

    float CenterX () const
    {
        return (minX+maxX)/2;
    }

    float CenterY () const
    {
        return (minY+maxY)/2;
    }

    float Width () const
    {
        return maxX - minX;
    }

    float Height () const
    {
        return maxY - minY;
    }

    float minX, maxX;
    float minY, maxY;
    size_t refID;
};

struct RTreeNode
{
    RTreeNode ();

    RTreeNode (const Region &region, uint16_t MaxDepth);

    ~RTreeNode ();

    bool InsertRect (const Region &region);

    bool Collides (const float x, const float y, size_t *rectID) const;

    bool Partition ();
    
    RTreeNode *NE;
    RTreeNode *SE;
    RTreeNode *NW;
    RTreeNode *SW;

    Region m_Region;
    uint16_t m_MaxDepth;
    std::vector<Region> m_Rects;
};

class RTree
{
public:

    RTree ();

    RTree (const Region &region, uint16_t depth = 2);

    ~RTree ();

    bool InsertRect (const Region &region);

    bool Collides (const float x, const float y, size_t *rectID) const;

    void Clear ();

protected:

    RTreeNode *m_Root;

    Region m_Region;
    uint16_t m_MaxDepth;
};

}

#endif // RTREE_HPP
