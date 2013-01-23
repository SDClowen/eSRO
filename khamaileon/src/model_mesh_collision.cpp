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

#include "model_mesh_collision.hpp"
#include "vector.hpp"
#include <cassert>
#include <cmath>

//#define DEBUG_PRINT

#if defined DEBUG_PRINT
#include <iostream>
#endif

CollisionLink::CollisionLink ()
    : vertexSrc(-1), vertexDest(-1), cellSrc(-1), cellDest(-1), flag(0), flag2(0)
{
}

size_t CollisionCell::GetLinkEdge (const uint8_t side) const
{
    size_t pos = static_cast<size_t>(-1);

    bool valid = false;
    for (size_t i = 0; i < Links.size(); ++i)
    {
        switch(side)
        {
        case EDGE_AB:
            if ((Links[i].vertexSrc == vertexA && Links[i].vertexDest == vertexB) ||
                (Links[i].vertexSrc == vertexB && Links[i].vertexDest == vertexA))
                valid = true;
            break;
        case EDGE_BC:
            if ((Links[i].vertexSrc == vertexB && Links[i].vertexDest == vertexC) ||
                (Links[i].vertexSrc == vertexC && Links[i].vertexDest == vertexB))
                valid = true;
            break;
        case EDGE_CA:
            if ((Links[i].vertexSrc == vertexC && Links[i].vertexDest == vertexA) ||
                (Links[i].vertexSrc == vertexA && Links[i].vertexDest == vertexC))
                valid = true;
            break;
        default:
            break;
        }

        if (valid)
        {
            pos = i;
            break;
        }
    }

    return pos;
}

void CollisionMesh::addVertex (const CollisionVertex &vertex)
{
    Vertices.push_back(vertex);
}

void CollisionMesh::addCell (const CollisionCell &cell)
{
    Cells.push_back(cell);
}

void CollisionMesh::addLink (const CollisionLink &link)
{
    assert(link.cellSrc < Cells.size());

    std::vector<CollisionCell>::iterator iter = Cells.begin()+link.cellSrc;

    iter->Links.push_back(link);

    if (link.cellDest != 0xFFFF)
    {
        assert(link.cellDest < Cells.size());

        iter = Cells.begin()+link.cellDest;

        iter->Links.push_back(link);
    }
}

double orientation (const CollisionVertex &A, const CollisionVertex &B, const CollisionVertex &C)
{
    return (A.x-C.x)*(B.y-C.y)-(A.y-C.y)*(B.x-C.x);
}

bool CollisionMesh::isPointInsideCell (const Coord &point, const nav_const_iterator &it) const
{
    int count = 0;
    double order = orientation(Vertices[it->vertexA],Vertices[it->vertexB],Vertices[it->vertexC]);

    for (int i = 0; i < 3; ++i)
    {
        Line edge;
        int vSrc;
        int vDest;

        switch(i)
        {
        case CollisionCell::EDGE_AB:
            vSrc = it->vertexA;
            vDest = it->vertexB;
            break;
        case CollisionCell::EDGE_BC:
            vSrc = it->vertexB;
            vDest = it->vertexC;
            break;
        case CollisionCell::EDGE_CA:
            vSrc = it->vertexC;
            vDest = it->vertexA;
            break;
        default:
            break;
        }

        if (order > 0)
            std::swap(vSrc,vDest);

        edge.x1 = Vertices[vSrc].x;
        edge.y1 = Vertices[vSrc].y;
        edge.x2 = Vertices[vDest].x;
        edge.y2 = Vertices[vDest].y;

        Line::POINT_CLASSIFICATION ret = edge.ClassifyPoint(Point(point.get_x(),point.get_y()));

        if (ret != Line::LEFT_SIDE)
            ++count;
    }

    return count == 3;
}

