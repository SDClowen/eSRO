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

#include "navigation_mesh.hpp"
#include "rtree.hpp"
#include "model.hpp"
#include "model_manager.hpp"
#include "model_mesh_collision.hpp"

#include <fstream>
#include <cassert>
#include <string.h>

#define DEBUG_PRINT

#if defined DEBUG_PRINT
#include <iostream>
#endif

NavigationMesh::NavigationMesh (int16_t ID)
    : refID(ID), m_Texturemap(0), m_Tree(0)
{
}

NavigationMesh::~NavigationMesh ()
{
    if (m_Texturemap)
        delete m_Texturemap;

    if (m_Tree)
        delete m_Tree;
}

bool NavigationMesh::buildNavigationMesh()
{
    if (Cells.empty())
        return false;

    m_Tree = new SR::RTree(SR::Region(0,0,1920,1920),0);

    if (!m_Tree)
        return false;

    for (std::vector<NavigationCell>::const_iterator it = Cells.begin(); it != Cells.end(); ++it)
    {
        SR::Region rect(it->rect.minX,it->rect.minY,it->rect.maxX,it->rect.maxY);
        rect.refID = it-Cells.begin();

        if (!m_Tree->InsertRect(rect))
            return false;
    }

    return true;
}

bool NavigationMesh::Read (const std::string &filename, READ_MODE mode)
{
    std::ifstream file(filename.c_str());

    if (!file)
        return false;

    char tag[13] = { 0 };
    file.read(tag,12);

    if (strcmp(tag,"JMXVNVM 1000") != 0)
        return false;

    uint32_t size = 0;
    file.read((char*)&size,sizeof(int16_t));

    NavigationEntity ent;
    for (size_t i = 0; i < size; ++i)
    {
        file.read((char*)&ent.refID,sizeof(uint32_t));
        file.read((char*)&ent.x,4);
        file.read((char*)&ent.y,4);
        file.read((char*)&ent.z,4);
        file.read((char*)&ent.uk2,2);
        file.read((char*)&ent.angle,4);
        file.read((char*)&ent.uk4,2);
        file.read((char*)&ent.uk5,2);
        file.read((char*)&ent.uk6,2);
        file.read((char*)&ent.zoneID,sizeof(int16_t));

        uint16_t extra_size = 0;
        file.read((char*)&extra_size,sizeof(uint16_t));

        std::pair<uint32_t,uint16_t> extra;
        for ( int j = 0; j < extra_size; ++j)
        {
            file.read((char*)&extra.first,sizeof(uint32_t));
            file.read((char*)&extra.second,sizeof(uint16_t));

            ent.extra.push_back(extra);
        }

        if (mode & MODE_NAV_MESH)
            Entities.push_back(ent);

        ent.extra.clear();
    }

    file.read((char*)&size,sizeof(uint32_t));
    file.read((char*)&EntitiesExtra,sizeof(uint32_t));

    Cells.reserve(size);

    NavigationCell cell;

    for (size_t i = 0; i < size; ++i)
    {
        file.read((char*)&cell.rect.minX,4);
        file.read((char*)&cell.rect.minY,4);
        file.read((char*)&cell.rect.maxX,4);
        file.read((char*)&cell.rect.maxY,4);

        uint16_t entID = 0;
        uint8_t extra_size = 0;

        file.read((char*)&extra_size,sizeof(uint8_t));

        for (int j = 0; j < extra_size; ++j)
        {
            file.read((char*)&entID,sizeof(uint16_t));
            cell.entities.push_back(entID);
        }

        if (mode & MODE_NAV_MESH)
            Cells.push_back(cell);

        cell.entities.clear();
    }

    assert(Cells.size());

    NavigationLink link;
    std::vector<NavigationCell>::iterator citer;

    for (int count = 0; count < 2; ++count)
    {
        file.read((char*)&size,sizeof(uint32_t));

        float fx;

        for (size_t i = 0; i < size; ++i)
        {
            file.read((char*)&fx,4);
            link.edge.x1 = fx;

            file.read((char*)&fx,4);
            link.edge.y1 = fx;

            file.read((char*)&fx,4);
            link.edge.x2 = fx;

            file.read((char*)&fx,4);
            link.edge.y2 = fx;

            file.read((char*)&link.type,sizeof(uint8_t));
            file.read((char*)&link.edgeSrc,sizeof(uint8_t));
            file.read((char*)&link.edgeDest,sizeof(uint8_t));
            file.read((char*)&link.cellSrc,sizeof(uint16_t));
            file.read((char*)&link.cellDest,sizeof(uint16_t));

            if (link.type == NavigationLink::LINK_ZONE)
            {
                file.read((char*)&link.zoneSrc,sizeof(int16_t));
                file.read((char*)&link.zoneDest,sizeof(int16_t));
            }

            if (mode & MODE_NAV_MESH)
            {
                citer = Cells.begin()+link.cellSrc;

                if (citer == Cells.end())
                    return false;

                citer->links.push_back(link);

                if (link.type != NavigationLink::LINK_ZONE && link.cellDest != 0xFFFF)
                {
                    citer = Cells.begin()+link.cellDest;

                    if (citer == Cells.end())
                        return false;

                    citer->links.push_back(link);
                }
            }

            link.zoneDest = 0;
            link.zoneSrc = 0;
        }
    }

    if (mode & MODE_TEXTURE)
    {
        if (!m_Texturemap)
        {
            m_Texturemap = new TTextureMapEntry[9216];

            if (!m_Texturemap)
                return false;
        }

        file.read((char*)m_Texturemap,0x12000);
    }
    else
    {
        if (m_Texturemap)
        {
            delete m_Texturemap;
            m_Texturemap = 0;
        }

        file.seekg(0x12000,std::ios_base::cur);
    }

    file.read((char*)heightmap,0x9304);

//    file.read((char*)&last3,36);
//    file.read((char*)&last4,36*4);

    return true;
}

