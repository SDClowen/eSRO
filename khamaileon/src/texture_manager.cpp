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

#include "texture_manager.hpp"
#include "strutil.hpp"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <stdlib.h>
#include <SOIL.h>
#include <iostream>

TextureManager::TextureManager (const std::string &path)
    : m_Path(path)
{
    if (*m_Path.rbegin() != '/')
        m_Path.append("/");
}

TextureManager::~TextureManager ()
{
    Release();
}

bool TextureManager::Initialize()
{
    std::string objname = m_Path + "Tile2D.ifo";

    std::ifstream file(objname.c_str());

    if (!file)
        return false;

    std::string line;
    std::getline(file,line);

    if (line != "JMXV2DTI1001")
        return false;

    std::getline(file,line);

    int count = atoi(line.c_str());

    TextureInfo obj;
    std::vector<std::string> fields;

    obj.glID = 0;

    while (!file.eof() && count)
    {
        getline(file,line);

        fields = split(line,' ');

        if (fields.size() >= 4)
        {
            obj.refID = atoi(fields[0].c_str());
            ///fields[1] is some flag stuff still unknown

            size_t idx = 2;

            do
            {
                obj.Area += fields[idx];

                if (*obj.Area.rbegin() != '"')
                    obj.Area.append(" ");

                ++idx;
            }
            while (*obj.Area.rbegin() != '"');

            do
            {
                obj.Name += fields[idx];

                if (*obj.Name.rbegin() != '"')
                    obj.Name.append(" ");

                ++idx;
            }
            while (*obj.Name.rbegin() != '"');

            obj.Area.erase(std::remove(obj.Area.begin(),obj.Area.end(),'"'),obj.Area.end());
            obj.Name.erase(std::remove(obj.Name.begin(),obj.Name.end(),'"'),obj.Name.end());

            m_Textures.insert(std::make_pair(obj.refID,obj));
        }

        obj.Area.clear();
        obj.Name.clear();
        fields.clear();
        --count;
    }

    if (count)
        return false;

    return true;
}

bool TextureManager::Release()
{
    for (std::map<uint32_t,TextureInfo>::iterator it = m_Textures.begin(); it != m_Textures.end(); ++it)
    {
        if (it->second.glID)
            glDeleteTextures(1,&it->second.glID);
    }

    m_Textures.clear();

    return true;
}

bool TextureManager::LoadTexture (uint32_t refID)
{
    std::map<uint32_t,TextureInfo>::iterator it = m_Textures.find(refID);

    if (it == m_Textures.end())
        return false;

    if (it->second.glID)
        return true;

    std::string filename = m_Path + "Tile2D/" + it->second.Name;

    std::ifstream file(filename.c_str());

    if (!file)
        return false;

    file.seekg(0,std::ios::end);

    size_t size = file.tellg();

    if (!size)
        return false;

    unsigned char *buffer = new unsigned char[size];

    if (!buffer)
        return false;

    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer),size);

//    GLuint texID = SOIL_load_OGL_texture_from_memory((buffer + 20), size - 20, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
//        SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_MULTIPLY_ALPHA |SOIL_FLAG_COMPRESS_TO_DXT |
//        SOIL_FLAG_DDS_LOAD_DIRECT | SOIL_FLAG_INVERT_Y);

//    delete [] buffer;

//    if (!texID)
//        return false;

//    it->second.glID = texID;

    return true;
}

void TextureManager::ReleaseTexture (uint32_t refID)
{
    std::map<uint32_t,TextureInfo>::iterator it = m_Textures.find(refID);

    if (it != m_Textures.end())
        glDeleteTextures(1,&it->second.glID);
}

GLuint TextureManager::Find (const uint32_t refID) const
{
    std::map<uint32_t,TextureInfo>::const_iterator it = m_Textures.find(refID);

    if (it == m_Textures.end())
        return 0;

    return it->second.glID;
}