bool IntersectTriangle (const CollisionVertex &A, const CollisionVertex &B, const CollisionVertex &C,
    const Vector<3,double> &P , const Vector<3,double> &Q, double *t)
{
    Vector<3,double> ab,ac;

    double order = orientation(A,B,C);

    if (order < 0)
    {
        ab(0) = B.x - A.x;
        ab(1) = B.y - A.y;
        ab(2) = B.z - A.z;

        ac(0) = C.x - A.x;
        ac(1) = C.y - A.y;
        ac(2) = C.z - A.z;
    }
    else if (order > 0)
    {
        ab(0) = C.x - A.x;
        ab(1) = C.y - A.y;
        ab(2) = C.z - A.z;

        ac(0) = B.x - A.x;
        ac(1) = B.y - A.y;
        ac(2) = B.z - A.z;
    }
    else
        return false;

    Vector<3,double> qp = Q - P;

    Vector<3,double> n = cross_product(ab,ac);

    double d = dot_product(qp,n);

    if (abs(d) <= 1e-5f)
        return false;

    Vector<3,double> ap;

    ap(0) = A.x-P(0);
    ap(1) = A.y-P(1);
    ap(2) = A.z-P(2);

    *t = dot_product(ap,n);

    if (*t < 0.0f)
        return false;

    // Compute barycentric coordinate components and test if within bounds
    Vector<3,double> e = cross_product(qp, ap);

    float v = dot_product(ac,e);

    if (v < 0.0f || v > d) return false;

    float w = -dot_product(ab,e);

    if (w < 0.0f || v + w > d) return false;

//    // Segment/ray intersects triangle. Perform delayed division and
//    // compute the last barycentric coordinate component
    float ood = 1.0f / d;
    *t *= ood;
    v *= ood;
    w *= ood;

    *t = P(2)+(*t)*qp(2);

    return true;
}

bool IntersectTrg (const CollisionVertex &A, const CollisionVertex &B, const CollisionVertex &C,
    double x, double z, double *t)
{
    Vector<3,double> q,p;

    p(0) = x;
    p(1) = z;
    p(2) = 1e4;

    q(0) = x;
    q(1) = z;
    q(2) = -1e4;

    return IntersectTriangle(A,B,C,p,q,t);
}

float CollisionMesh::getHeightAt (float x, float z) const
{
    Vector<3,double> q,p;

    p(0) = x;
    p(1) = z;
    p(2) = 1e4;

    q(0) = x;
    q(1) = z;
    q(2) = -1e4;

    double h = 0.0f;

    for (std::vector<CollisionCell>::const_iterator it = Cells.begin(); it != Cells.end(); ++it)
    {
        if (IntersectTriangle(Vertices[it->vertexA],Vertices[it->vertexB],Vertices[it->vertexC],p,q,&h))
            return h;
    }

    return -1e4;
}

CollisionMesh::nav_const_iterator CollisionMesh::findClosestCell(const Line &path) const
{
    double ClosestDistance = -3.4E+38f;
    std::vector<CollisionCell>::const_iterator closestCell = Cells.end();

#if defined DEBUG_PRINT
    std::cout << "Src = " << path.x1 << " " << path.y1 << std::endl;
    std::cout << "Dest = " << path.x2 << " " << path.y2 << std::endl;
#endif

    for (std::vector<CollisionCell>::const_iterator it = Cells.begin(); it != Cells.end(); ++it)
    {

#if defined DEBUG_PRINT
        std::cout << "Cell #" << it-Cells.begin() << std::endl;
        std::cout << "VertexA = " << Vertices[it->vertexA].x << "," << Vertices[it->vertexA].z
                << "," << Vertices[it->vertexA].y << std::endl;
        std::cout << "VertexB = " << Vertices[it->vertexB].x << "," << Vertices[it->vertexB].z
                << "," << Vertices[it->vertexB].y << std::endl;
        std::cout << "VertexC = " << Vertices[it->vertexC].x << "," << Vertices[it->vertexC].z
                << "," << Vertices[it->vertexC].y << std::endl;
        std::cout << "Orientation = " << orientation(Vertices[it->vertexA],Vertices[it->vertexB],Vertices[it->vertexC]) << std::endl;
#endif

        Coord inter;
        std::pair<PATH_RESULT,uint8_t> ret = classifyPathToCell(path,it,&inter);

        if (ret.first == PATH_CELL_EXIT)
        {
            double d2 = std::pow(inter.get_x()-path.x1,2)+std::pow(inter.get_y()-path.y1,2);

#if defined DEBUG_PRINT
            std::cout << "D² = " << d2 << std::endl;
#endif

            if (d2 > ClosestDistance)
            {
                ClosestDistance = d2;
                closestCell = it;
            }
        }
        else if (ret.first == PATH_CELL_END)
            return it;
    }

    return closestCell;
}

