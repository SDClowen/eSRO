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

#ifndef PACKET_ERROR_GLOBAL_HPP
#define PACKET_ERROR_GLOBAL_HPP

namespace pkt
{

enum GLOBAL_ERROR_CODES
{
    GLOBAL_ERROR_GENERAL = 0x00,
    GLOBAL_ERROR_LOGOUT = 0x08,
    GLOBAL_ERROR_NOTIFY = 0x0C,
    GLOBAL_ERROR_ITEM = 0x18,
    GLOBAL_ERROR_TELEPORT = 0x1C,
    GLOBAL_ERROR_PARTY = 0x2C,
    GLOBAL_ERROR_SKILL_CAST = 0x30,
    GLOBAL_ERROR_STALL_NETWORK = 0x3C,
    GLOBAL_ERROR_JOB = 0x48,
    GLOBAL_ERROR_GUILD = 0x4C,
    GLOBAL_ERROR_ALCHEMY = 0x58,
    GLOBAL_ERROR_FRIENDS = 0x64,
    GLOBAL_ERROR_ACADEMY = 0x68
};

}

#endif // PACKET_ERROR_GLOBAL_HPP
