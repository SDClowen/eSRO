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

#ifndef __SLES_GAME_SERVER_TELEPORT_PACKETS_HPP__
#define __SLES_GAME_SERVER_TELEPORT_PACKETS_HPP__

#include <stdint.h>
#include <boost/shared_ptr.hpp>

class OPacket;

namespace srv_pkt
{

enum TELEPORT_DESTINATION_ERROR
{
    TELEPORT_DST_ERROR_INSUFFICIENT_GOLD = 0x07,
    TELEPORT_DST_ERROR_IN_USE = 0x10,
    TELEPORT_DST_UNREPAIRABLE,
    TELEPORT_DST_UNREPRODUCEABLE,
    TELEPORT_DST_ERROR_TO_FAR_FROM_DOCK,
    TELEPORT_DST_ERROR_BEING_TELEPORTED,
    TELEPORT_DST_ERROR_INVALID_LEVEL,
    TELEPORT_DST_ERROR_MURDER_STATUS,
    TELEPORT_DST_ERROR_INVALID_REQUEST,
    TELEPORT_DST_ERROR_INVALID_JOB,
    TELEPORT_DST_ERROR_LEVEL_ABOVE_20 = 0x20,
    TELEPORT_DST_ERROR_NO_RECALL_OR_DEATH_POINT
};

void TeleportZone (const boost::shared_ptr<OPacket> &pkt, int16_t zone);

void TeleportDesignate (const boost::shared_ptr<OPacket> &pkt);

void TeleportDestination (const boost::shared_ptr<OPacket> &pkt);

void TeleportDestination (const boost::shared_ptr<OPacket> &pkt, const TELEPORT_DESTINATION_ERROR &error);

}

#endif //__SLES_GAME_SERVER_TELEPORT_PACKETS_HPP__
