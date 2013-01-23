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

#ifndef TELEPORT_HPP
#define TELEPORT_HPP

#include <stdint.h>
#include <string>
#include <sys/types.h>

struct TeleportLink
{
    uint16_t src;
    uint16_t dest;
    int tax;
    int minlv;
    int maxlv;
};

struct Teleport
{
    uint16_t ID;
    std::string refName;
    uint32_t AssignedID;
    std::string refZone;
    int16_t Zone;
    float x;
    float z;
    float y;
    ///bool tf; //Teleport Gate flag
    ///bool td; //TD teleport flag
    ///int ff; //Fortress War owner
};

struct TeleportOptional
{
    uint32_t id;
    int16_t zone;
    float x;
    float z;
    float y;
    int location;
    bool free;
    uint16_t minlv;
    uint16_t maxlv;
};

#endif // TELEPORT_HPP
