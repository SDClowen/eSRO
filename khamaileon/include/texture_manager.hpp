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

#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include <map>
#include <stdint.h>
#include <string>
#include <GL/gl.h>

struct TextureInfo
{
    uint32_t refID;
    std::string Area;
    std::string Name;
    GLuint glID;
};

class TextureManager
{
public:

    TextureManager (const std::string &path);

    ~TextureManager ();

    bool Initialize ();

    bool Release ();

    bool LoadTexture (const uint32_t refID);

    void ReleaseTexture (const uint32_t refID);

    GLuint Find (const uint32_t refID) const;

private:

    std::string m_Path;
    std::map<uint32_t,TextureInfo> m_Textures;
};

#endif // TEXTURE_MANAGER_HPP
