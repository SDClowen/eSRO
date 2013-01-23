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

#include "packet_util_functions.hpp"
#include <sstream>
#include <iomanip>
#include <syslog.h>

void PrintPacketToSyslog (const OPacket &packet)
{
    std::stringstream ss;

    const char *buffer = (const char*)packet.buffer();

    for (int i = 0; i < packet.size(); ++i)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)buffer[i] << " ";

    syslog(LOG_DEBUG,"Hexed Packet:\n%s",ss.str().c_str());
}

void PrintPacketToSyslog (const IPacket &packet)
{
    std::stringstream ss;

    const char *buffer = (const char*)packet.buffer();

    for (int i = 0; i < packet.size(); ++i)
        ss << std::hex << std::setw(2) << std::setfill('0') << ((int)buffer[i] & 0xFF) << " ";

    syslog(LOG_DEBUG,"Hexed Packet:\n%s",ss.str().c_str());
}
