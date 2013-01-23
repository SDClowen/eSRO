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

#include "navigation_cell.hpp"
#include <cassert>
#include <cmath>

//#define DEBUG_PRINT

#if defined DEBUG_PRINT
#include <iostream>
#endif

NavigationLink::NavigationLink ()
    :   type (-1),
        edgeSrc(RECT_EDGE_NONE),
        edgeDest(RECT_EDGE_NONE),
        cellSrc(-1),
        cellDest(-1),
        zoneSrc(0),
        zoneDest(0)
{
}

std::pair<NavigationCell::PATH_RESULT,NavigationCell::link_const_iterator>
NavigationCell::ClassifyPath (const size_t prevCell, const size_t curCell, const Line &path, bool dirVDown,
    bool dirHLeft, bool dirPrefH, Coord *inter) const
{
    if (rect.Contains(path.x1,path.y1))
    {
        if (!rect.Contains(path.x2,path.y2))
        {
            /// CHECK EDGE INTERSECTION.
            double xdiff = 1.7e308;
            double ydiff = 1.7e308;

            Point point(-1,-1);

            for (int i = 0; i < 4; ++i)
            {
                Line edge;

                switch(i)
                {
                case RECT_EDGE_TOP:
                    edge.x1 = rect.minX;
                    edge.x2 = rect.maxX;
                    edge.y1 = edge.y2 = rect.maxY;
                    break;
                case RECT_EDGE_BOTTOM:
                    edge.x1 = rect.minX;
                    edge.x2 = rect.maxX;
                    edge.y1 = edge.y2 = rect.minY;
                    break;
                case RECT_EDGE_LEFT:
                    edge.y1 = rect.minY;
                    edge.y2 = rect.maxY;
                    edge.x1 = edge.x2 = rect.minX;
                    break;
                case RECT_EDGE_RIGHT:
                    edge.y1 = rect.minY;
                    edge.y2 = rect.maxY;
                    edge.x1 = edge.x2 = rect.maxX;
                    break;
                }

                Point px;
                Line::LINE_CLASSIFICATION ret = edge.Intersection(path,&px);

                if (ret == Line::SEGMENTS_INTERSECT)
                {
                    double ddx = std::abs(px.X-path.x2);
                    double ddy = std::abs(px.Y-path.y2);

                    if (dirPrefH)
                    {
                        if (ddx < xdiff)
                        {
                            xdiff = ddx;
                            ydiff = ddy;
                            point = px;
                        }
                        else if (ddx == xdiff)
                        {
                            if (ddy < ydiff)
                            {
                                xdiff = ddx;
                                ydiff = ddy;
                                point = px;
                            }
                        }
                    }
                    else
                    {
                        if (ddy < ydiff)
                        {
                            xdiff = ddx;
                            ydiff = ddy;
                            point = px;
                        }
                        else if (ddy == ydiff)
                        {
                            if (ddx < xdiff)
                            {
                                xdiff = ddx;
                                ydiff = ddy;
                                point = px;
                            }
                        }
                    }

#if defined DEBUG_PRINT
                    std::cout << "DirPrefH = " << dirPrefH << std::endl;
                    std::cout << "Edge = " << i << " X = " << px.X << " Y = " << px.Y << std::endl;

                    std::cout << "dx = " << std::abs(px.X-path.x2) << " dy = " << std::abs(px.Y-path.y2) << std::endl;
#endif
                }
            }

            if (inter)
            {
                inter->set_x(point.X);
                inter->set_y(point.Y);

#if defined DEBUG_PRINT
                std::cout << "Intersection Point = " << inter->get_x() << " " << inter->get_y() << std::endl;
                std::cout << "xDiff = " << xdiff << " yDiff = " << ydiff << std::endl;
#endif
            }

            link_const_iterator prev = links.end();

            for (link_const_iterator it = links.begin(); it != links.end(); ++it)
            {
                /// SKIP SOURCE CELL EDGES
                if (it->type != NavigationLink::LINK_ZONE)
                {
                    if (it->cellSrc == prevCell || it->cellDest == prevCell)
                        continue;
                }

                /// SKIP EDGES THAT DONT CONTAIN THE INTERSECTION POINT
                if (!it->edge.Contains(point))
                    continue;

#if defined DEBUG_PRINT
                std::cout << "Link #" << it-links.begin() << std::endl;
                std::cout << "\tP1 = (" << it->edge.x1 << "," << it->edge.y1 << ")" << std::endl;
                std::cout << "\tP2 = (" << it->edge.x2 << "," << it->edge.y2 << ")" << std::endl;
                std::cout << "\tType = " << (int)it->type << std::endl;
                std::cout << "\tEdgeSrc = " << (int)it->edgeSrc << std::endl;
                std::cout << "\tEdgeDest = " << (int)it->edgeDest << std::endl;
                std::cout << "\tCellSrc = " << it->cellSrc << std::endl;
                std::cout << "\tCellDest = " << it->cellDest << std::endl;
                std::cout << "\tZoneSrc = " << it->zoneSrc << std::endl;
                std::cout << "\tZoneDest = " << it->zoneDest << std::endl;
                std::cout << std::endl;
#endif

                /// IF THE INTERSECTION IS AN ENDPOINT OF THE EDGE WE NEED TO CHECK THE REST OF THEM
                /// TO SELECT THE BEST ONE ACCORDING THE MOTION PATH DIRECTION

                if (it->edge.isEndpoint(point))
                {
#if defined DEBUG_PRINT
                    std::cout << "Potencially more than 1 intersection." << std::endl;
#endif

                    if (prev != links.end())
                    {
#if defined DEBUG_PRINT
                        std::cout << "Solving edge conflict." << std::endl;
#endif

                        /// CHOOSE THE EDGE WITH BEST PREFERENCE
                        /// IF THE EDGES ARE IN THE SAME ORIENTATION (HORIZONTAL, VERTICAL) <XXX>
                        /// IF THE EDGES ARE DIFFERENT ORIENTATIONS CHOOSE THE ONE IN THE PREFERED DIRECTION

                        if (it->edgeSrc%2 == prev->edgeSrc%2)
                        {
#if defined DEBUG_PRINT
                            std::cout << "Edge of the same orientation." << std::endl;
#endif

                            int pv;
                            int cv;

                            if (it->edgeSrc%2)
                            {
#if defined DEBUG_PRINT
                                std::cout << "Vertical Edges..." << std::endl;
#endif

                                pv = (prev->edge.y1 == point.Y) ? prev->edge.y2 : prev->edge.y1;
                                cv = (it->edge.y1 == point.Y) ? it->edge.y2 : it->edge.y1;

#if defined DEBUG_PRINT
                                std::cout << "dirVDown = " << dirVDown << std::endl;
                                std::cout << "Prev Y = " << pv << std::endl;
                                std::cout << "Cur Y = " << cv << std::endl;
#endif

                                if (dirVDown)
                                {
                                    if (cv < pv)
                                        prev = it;
                                }
                                else
                                {
                                    if (cv > pv)
                                        prev = it;
                                }
                            }
                            else
                            {
#if defined DEBUG_PRINT
                                std::cout << "Horizontal Edges..." << std::endl;
#endif

                                pv = (prev->edge.x1 == point.X) ? prev->edge.x2 : prev->edge.x1;
                                cv = (it->edge.x1 == point.X) ? it->edge.x2 : it->edge.x1;

#if defined DEBUG_PRINT
                                std::cout << "dirHLeft = " << dirHLeft << std::endl;
                                std::cout << "Prev X = " << pv << std::endl;
                                std::cout << "Cur X = " << cv << std::endl;
#endif

                                if (dirHLeft)
                                {
                                    if (cv > pv)
                                        prev = it;
                                }
                                else
                                {
                                    if (cv < pv)
                                        prev = it;
                                }
                            }
                        }
                        else
                        {
#if defined DEBUG_PRINT
                            std::cout << "Edges of different orientation." << std::endl;
#endif

                            int pedge = (prev->cellSrc == curCell) ? prev->edgeSrc : prev->edgeDest;

#if defined DEBUG_PRINT
                            std::cout << "Prev Edge = " << pedge << std::endl;
#endif

                            if (prev->edgeSrc%2 == dirPrefH)
                            {
                                /// PREFERED DIRECTION VERTICAL
                                if (dirVDown)
                                    prev = (pedge == RECT_EDGE_BOTTOM) ? prev : it;
                                else
                                    prev = (pedge == RECT_EDGE_TOP) ? prev : it;
                            }
                            else
                            {
                                /// PREFERED DIRECTION HORIZONTAL
                                if (dirHLeft)
                                    prev = (pedge == RECT_EDGE_LEFT) ? prev : it;
                                else
                                    prev = (pedge == RECT_EDGE_RIGHT) ? prev : it;

                                prev = it;
                            }
                        }

#if defined DEBUG_PRINT
                        std::cout << "Selected Link #" << prev-links.begin() << std::endl;
#endif
                    }
                    else
                        prev = it;
                }
                else
                    return std::make_pair(it->zoneDest ? EXITING_ZONE : EXITING_CELL,it);

            }

            if (prev != links.end())
                return std::make_pair(prev->zoneDest ? EXITING_ZONE : EXITING_CELL,prev);

//            assert(0);
        }
        else
            return std::make_pair(ENDING_CELL,links.end());
    }

    return std::make_pair(NO_RELATIONSHIP,links.end());
}
