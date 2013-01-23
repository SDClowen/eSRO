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

#include "rtree.hpp"
#include <cassert>
#include <iostream>

using namespace SR;

RTreeNode::RTreeNode ()
    : NE(NULL), SE(NULL), NW(NULL), SW(NULL), m_MaxDepth(0)
{
    m_Region.minX = 0;
    m_Region.minY = 0;
    m_Region.maxX = 0;
    m_Region.maxY = 0;
}

RTreeNode::RTreeNode (const Region &region, uint16_t MaxDepth)
    : NE(0), SE(0), NW(0), SW(0), m_Region(region), m_MaxDepth(MaxDepth)
{
}

RTreeNode::~RTreeNode()
{
    if (NE)
    {
        delete NE;
        NE = 0;
    }

    if (SE)
    {
        delete SE;
        SE = 0;
    }

    if (NW)
    {
        delete NW;
        NW = 0;
    }

    if (SW)
    {
        delete SW;
        SW = 0;
    }
}

bool RTreeNode::InsertRect (const Region &region)
{
    if (!m_MaxDepth)
    {
        assert(m_Region.Contains(region));

        m_Rects.push_back(region);
    }
    else
    {
        if ((region.minX < m_Region.CenterX() && region.maxX > m_Region.CenterX()) ||
            (region.minY < m_Region.CenterY() && region.maxY > m_Region.CenterY()))
        {
            m_Rects.push_back(region);
        }
        else
        {
            if (!Partition())
                return false;

            if (region.minX >= m_Region.CenterX())
            {
                if (region.minY >= m_Region.CenterY())
                    NE->InsertRect(region);
                else
                    SE->InsertRect(region);
            }
            else
            {
                if (region.minY >= m_Region.CenterY())
                    NW->InsertRect(region);
                else
                    SW->InsertRect(region);
            }
        }
    }

    return true;
}

bool RTreeNode::Collides (const float x, const float y, size_t *rectID) const
{
    if (m_MaxDepth)
    {
        if (NE && NE->m_Region.Contains(x,y) && NE->Collides(x,y,rectID))
            return true;

        if (SE && SE->m_Region.Contains(x,y) && SE->Collides(x,y,rectID))
            return true;

        if (NW && NW->m_Region.Contains(x,y) && NW->Collides(x,y,rectID))
            return true;

        if (SW && SW->m_Region.Contains(x,y) && SW->Collides(x,y,rectID))
            return true;
    }

    for (std::vector<Region>::const_iterator it = m_Rects.begin(); it != m_Rects.end(); ++it)
    {
        if (it->Contains(x,y))
        {
            if (rectID)
                *rectID = it->refID;

            return true;
        }
    }

    return false;
}

bool RTreeNode::Partition ()
{
    if (!NE)
    {
        Region region = m_Region;
        region.minX = m_Region.CenterX();
        region.minY = m_Region.CenterY();

        NE = new RTreeNode(region,m_MaxDepth-1);
    }

    if (!SE)
    {
        Region region = m_Region;
        region.minX = m_Region.CenterX();
        region.maxY = m_Region.CenterY();

        SE = new RTreeNode(region,m_MaxDepth-1);
    }

    if (!NW)
    {
        Region region = m_Region;
        region.maxX = m_Region.CenterX();
        region.minY = m_Region.CenterY();

        NW = new RTreeNode(region,m_MaxDepth-1);
    }

    if (!SW)
    {
        Region region = m_Region;
        region.maxX = m_Region.CenterX();
        region.maxY = m_Region.CenterY();

        SW = new RTreeNode(region,m_MaxDepth-1);
    }

    if (!NE || !SE || !NW || !SW)
        return false;

    return true;
}

RTree::RTree ()
    : m_Root(0), m_MaxDepth(0)
{
    m_Region.minX = 0;
    m_Region.minY = 0;
    m_Region.maxX = 0;
    m_Region.maxY = 0;
}

RTree::RTree (const Region &region, uint16_t depth)
    : m_Root(0), m_Region(region), m_MaxDepth(depth)
{
}

RTree::~RTree()
{
    if (m_Root)
    {
        delete m_Root;
        m_Root = 0;
    }
}

bool RTree::InsertRect (const Region &region)
{
    if (!m_Region.Contains(region))
        return false;

    if (region.minX > region.maxX || region.minY > region.maxY)
        return false;

    if (!m_Root)
    {
        m_Root = new RTreeNode(m_Region,m_MaxDepth);

        if (!m_Root)
            return false;
    }

    return m_Root->InsertRect(region);
}

bool RTree::Collides (const float x, const float y, size_t *rectID) const
{
    if (!m_Region.Contains(x,y) || !m_Root)
        return false;

    return m_Root->Collides(x,y,rectID);
}

void RTree::Clear()
{
    if (m_Root)
    {
        delete m_Root;
        m_Root = 0;
    }
}