int16_t NavigationMesh::ID() const
{
    return refID;
}

float NavigationMesh::minX () const
{
    return 0;
}

float NavigationMesh::maxX () const
{
    return 1920;
}

float NavigationMesh::minZ () const
{
    return 0;
}

float NavigationMesh::maxZ () const
{
    return 1920;
}

int16_t NavigationMesh::Width() const
{
    return maxZ()-minZ();
}

int16_t NavigationMesh::Length() const
{
    return maxX()-minX();
}

float NavigationMesh::getHeightAt (int x, int z) const
{
    assert(x < HEIGHTMAP_SIZE && z < HEIGHTMAP_SIZE);

    return heightmap[x+HEIGHTMAP_SIZE*z];
}

float NavigationMesh::getObjectHeight (float x, float z, const boost::shared_ptr<ModelManager> &modelMgr) const
{
    size_t currentCell = findClosestCell(x,z);

    if (currentCell == npos)
        return -1e4;

    std::vector<NavigationCell>::const_iterator citer = Cells.begin() + currentCell;

    entity_const_iterator eit;
    std::vector<uint32_t>::const_iterator it;

    for (it = citer->entities.begin(); it != citer->entities.end(); ++it)
    {
        assert(*it < Entities.size());

        eit = Entities.begin() + *it;

        boost::shared_ptr<Model> model = modelMgr->FindModel(eit->refID);

        assert(model);

        /// TRANSFORM MOTION PATH INTO OBJECT COORDINATES
        /// TRANSLATE PATH INTO ORIGIN, ROTATE PATH CCW FROM PATH CENTER POINT OF VIEW AND TRANSLATE
        /// NOTE: OBJECTS ARE CENTERED IN THE IN ORIGIN.

        Point point(x,z);

        point.X -= eit->x;
        point.Y -= eit->z;

        point = eit->TransformIntoObjectCoordinates(point);

        float height = model->getHeightAt(point.X,point.Y);

        if (height != -1e4)
            return height+eit->y;
    }

    return -1e4;
}