std::pair<CollisionMesh::PATH_RESULT,uint8_t> CollisionMesh::classifyPathToCell (const Line &path,
    const nav_const_iterator &it, Coord *intersection) const
{
#if defined DEBUG_PRINT
    std::cout << "\tCollisionModel::classifyPathToCell()" << std::endl;
    std::cout << "\tPath Src = " << path.x1 << " " << path.y1 << std::endl;
    std::cout << "\tPath Dest = " << path.x2 << " " << path.y2 << std::endl;
#endif

    Line edge;
    int vSrc;
    int vDest;
    int InteriorCount = 0;

    double order = orientation(Vertices[it->vertexA],Vertices[it->vertexB],Vertices[it->vertexC]);

    // Check our MotionPath against each of the three cell walls
    for (int i=0; i<3; ++i)
    {
        switch(i)
        {
        case CollisionCell::EDGE_AB:
            vSrc = it->vertexA;
            vDest = it->vertexB;
            break;
        case CollisionCell::EDGE_BC:
            vSrc = it->vertexB;
            vDest = it->vertexC;
            break;
        case CollisionCell::EDGE_CA:
            vSrc = it->vertexC;
            vDest = it->vertexA;
            break;
        default:
            break;
        }

        if (order > 0)
            std::swap(vSrc,vDest);

        edge.x1 = Vertices[vSrc].x;
        edge.y1 = Vertices[vSrc].y;
        edge.z1 = Vertices[vSrc].z;
        edge.x2 = Vertices[vDest].x;
        edge.y2 = Vertices[vDest].y;
        edge.z2 = Vertices[vDest].z;

        // Classify the MotionPath endpoints as being either ON_LINE,
        // or to its LEFT_SIDE or RIGHT_SIDE.
        // Since our triangle vertices are in clockwise order,
        // we know that points  to the right of each line are inside the cell.
        // Points to the left are outside.
        // We do this test using the ClassifyPoint function of Line2D

        // If the destination endpoint of the MotionPath
        // is Not on the right side of this wall...
        if (edge.ClassifyPoint(Point(path.x2,path.y2)) != Line::RIGHT_SIDE)
        {
            // ..and the starting endpoint of the MotionPath
            // is Not on the left side of this wall...
            if (edge.ClassifyPoint(Point(path.x1,path.y1)) != Line::LEFT_SIDE)
            {
                Point pt;
                // Check to see if we intersect the wall
                // using the Intersection function of Line2D
                Line::LINE_CLASSIFICATION IntersectResult = path.Intersection(edge,&pt);

                if (IntersectResult == Line::SEGMENTS_INTERSECT)
                {
                    if (intersection)
                    {
                        intersection->set_x(pt.X);
                        intersection->set_y(pt.Y);
                        
                        double z;
                        
//                        if (!IntersectTrg(Vertices[it->vertexA],Vertices[it->vertexB],Vertices[it->vertexC],pt.X,pt.Y,&z))
                            z = (edge.z1+edge.z2)*0.5;

                        intersection->set_z(z);
                    }

                    return std::make_pair(PATH_CELL_EXIT,i);
                }
            }
        }
        else
        {
            // The destination endpoint of the MotionPath is on the right side.
            // Increment our InteriorCount so we'll know how many walls we were
            // to the right of.
            InteriorCount++;
        }
    }

    // An InteriorCount of 3 means the destination endpoint of the MotionPath
    // was on the right side of all walls in the cell.
    // That means it is located within this triangle, and this is our ending cell.
    if (InteriorCount == 3)
        return std::make_pair(PATH_CELL_END,255);

    // We only reach here is if the MotionPath does not intersect the cell at all.
    return std::make_pair(PATH_NO_RELATIONSHIP,255);
}

