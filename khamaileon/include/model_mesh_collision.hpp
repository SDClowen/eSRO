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

#ifndef MODEL_COLLISION_MESH_HPP
#define MODEL_COLLISION_MESH_HPP

#include <boost/function.hpp>
#include <stdint.h>
#include <vector>

#include <coord.hpp>
#include "line.hpp"

struct CollisionVertex
{
    float x;
    float z;
    float y;
    uint8_t unk;
};

struct CollisionLink
{
    enum TYPE
    {
        LINK_OUTER_BORDER_PASSABLE = 0x00,
        LINK_OUTER_BORDER_WALL = 0x03,
        LINK_INNER_BORDER = 0x04,
        LINK_INNER_WALL = 0x07,
        LINK_UNKNOWN = 0x08,
        LINK_AERIAL_OUTER_BORDER_WALL = 0x10,
        LINK_AERIAL_INNER_BORDER_PASSABLE = 0x14,
        LINK_NONE = 0xFF
    };

    CollisionLink ();

    uint16_t vertexSrc;
    uint16_t vertexDest;
    uint16_t cellSrc;
    uint16_t cellDest;
    uint8_t flag;
    uint8_t flag2;
};

struct CollisionCell
{
    enum EDGE
    {
        EDGE_AB,
        EDGE_BC,
        EDGE_CA
    };

    uint16_t vertexA;
    uint16_t vertexB;
    uint16_t vertexC;
    uint16_t u4;
    uint8_t u5;

    size_t GetLinkEdge (const uint8_t side) const;

    std::vector<CollisionLink> Links;
};

class CollisionMesh
{
public:

    enum PATH_RESULT
    {
        PATH_CELL_EXIT,
        PATH_CELL_END,
        PATH_NO_RELATIONSHIP
    };

    typedef std::vector<CollisionCell>::const_iterator nav_const_iterator;

    /**
     *
     *  @brief Insert a new vertex into mesh list.
     *
     **/

    void addVertex (const CollisionVertex &vertex);

    /**
     *
     *  @brief Insert a new cell into mesh list.
     *
     **/

    void addCell (const CollisionCell &cell);

    /**
     *
     *  @brief Insert a new link between cells.
     *
     **/

    void addLink (const CollisionLink &link);

    /**
     *
     *  @brief Check if the requested point is inside the selected cell performing the selected
     *  translation (Transformation).
     *  @point: Point to test.
     *  @cellIter: Cell to test if the selected point is inside or not.
     *  @return: If the point is inside the cell or not.
     *
     **/

    bool isPointInsideCell (const Coord &point, const nav_const_iterator &cellIter) const;

    /**
     *
     *  @brief Get model height at the specified coords, by shooting a ray from inf to -inf.
     *  @return Height if nothing intersectes with the ray returns -infinite.
     *
     **/

    float getHeightAt (float x, float z) const;

    /**
     *
     * @brief find the closest cell to the given motion path.
     *
     **/

    nav_const_iterator findClosestCell (const Line &path) const;

    /**
     *
     *  @brief force the point into the cell by getting the intersection from the
     *  cell midpoint to the given point.
     *
     **/

    std::pair<bool,uint8_t> forcePointToCell (const nav_const_iterator &iter, Coord &point) const;

    std::pair<PATH_RESULT,uint8_t> classifyPathToCell (const Line &path, const nav_const_iterator &cellIter,
        Coord *intersection) const;

    std::pair<bool,uint8_t> resolveMotion (const Line &path, Coord *intersection) const;

    /**
     *
     *  @brief Iterate through all vertices and calling the given function.
     *  @func: Function to call for each vertex.
     *
     **/

    void visitVertices (const boost::function<void (const CollisionVertex&) > func) const;

    /**
     *
     *  @brief Iterate through all cells and calling the given function.
     *  @func: Function to call for each cell.
     *
     **/

    void visitCells (const boost::function<void (const CollisionCell&)> func) const;

    const std::vector<CollisionVertex>& getVertices () const;

    const std::vector<CollisionCell>& getCells () const;

    void clear ();

private:

    std::vector<CollisionVertex> Vertices;
    std::vector<CollisionCell> Cells;
};

#endif // MODEL_COLLISION_MESH_HPP