uint16_t NavigationMesh::getTextureAt (int x, int z) const
{
    assert(x < TEXTURE_SIZE && z < TEXTURE_SIZE);

    if (!m_Texturemap)
        return 0;

    return m_Texturemap[x+TEXTURE_SIZE*z].refID;
}

size_t NavigationMesh::findClosestCell (float x, float z) const
{
    size_t cell = npos;

    m_Tree->Collides(x,z,&cell);

    return cell;
}

bool NavigationMesh::resolveMotion (const Coord &beg, const Coord &end,
    const boost::shared_ptr<ModelManager> &modelMgr, Coord *endPos) const
{
#if defined DEBUG_PRINT
    std::cout << "NavigationMesh::resolveMotion()" << std::endl;
#endif

    typedef std::pair<NavigationCell::PATH_RESULT,NavigationCell::link_const_iterator> result_type;

    assert(beg.get_zone() == end.get_zone() && endPos);

    if (beg == end)
    {
        *endPos = beg;
        return false;
    }

    size_t prevCell = npos;
    size_t currentCell = findClosestCell(beg.get_x(),beg.get_y());

    if (currentCell == npos)
        return false;

    Line path(beg.get_x(),beg.get_y(),beg.get_z(),end.get_x(),end.get_y(),end.get_z());

    bool dirVDown = (path.y1 > path.y2);
    bool dirHLeft = (path.x1 < path.x2);
    bool dirPrefH = (std::abs(path.x2-path.x1) >= std::abs(path.y2-path.y1));

    result_type ret;
    ret.first = NavigationCell::NO_RELATIONSHIP;

    Coord intersection;
    bool keepMov = true;

#if defined DEBUG_PRINT
    std::cout << "Beggining Cell = " << currentCell << std::endl;
#endif

    while (ret.first != NavigationCell::ENDING_CELL && ret.first != NavigationCell::EXITING_ZONE &&
           (path.x1 != path.x2 || path.y1 != path.y2))
    {
        if (currentCell >= Cells.size())
        {
#if defined DEBUG_PRINT
            std::cout << "We hitted a wall." << std::endl;
#endif
            keepMov = false;
            break;
        }

        std::vector<NavigationCell>::const_iterator citer = Cells.begin()+currentCell;

        assert(citer != Cells.end() && citer->rect.Contains(path.x1,path.y1));

#if defined DEBUG_PRINT
        std::cout << "Previous Cell = " << prevCell << std::endl;
        std::cout << "Current Cell = " << currentCell << std::endl;
        std::cout << "Rect = (" << citer->rect.minX << "," << citer->rect.minY
                << ")\t(" << citer->rect.maxX << "," << citer->rect.maxY << ")" << std::endl;
        std::cout << "P1 = (" << path.x1 << "," << path.y1 << ")" << std::endl;
        std::cout << "P2 = (" << path.x2 << "," << path.y2 << ")" << std::endl;
        std::cout << "# Links = " << citer->links.size() << std::endl;
#endif

        /// CHECK IF WE COLLIDE WITH A ENTITY
        bool throughObj = false;
        bool endObj = false;

        if (modelMgr)
        {
            std::vector<uint32_t>::const_iterator it;

            for (it = citer->entities.begin(); it != citer->entities.end(); ++it)
            {
                assert(*it < Entities.size());

                entity_const_iterator eit = Entities.begin() + *it;

                boost::shared_ptr<Model> model = modelMgr->FindModel(eit->refID);

                assert(model);

#if defined DEBUG_PRINT
                std::cout << "Testing againts model = " << model->getName() << std::endl;
#endif

                /// TRANSFORM MOTION PATH INTO OBJECT COORDINATES
                /// TRANSLATE PATH INTO ORIGIN, ROTATE PATH CCW FROM PATH CENTER POINT OF VIEW AND TRANSLATE
                /// NOTE: OBJECTS ARE CENTERED IN THE IN ORIGIN.

                Line obj_path = path;

    //            std::cout << "Initial Path: X = " << obj_path.x1 << " Y = " << obj_path.y1 << " Z = " << obj_path.z1
    //                << " X = " << obj_path.x2 << " Y = " << obj_path.y2 << " Z = " << obj_path.z2 << std::endl;

                obj_path.x1 -= eit->x;
                obj_path.y1 -= eit->z;
                obj_path.z1 -= eit->y;
                obj_path.x2 -= eit->x;
                obj_path.y2 -= eit->z;
                obj_path.z2 -= eit->y;

                obj_path = eit->TransformIntoObjectCoordinates(obj_path);

    //            std::cout << "Transformed: X = " << obj_path.x1 << " Y = " << obj_path.y1 << " Z = " << obj_path.z1
    //                << " X = " << obj_path.x2 << " Y = " << obj_path.y2 << " Z = " << obj_path.z2 << std::endl;

                std::pair<bool,uint8_t> ret = model->resolveMotion(obj_path,&intersection);

                if (ret.first)
                {
                    switch(ret.second)
                    {
                    case CollisionLink::LINK_OUTER_BORDER_PASSABLE:
#if defined DEBUG_PRINT
                        std::cout << "Outer border passable." << std::endl;
#endif
                        throughObj = true;
                        break;
                    case CollisionLink::LINK_OUTER_BORDER_WALL:
#if defined DEBUG_PRINT
                        std::cout << "Outer border wall." << std::endl;
#endif

                        if (intersection.get_z() == -1e4)
                        {
#if defined DEBUG_PRINT
                            std::cout << "Moving below." << std::endl;
#endif
                            intersection.set_z(obj_path.z1);
                        }

                        endObj = true;
                        break;
                    case CollisionLink::LINK_INNER_BORDER:
#if defined DEBUG_PRINT
                        std::cout << "Inner border passable." << std::endl;
#endif
						if (intersection.get_z() > obj_path.z1)
						{
#if defined DEBUG_PRINT
							std::cout << "Moving below." << std::endl;
#endif
							intersection.set_z(obj_path.z1);
						}

                        endObj = true;
                        break;
                    case CollisionLink::LINK_INNER_WALL:
#if defined DEBUG_PRINT
                        std::cout << "Inner border wall." << std::endl;
#endif
                        endObj = true;
                        break;
                    case CollisionLink::LINK_AERIAL_OUTER_BORDER_WALL:
#if defined DEBUG_PRINT
                        std::cout << "Outer aerial border wall." << std::endl;
                        std::cout << "Object Z = " << intersection.get_z() << std::endl;
                        std::cout << "Path Z = " << obj_path.z1 << std::endl;
#endif

                        if (intersection.get_z() > obj_path.z1)
                        {
#if defined DEBUG_PRINT
                            std::cout << "Moving below." << std::endl;
#endif
                        }
                        else if (intersection.get_z() == -1e4) /// WE COLLIDED FROM THE OUTSIDE
                        {
                            intersection.set_z(obj_path.z1);
                        }
                        else
                        {
#if defined DEBUG_PRINT
                            std::cout << "Moving above." << std::endl;
#endif
                            endObj = true;
                        }
                        break;
                    case CollisionLink::LINK_AERIAL_INNER_BORDER_PASSABLE:
#if defined DEBUG_PRINT
                        std::cout << "Outer aerial border passable." << std::endl;
                        std::cout << "Object Z = " << intersection.get_z() << std::endl;
                        std::cout << "Path Z = " << obj_path.z1 << std::endl;
#endif

                        if (intersection.get_z() > obj_path.z1)
                        {
#if defined DEBUG_PRINT
                            std::cout << "Moving below." << std::endl;
#endif
                        }
                        else
                        {
#if defined DEBUG_PRINT
                            std::cout << "Moving above." << std::endl;
#endif
                            endObj = true;
                        }
                        break;
                    case CollisionLink::LINK_UNKNOWN:
                        throughObj = true;
                        break;
                    default:
#if defined DEBUG_PRINT
                        std::cout << "Unknown.. Flag = " << (int)ret.second << std::endl;
#endif
                        break;
                    }

                    if (endObj || throughObj)
                    {
                        obj_path.x1 = intersection.get_x();
                        obj_path.y1 = intersection.get_y();
                        obj_path.z1 = intersection.get_z();
                    }

//                    std::cout << "End Path: X = " << obj_path.x1 << " Y = " << obj_path.y1
//                        << " X = " << obj_path.x2 << " Y = " << obj_path.y2 << std::endl;

                    obj_path = eit->TransformIntoWorldCoordinates(obj_path);

                    obj_path.x1 += eit->x;
                    obj_path.y1 += eit->z;
                    obj_path.z1 += eit->y;
                    obj_path.x2 += eit->x;
                    obj_path.y2 += eit->z;
                    obj_path.z2 += eit->y;

//                    std::cout << "End Path: X = " << obj_path.x1 << " Y = " << obj_path.y1
//                        << " X = " << obj_path.x2 << " Y = " << obj_path.y2 << std::endl;

                    path = obj_path;

                    if (endObj)
                        break;
                }
            }
        }

        if (endObj)
        {
#if defined DEBUG_PRINT
            std::cout << "Ending inside an object or collided to an object wall." << std::endl;
            std::cout << "X = " << path.x1 << " Y = " << path.y1 << " Z = " << path.z1
                << " X = " << path.x2 << " Y = " << path.y2 << " Z = " << path.z2 << std::endl;
#endif
            keepMov = false;
            break;
        }

        if (throughObj)
        {
#if defined DEBUG_PRINT
            std::cout << "Passed through an object." << std::endl;

            std::cout << "End Path: X = " << path.x1 << " Y = " << path.y1
                << " X = " << path.x2 << " Y = " << path.y2 << std::endl;
#endif

            if (path.x1 < -1e-6 || path.x1 > maxX() || path.y1 < -1e-6 || path.y1 > maxZ())
            {
#if defined DEBUG_PRINT
                std::cout << "Exited zone." << std::endl;
#endif
                break;
            }

            if (path.x1 < minX())
                path.x1 = minX();

            if (path.y1 < minZ())
                path.y1 = minZ();

            if (!citer->rect.Contains(path.x1,path.y1))
            {
#if defined DEBUG_PRINT
                std::cout << "Searching for new cell." << std::endl;
#endif
                prevCell = NavigationMesh::npos;
                currentCell = findClosestCell(path.x1,path.y1);

                citer = Cells.begin()+currentCell;

                assert(currentCell != npos);
            }

            throughObj = false;
        }

        assert(citer != Cells.end() && citer->rect.Contains(path.x1,path.y1));

#if defined DEBUG_PRINT
        std::cout << "Previous Cell = " << prevCell << std::endl;
        std::cout << "Current Cell = " << currentCell << std::endl;
        std::cout << "Rect = (" << citer->rect.minX << "," << citer->rect.minY
                << ")\t(" << citer->rect.maxX << "," << citer->rect.maxY << ")" << std::endl;
        std::cout << "P1 = (" << path.x1 << "," << path.y1 << ")" << std::endl;
        std::cout << "P2 = (" << path.x2 << "," << path.y2 << ")" << std::endl;
        std::cout << "# Links = " << citer->links.size() << std::endl;
#endif

        /// CHECK IF WE CAN MOVE TO OTHER CELLS IF NOT JUST SET END POINT AS SOURCE
        if (citer->links.empty())
        {
            keepMov = false;
            break;
        }

        ret = citer->ClassifyPath(prevCell,currentCell,path,dirVDown,dirHLeft,dirPrefH,&intersection);

        if (ret.first == NavigationCell::EXITING_CELL || ret.first == NavigationCell::EXITING_ZONE)
        {
            path.x1 = intersection.get_x();
            path.y1 = intersection.get_y();
            path.z1 = getHeightAt(path.x1/20,path.y1/20);

            prevCell = currentCell;
            currentCell = (ret.second->cellSrc == prevCell) ? ret.second->cellDest : ret.second->cellSrc;

            if (ret.first == NavigationCell::EXITING_CELL)
                assert(prevCell != currentCell);
        }
        else if (ret.first == NavigationCell::NO_RELATIONSHIP)
        {
            prevCell = NavigationMesh::npos;
            currentCell = findClosestCell(path.x1,path.y1);

            citer = Cells.begin()+currentCell;

            assert(currentCell != npos);
//            assert(0);
        }

        if (ret.first == NavigationCell::ENDING_CELL)
        {
#if defined DEBUG_PRINT
            std::cout << "Inside Cell." << std::endl;
#endif

            path.x1 = path.x2;
            path.y1 = path.y2;
            path.z1 = getHeightAt(path.x2/20,path.y2/20);
            keepMov = false;

            assert(citer->rect.Contains(path.x2,path.y2));
        }
    }

    if (ret.first == NavigationCell::EXITING_ZONE)
    {
#if defined PRINT_DEBUG
            std::cout << "Exiting Zone." << std::endl;
#endif
        endPos->set_zone(ret.second->zoneDest);

        /// ADJUST POSITION ACCORDING TO ZONE
        switch(ret.second->edgeDest)
        {
        case RECT_EDGE_LEFT:
            path.x1 += maxX();
            break;
        case RECT_EDGE_TOP:
            path.y1 += maxZ();
            break;
        case RECT_EDGE_RIGHT:
            path.x1 -= maxX();
            break;
        case RECT_EDGE_BOTTOM:
            path.y1 -= maxZ();
            break;
        default:
            break;
        }
    }
    else
    {
        /// ADJUST THE RESPECTIVE ZONE WITH A DEFINED TOLERANCE OF 1E-6

        int16_t dzone = 0;

        if (path.x1 > maxX())
        {
            if (path.x1-maxX() > 1E-6)
            {
                path.x1 -= maxX();

                if (path.x1 < minX())
                    path.x1 = minX();

                if (path.x1 > maxX())
                    path.x1 = maxX();

                ++dzone;
            }
            else
                path.x1 = maxX();
        }

        if (path.x1 < minX())
        {
            if (path.x1-minX() < -1E-6)
            {
                path.x1 += maxX();

                if (path.x1 > maxX())
                    path.x1 = maxX();

                if (path.x1 < minX())
                    path.x1 = minX();

                --dzone;
            }
            else
                path.x1 = minX();
        }

        if (path.y1 > maxZ())
        {
            if (path.y1-maxZ() > 1E-6)
            {
                path.y1 -= maxZ();

                if (path.y1 < minZ())
                    path.y1 = minZ();

                if (path.y1 > maxZ())
                    path.y1 = maxZ();

                dzone += 256;
            }
            else
                path.y1 = maxZ();
        }

        if (path.y1 < 0)
        {
            if (path.y1-minZ() < -1E-6)
            {
                path.y1 += maxZ();

                if (path.y1 > maxZ())
                    path.y1 = maxZ();

                if (path.y1 < minZ())
                    path.y1 = minZ();

                dzone -= 256;
            }
            else
                path.y1 = 0;
        }

        endPos->set_zone(dzone+beg.get_zone());
    }

    endPos->set_x(path.x1);
    endPos->set_y(path.y1);
    endPos->set_z(path.z1);

    assert(endPos->get_x() <= maxX() && endPos->get_x() >= minX());
    assert(endPos->get_y() <= maxZ() && endPos->get_y() >= minZ());

    return keepMov;
}

const NavigationEntity& NavigationMesh::getEntityAt (size_t idx) const
{
    assert(idx < Entities.size());

    return Entities[idx];
}

const std::vector<NavigationEntity>& NavigationMesh::getEntities () const
{
    return Entities;
}

const std::vector<NavigationCell>& NavigationMesh::getCells () const
{
    return Cells;
}

void NavigationMesh::visitEntities (const boost::function<void (const NavigationEntity&)> &func) const
{
    std::for_each(Entities.begin(),Entities.end(),func);
}

void NavigationMesh::visitCells (const boost::function<void (const NavigationCell&)> &func) const
{
    std::for_each(Cells.begin(),Cells.end(),func);
}