std::pair<bool,uint8_t> CollisionMesh::resolveMotion (const Line &path, Coord *intersection) const
{
    assert(intersection);

    nav_const_iterator iter = findClosestCell(Line(path.x2,path.y2,path.z2,path.x1,path.y1,path.z1));

    if (iter == Cells.end())
        return std::make_pair(false,CollisionLink::LINK_NONE);

#if defined DEBUG_PRINT
    std::cout << "\n\n*********************\n\nCollisionMesh::resolveMotion()\n";
#endif

    size_t curCell = iter - Cells.begin();
    size_t prevCell = static_cast<size_t>(-1);

#if defined DEBUG_PRINT
    std::cout << "Beg Cell = " << curCell << std::endl;
    std::cout << "Path Src = " << path.x1 << " " << path.y1 << std::endl;
    std::cout << "Path Dest = " << path.x2 << " " << path.y2 << std::endl;
#endif

    /// CHECK IF WE CAN TRANSVERSE THE SOURCE EDGE OF THE CLOSEST CELL.
    std::pair<PATH_RESULT,uint8_t> ret = classifyPathToCell(Line(path.x2,path.y2,path.z2,path.x1,path.y1,path.z1),
        iter,intersection);

    if(ret.first == PATH_CELL_EXIT)
    {
//        std::cout << "Side =" << (int)cret.second << std::endl;
//        std::cout << "X = " << pA.get_x() << " Y = " << pA.get_y() << std::endl;

        size_t lpos = iter->GetLinkEdge(ret.second);
//        std::cout << "Link = " << lpos << std::endl;

        if (iter->Links[lpos].cellDest == 0xFFFF)
        {
            switch(iter->Links[lpos].flag)
            {
            case CollisionLink::LINK_OUTER_BORDER_WALL:
            case CollisionLink::LINK_INNER_WALL:
            case CollisionLink::LINK_AERIAL_OUTER_BORDER_WALL:
                {
#if defined DEBUG_PRINT
                std::cout << "Collided againts a wall." << std::endl;
                std::cout << "Intersection = " << intersection->get_x() << " " << intersection->get_y()
                    << " " << intersection->get_z() << std::endl;
#endif
    
                    /// ASSURE WE DONT STAND NEXT TO THE WALL!!
                    Vector<2,double> dir;
                    dir(0) = intersection->get_x() - path.x1;
                    dir(1) = intersection->get_y() - path.y1;

                    dir *= 0.95;

                    intersection->set_x(dir(0)+path.x1);
                    intersection->set_y(dir(1)+path.y1);

                    if (!isPointInsideCell(*intersection,iter))
                        intersection->set_z(-1e4);

                    return std::make_pair(true,static_cast<CollisionLink::TYPE>(iter->Links[lpos].flag));
                }
            default:
                break;
            }
        }
    }

	CollisionLink::TYPE link_type;

	if (iter->Links[0].flag >= CollisionLink::LINK_AERIAL_OUTER_BORDER_WALL)
		link_type = CollisionLink::LINK_AERIAL_INNER_BORDER_PASSABLE;
	else
		link_type = CollisionLink::LINK_INNER_BORDER;

    ret = std::make_pair(PATH_NO_RELATIONSHIP,255);

    while (ret.first != PATH_CELL_END)
    {
        iter = Cells.begin()+curCell;

        assert(iter != Cells.end());

        /// CHECK IF WE CAN MOVE TO OTHER CELLS IF NOT JUST SET END POINT AS SOURCE
        if (iter->Links.empty())
            break;

#if defined DEBUG_PRINT
        std::cout << "Current Cell = " << curCell << std::endl;
#endif

        ret = classifyPathToCell(path,iter,intersection);

//        std::cout << "Result = " << ret.first << " Side = " << (int)ret.second << std::endl;

        if (ret.first == PATH_CELL_EXIT)
        {
            size_t lpos = iter->GetLinkEdge(ret.second);
//            std::cout << "Link = " << lpos << std::endl;

            link_type = static_cast<CollisionLink::TYPE>(iter->Links[lpos].flag);

#if defined DEBUG_PRINT
            std::cout << "vSrc = " << iter->Links[lpos].vertexSrc << " vDest = "
                << iter->Links[lpos].vertexDest << std::endl;
            std::cout << "Intersection = " << intersection->get_x() << " " << intersection->get_y() << std::endl;
#endif

            prevCell = curCell;
            curCell = (iter->Links[lpos].cellSrc == prevCell) ? iter->Links[lpos].cellDest : iter->Links[lpos].cellSrc;

            if (curCell >= Cells.size() || link_type == CollisionLink::LINK_INNER_WALL
                || link_type == CollisionLink::LINK_AERIAL_OUTER_BORDER_WALL)
            {
                /// ASSURE WE DONT STAND INSIDE THE OBJECT!!
                Vector<2,double> dir;
                dir(0) = intersection->get_x() - path.x1;
                dir(1) = intersection->get_y() - path.y1;

                dir *= 0.95;

                intersection->set_x(dir(0)+path.x1);
                intersection->set_y(dir(1)+path.y1);

                break;
            }

            assert(prevCell != curCell);
        }
        else if (ret.first == PATH_CELL_END)
        {
            intersection->set_x(path.x2);
            intersection->set_y(path.y2);
            intersection->set_z((Vertices[iter->vertexA].z+Vertices[iter->vertexB].z+Vertices[iter->vertexC].z)/3);
        }
        else
        {
            assert(0);
        }
    }

    return std::make_pair(true,link_type);
}

void CollisionMesh::visitVertices (boost::function<void(const CollisionVertex &)>func) const
{
    std::for_each(Vertices.begin(),Vertices.end(),func);
}

void CollisionMesh::visitCells (boost::function<void(const CollisionCell &)>func) const
{
    std::for_each(Cells.begin(),Cells.end(),func);
}

const std::vector<CollisionVertex>& CollisionMesh::getVertices () const
{
    return Vertices;
}

const std::vector<CollisionCell>& CollisionMesh::getCells () const
{
    return Cells;
}

void CollisionMesh::clear()
{
    Vertices.clear();
    Cells.clear();
}
