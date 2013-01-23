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

#ifndef NAVMESH_HPP
#define NAVMESH_HPP

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <utility>

#include "navigation_cell.hpp"
#include "navigation_entity.hpp"

namespace SR
{
    class RTree;
}

struct TTextureMapEntry
{
    uint16_t w1;
    uint16_t w2;
    uint16_t w3;
    uint16_t refID;     /// Reference ID (listed map/Title2D.ifo file)
};

class ModelManager;

struct NavigationMesh
{
public:

    enum READ_MODE
    {
        MODE_NAV_MESH = 0x01,
        MODE_TEXTURE = 0x02,
        MODE_ALL = 0x03
    };

    enum { HEIGHTMAP_SIZE = 97, TEXTURE_SIZE = 96,  };

    typedef std::vector<NavigationEntity>::iterator entity_iterator;
    typedef std::vector<NavigationEntity>::const_iterator entity_const_iterator;

    static const size_t	npos = static_cast<size_t>(-1);

    NavigationMesh (int16_t refID);

    ~NavigationMesh ();

    bool buildNavigationMesh ();

    bool Read (const std::string &filename, const READ_MODE mode);

    int16_t ID () const;

    float minX () const;

    float maxX () const;

    float minZ () const;

    float maxZ () const;

    int16_t Width () const;

    int16_t Length () const;

    float getHeightAt (int x, int z) const;

    float getObjectHeight (float x, float z, const boost::shared_ptr<ModelManager> &modelMgr) const;

    uint16_t getTextureAt (int x, int z) const;

    size_t findClosestCell (float x, float z) const;

    bool resolveMotion (const Coord &beg, const Coord &end, const boost::shared_ptr<ModelManager> &modelMgr,
        Coord *endPos) const;

    const NavigationEntity& getEntityAt (size_t Idx) const;

    const std::vector<NavigationEntity>& getEntities () const;

    const std::vector<NavigationCell>& getCells () const;

    void visitEntities (const boost::function<void (const NavigationEntity&)> &func) const;

    void visitCells (const boost::function<void (const NavigationCell&)> &func) const;

private:

    int16_t refID;      /// ZONE REFERENCE ID

    std::vector<NavigationEntity> Entities;

    uint32_t EntitiesExtra;

    std::vector<NavigationCell> Cells;

    TTextureMapEntry *m_Texturemap;
    float heightmap[9409];

//    unsigned char last3[36];
//    float last4[36];

    SR::RTree *m_Tree;
};

#endif // NAVMESH_HPP
