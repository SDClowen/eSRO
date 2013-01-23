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

#include "model_mesh.hpp"
#include "model_mesh_collision.hpp"

#include <fstream>
#include <cassert>
#include <cmath>
#include <string.h>
//#include <iostream>

struct CCL_UK4
{
    float u1;
    float u2;
    uint32_t u3;
    uint32_t u4;

    std::vector<std::vector<uint16_t> > u5;
};

ModelMesh::ModelMesh ()
    : NMesh(0)
{
}

ModelMesh::~ModelMesh ()
{
    if (NMesh)
        delete NMesh;
}

bool ModelMesh::Read (const std::string &filename)
{
    enum OFFSETS
    {
        OFFSET_VERTEX,
        OFFSET_BONE,
        OFFSET_TRIANGLE,
        OFFSET_3,
        OFFSET_4,
        OFFSET_STAT,
        OFFSET_6,
        OFFSET_COLLISION,
        OFFSET_8,
        OFFSET_9,
        OFFSET_10,
        OFFSET_MAX
    };

    enum FLAGS
    {
        FLAG_0,
        FLAG_1,
        FLAG_2,
        FLAG_3,
        FLAG_MAX
    };

    std::ifstream file(filename.c_str());

    if (!file)
        return false;

    char tag[13] = { 0 };
    file.read(tag,12);

    if (strcmp(tag,"JMXVBMS 0110") != 0)
        return false;

    uint32_t Offsets[OFFSET_MAX];

    file.read(reinterpret_cast<char*>(Offsets),sizeof(uint32_t)*OFFSET_MAX);

//    for (int i = 0; i < OFFSET_MAX; ++i)
//        std::cout << "Offset #" << i << " = " << std::hex << Offsets[i] << std::dec << std::endl;

    uint32_t Flags[FLAG_MAX];
    file.read(reinterpret_cast<char*>(Flags),sizeof(uint32_t)*FLAG_MAX);

//    for (int i = 0; i < FLAG_MAX; ++i)
//        std::cout << "Flag #" << i << " = " << std::hex << Flags[i] << std::dec << std::endl;

    uint32_t len = 0;
    char strbuff[200] = { 0 };

    file.read(reinterpret_cast<char*>(&len),sizeof(uint32_t));

    assert(len < 200);

    file.read(strbuff,len);
    Name.assign(strbuff,strbuff+len);

//    std::cout << "Mesh Name = " << Name << std::endl;

    memset(strbuff,0,200);

    file.read(reinterpret_cast<char*>(&len),sizeof(uint32_t));

    assert(len < 200);

    file.read(strbuff,len);
    ModelName.assign(strbuff,strbuff+len);

//    std::cout << "Referenced Model Name = " << ModelName << std::endl;

//    /// SKIP UNKNOWN 4 BYTES (SEEM THATS ALWAYS 0)
//    file.seekg(4,std::ios::cur);
//
//    /// READ MESH VERTICES
//    file.read(reinterpret_cast<char*>(&len),sizeof(uint32_t));
//
//    std::cout << "# Vertices = " << len << std::endl;
//
//    MeshVertex vertex;
//    for (size_t i = 0; i < len; ++i)
//    {
//        file.read(reinterpret_cast<char*>(vertex.Position),3*sizeof(float));
//        file.read(reinterpret_cast<char*>(vertex.Normal),3*sizeof(float));
//        file.read(reinterpret_cast<char*>(vertex.Texture),2*sizeof(float));
//        file.read(reinterpret_cast<char*>(vertex.Unk),3*sizeof(float));
//
//        Vertices.push_back(vertex);
//    }
//
//    if (Flags[FLAG_2] == 0x400)
//    {
//        std::cout << "Reading Lightmap data." << std::endl;
//
//        MeshLightmap lmap;
//
//        for (size_t i = 0; i < len; ++i)
//        {
//            file.read(reinterpret_cast<char*>(&lmap.u),sizeof(int32_t));
//            file.read(reinterpret_cast<char*>(&lmap.v),sizeof(int32_t));
//
//            Lightmap.push_back(lmap);
//        }
//
//        size_t slen = 0;
//        file.read(reinterpret_cast<char*>(&slen),sizeof(uint32_t));
//
//        assert(slen < 200);
//
//        memset(strbuff,0,200);
//        file.read(strbuff,slen);
//        LightmapTexture.assign(strbuff,strbuff+slen);
//    }
//
//    /// READ MESH BONES
//    file.read(reinterpret_cast<char*>(&len),sizeof(uint32_t));
//
//    std::cout << "# Bones = " << len << std::endl;
//
//    size_t bone_len = 0;
//    for (size_t i = 0; i < len; ++i)
//    {
//        file.read(reinterpret_cast<char*>(&bone_len),sizeof(uint32_t));
//
//        assert(bone_len < 200);
//
//        memset(strbuff,0,200);
//        file.read(strbuff,bone_len);
//        Bones.push_back(std::string(strbuff,strbuff+len));
//    }
//
//    if (len)
//    {
//        for (size_t i = 0; i < Vertices.size(); ++i)
//            file.seekg(6,std::ios::cur);
//    }
//
//    /// READ MESH FACES
//    file.read(reinterpret_cast<char*>(&len),sizeof(uint32_t));
//
//    std::cout << "# Faces = " << len << std::endl;
//
//    MeshFace face;
//    for (size_t i = 0; i < len; ++i)
//    {
//        file.read(reinterpret_cast<char*>(&face.VertexIdx),3*sizeof(uint16_t));
//
//        Faces.push_back(face);
//    }
//
//    /// READ OFFSET_3 DATA
//    file.read(reinterpret_cast<char*>(&len),sizeof(uint32_t));
//
//    for (size_t i = 0; i < len; ++i)
//    {
//        file.seekg(4,std::ios::cur); // FLOAT
//        file.seekg(4,std::ios::cur); // INT32
//    }
//
//    /// READ OFFSET_4 DATA
//    file.read(reinterpret_cast<char*>(&len),sizeof(uint32_t));
//
//    for (size_t i = 0; i < len; ++i)
//    {
//        file.seekg(sizeof(int32_t),std::ios::cur); // INT32
//        file.seekg(sizeof(int32_t),std::ios::cur); // INT32
//        file.seekg(sizeof(float),std::ios::cur); // FLOAT
//    }
//
//    if (len)
//    {
////        file.read(reinterpret_cast<char*>(&len),sizeof(uint32_t));
////
////        std::cout << "len = " << len << std::endl;
////
////        for (size_t i = 0; i < len; ++i)
////        {
////            file.seekg(4,std::ios::cur); // INT32
////            file.seekg(4,std::ios::cur); // INT32
////        }
////
////        file.read(reinterpret_cast<char*>(&len),sizeof(uint32_t));
////
////        std::cout << "len = " << len << std::endl;
////
////        for (size_t i = 0; i < len; ++i)
////            file.seekg(sizeof(uint16_t),std::ios::cur);
////
////        file.seekg(8*4,std::ios::cur); // FLOAT
////
////        static int count = 0;
////        count++;
////
////        if (count > 4)
////            assert(0);
//    }
//
//    /// READ STAT UNK DATA
//    file.seekg(Offsets[OFFSET_STAT],std::ios::beg);
//    file.read(reinterpret_cast<char*>(StatUnk),6*sizeof(float));
//
//    /// READ OFFSET_6 DATA
//    file.read(reinterpret_cast<char*>(&len),sizeof(uint32_t));
//
//    std::cout << len << std::endl;
//
//    for (size_t i = 0; i < len; ++i)
//    {
//        std::cout << filename << std::endl;
//        assert(0);
//    }
//
//    /// READ OFFSET_9 DATA
//    file.read(reinterpret_cast<char*>(&len),sizeof(uint32_t));
//
//    for (size_t i = 0; i < len; ++i)
//    {
//        std::cout << filename << std::endl;
//        assert(0);
//    }

    if (Offsets[OFFSET_COLLISION])
    {
        if (!NMesh)
        {
            NMesh = new CollisionMesh();

            if (!NMesh)
                return false;
        }
        else
        {
            NMesh->clear();
        }

        file.seekg(Offsets[OFFSET_COLLISION],std::ios::beg);

        /// READ COLLISION DATA
        file.read(reinterpret_cast<char*>(&len),sizeof(uint32_t));

        CollisionVertex nvertex;
        for (size_t i = 0; i < len; ++i)
        {
            file.read(reinterpret_cast<char*>(&nvertex.x),sizeof(float));
            file.read(reinterpret_cast<char*>(&nvertex.z),sizeof(float));
            file.read(reinterpret_cast<char*>(&nvertex.y),sizeof(float));
            file.read(reinterpret_cast<char*>(&nvertex.unk),sizeof(uint8_t));

            NMesh->addVertex(nvertex);
        }

        file.read(reinterpret_cast<char*>(&len),sizeof(uint32_t));

        CollisionCell cell;
        for (size_t i = 0; i < len; ++i)
        {
            file.read(reinterpret_cast<char*>(&cell.vertexA),sizeof(uint16_t));
            file.read(reinterpret_cast<char*>(&cell.vertexB),sizeof(uint16_t));
            file.read(reinterpret_cast<char*>(&cell.vertexC),sizeof(uint16_t));
            file.read(reinterpret_cast<char*>(&cell.u4),sizeof(uint16_t));

            if (Flags[FLAG_0] == 6 || Flags[FLAG_0] == 0x0E)
                file.read(reinterpret_cast<char*>(&cell.u5),sizeof(uint8_t));

            NMesh->addCell(cell);
        }

        for (int j = 0; j < 2; ++j)
        {
            file.read(reinterpret_cast<char*>(&len),sizeof(uint32_t));

            CollisionLink link;
            for (size_t i = 0; i < len; ++i)
            {
                file.read(reinterpret_cast<char*>(&link.vertexSrc),sizeof(uint16_t));
                file.read(reinterpret_cast<char*>(&link.vertexDest),sizeof(uint16_t));
                file.read(reinterpret_cast<char*>(&link.cellSrc),sizeof(uint16_t));
                file.read(reinterpret_cast<char*>(&link.cellDest),sizeof(uint16_t));
                file.read(reinterpret_cast<char*>(&link.flag),sizeof(uint8_t));

                if (Flags[FLAG_0] == 5)
                    file.read(reinterpret_cast<char*>(&link.flag2),sizeof(uint8_t));

                NMesh->addLink(link);
            }
        }
//
//        std::cout << "Reading Unknown section4..." << std::endl;
//
//        CCL_UK4 uk4;
//        file.read(reinterpret_cast<char*>(&uk4.u1),sizeof(float));
//        file.read(reinterpret_cast<char*>(&uk4.u2),sizeof(float));
//        file.read(reinterpret_cast<char*>(&uk4.u3),sizeof(uint32_t));
//        file.read(reinterpret_cast<char*>(&uk4.u4),sizeof(uint32_t));
//
//        std::cout << "U1 = " << uk4.u1 << std::endl;
//        std::cout << "U2 = " << uk4.u2 << std::endl;
//        std::cout << "U3 = " << uk4.u3 << std::endl;
//        std::cout << "U4 = " << uk4.u4 << std::endl;
//
//        file.read(reinterpret_cast<char*>(&len),sizeof(uint32_t));
//
//        uint16_t val4;
//        std::vector<uint16_t> vlist;
//        for (size_t i = 0; i < len; ++i)
//        {
//            std::cout << "List #" << i << std::endl;
//
//            size_t size = 0;
//            file.read(reinterpret_cast<char*>(&size),sizeof(uint32_t));
//
//            for (size_t j = 0; j < size; ++j)
//            {
//                file.read(reinterpret_cast<char*>(&val4),sizeof(uint16_t));
//                vlist.push_back(val4);
//
//                std::cout << "Val4 = " << val4 << std::endl;
//            }
//
//            uk4.u5.push_back(vlist);
//
//            vlist.clear();
//        }
    }

    return true;
}

const CollisionMesh* ModelMesh::getCollisionMesh() const
{
    return NMesh;
}
