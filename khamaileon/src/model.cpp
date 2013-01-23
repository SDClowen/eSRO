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

#include "model.hpp"
#include "model_mesh.hpp"
#include "model_mesh_collision.hpp"

#include <cassert>
#include <memory.h>
#include <string.h>
#include <fstream>
#include <iostream>

Model::Model ()
    : Mesh(0)
{
}

Model::~Model ()
{
    delete Mesh;
}

bool Model::Read (const std::string &filename)
{
    enum OFFSETS
    {
        OFFSET_MATERIAL,
        OFFSET_MESH,
        OFFSET_SKELETON,
        OFFSET_ANIMATION,
        OFFSET_5,
        OFFSET_SOUND,
        OFFSET_LIGHT,
        OFFSET_COLLISION,
        OFFSET_9,
        OFFSET_10,
        OFFSET_11,
        OFFSET_12,
        OFFSET_13,
        OFFSET_MAX
    };

    std::ifstream file(filename.c_str());

    if (!file)
        return 0;

    char company[4] = { 0 };
    file.read(company,3);

    if (strcmp(company,"JMX") != 0)
        return false;

    char format[6] = { 0 };
    file.read(format,5);

    if (strcmp(format,"VRES ") != 0)
        return false;

    char version[5] = { 0 };
    file.read(version,4);

    if (strcmp(version,"0109") != 0)
        return false;

    uint32_t Offsets[OFFSET_MAX];

    file.read((char*)Offsets,OFFSET_MAX*sizeof(uint32_t));
    file.read((char*)&Type,sizeof(uint32_t));

    switch(Type)
    {
    case TYPE_0:
    case TYPE_1:
    case TYPE_2:
    case TYPE_3:
    case TYPE_4:
    case TYPE_5:
        break;
    default:
        std::cout << "Unknown Type " << std::hex << Type << std::dec << std::endl;
        return false;
    }

    uint32_t len = 0;
    char strbuff[200] = { 0 };

    file.read((char*)&len,sizeof(uint32_t));

    assert(len < 200);

    file.read(strbuff,len);
    Name = std::string(strbuff,strbuff+len);

    file.seekg(48,std::ios::cur);

    memset(strbuff,0,200);
    file.read((char*)&len,sizeof(uint32_t));

    assert(len < 200);

    file.read(strbuff,len);
    CollisionMesh = std::string(strbuff,strbuff+len);

    for (size_t i = 0; i < CollisionMesh.size(); ++i)
    {
        if (CollisionMesh[i] == '\\')
            CollisionMesh[i] = '/';
    }

    file.read((char*)&minX,sizeof(float));
    file.read((char*)&minY,sizeof(float));
    file.read((char*)&minZ,sizeof(float));
    file.read((char*)&maxX,sizeof(float));
    file.read((char*)&maxY,sizeof(float));
    file.read((char*)&maxZ,sizeof(float));

    return true;
}

bool Model::ReadMeshs (const std::string &path)
{
    if (path.empty() || CollisionMesh.empty())
        return false;

    Mesh = new ModelMesh();

    if (!Mesh)
        return false;

    std::string filename = path;

    if (*filename.rbegin() != '/')
        filename.append("/");

    filename += CollisionMesh;

    if (!Mesh->Read(filename))
        return false;

    return true;
}

bool Model::hasCollisionMesh () const
{
    return CollisionMesh.size();
}

ModelMesh* Model::getCollisionMesh () const
{
    return Mesh;
}

std::pair<bool,uint8_t> Model::resolveMotion (const Line &path, Coord *intersection) const
{
    if (!hasCollisionMesh())
        return std::make_pair(false,255);

    return Mesh->getCollisionMesh()->resolveMotion(path,intersection);
}

float Model::getHeightAt (float x, float z) const
{
    if (!hasCollisionMesh())
        return -1E4;

    return Mesh->getCollisionMesh()->getHeightAt(x,z);
}
