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

#ifndef MODEL_HPP
#define MODEL_HPP

#include "line.hpp"
#include <coord.hpp>

#include <stdint.h>
#include <string>
#include <utility>

class ModelMesh;

class Model
{
public:

    enum TYPES
    {
        TYPE_0 = 0x20000,
        TYPE_1 = 0x20001,
        TYPE_2 = 0x20002,
        TYPE_3 = 0x20003,
        TYPE_4 = 0x20004,
        TYPE_5 = 0x20006,
    };

    Model ();

    ~Model ();

    inline const std::string& getName () const { return Name; }

    bool Read (const std::string &filename);

    bool ReadMeshs (const std::string &path);

    bool hasCollisionMesh () const;

    ModelMesh* getCollisionMesh () const;

    std::pair<bool,uint8_t> resolveMotion (const Line &path, Coord *intersection) const;

    float getHeightAt (float x, float z) const;

private:

    uint32_t Type;
    std::string Name;

    std::string CollisionMesh;
    ModelMesh *Mesh;

public:

    /// AABB DATA
    float minX,minY,minZ;
    float maxX,maxY,maxZ;
};

#endif // MODEL_HPP
