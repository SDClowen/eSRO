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

#ifndef MODEL_MESH_HPP
#define MODEL_MESH_HPP

#include <stdint.h>
#include <string>
#include <vector>

#include "coord.hpp"
#include "line.hpp"

struct MeshVertex
{
    float Position[3];
    float Normal[3];
    float Texture[2];
    float Unk[3];
};

struct TextureCoord
{
    int32_t u;
    int32_t v;
};

struct MeshFace
{
    size_t VertexIdx[3];
};

class CollisionMesh;

class ModelMesh
{
public:

    ModelMesh ();

    ~ModelMesh ();

    bool Read (const std::string &filename);

    const CollisionMesh* getCollisionMesh () const;

private:

    std::string Name;
    std::string ModelName;

//    std::vector<MeshVertex> Vertices;
//    std::vector<TextureCoord> Lightmap;
//    std::string LightmapTexture;
//    std::vector<std::string> Bones;
//    std::vector<MeshFace> Faces;
//
//    float StatUnk[6];

    CollisionMesh *NMesh;
};

#endif // MODEL_MESH_HPP
