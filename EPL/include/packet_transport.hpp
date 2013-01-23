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

#ifndef __SRNL_PACKET_HORSE_HPP__
#define __SRNL_PACKET_HORSE_HPP__

#include <stdint.h>
#include <boost/shared_ptr.hpp>

class OPacket;
class Transport;

namespace srv_pkt
{

enum TRANSPORT_ACTION
{
    TRANSPORT_ACTION_UNMOUNT,
    TRANSPORT_ACTION_MOUNT
};

enum TRANSPORT_OPERATION
{
    SOLD_GOODS,
    BUYING_GOODS
};

enum MSG_ERROR_TRANSPORT
{
    MSG_ERROR_TRANSPORT_TO_FAR = 0x04,
    MSG_ERROR_STATE_ATTACK = 0x78
};

void TransportStats (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Transport> &transport,
    const uint32_t playerID);

void TransportStatsRemove (const boost::shared_ptr<OPacket> &pkt, const uint32_t transportID);

void TransportAction (const boost::shared_ptr<OPacket> &pkt, const MSG_ERROR_TRANSPORT &error);

void TransportAction (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const TRANSPORT_ACTION action,
    const uint32_t transportID);

void TransportTerminate (const boost::shared_ptr<OPacket> &pkt);

void TransportOperation (const boost::shared_ptr<OPacket> &pkt, const TRANSPORT_OPERATION op);

}

#endif
