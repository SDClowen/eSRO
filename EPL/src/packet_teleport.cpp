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

#include "packet_teleport.hpp"
#include "opcodes_shard_server.hpp"

#include <packet.hpp>

namespace srv_pkt
{

void TeleportZone (const boost::shared_ptr<OPacket> &pkt, int16_t zone)
{
    pkt->WriteOpcode(SERVER_TELEPORT_ZONE);
    pkt->Write<int16_t>(zone);
}

void TeleportDesignate (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_TELEPORT_DESIGNATE);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
}

void TeleportDestination (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_TELEPORT_TO_DESTINATION);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
}

void TeleportDestination (const boost::shared_ptr<OPacket> &pkt, const TELEPORT_DESTINATION_ERROR &error)
{
    pkt->WriteOpcode(SERVER_TELEPORT_TO_DESTINATION);
    pkt->Write<uint8_t>(ANSWER_ERROR);
    pkt->Write<uint8_t>(error);
    pkt->Write<uint8_t>(0x1C);
}

}
