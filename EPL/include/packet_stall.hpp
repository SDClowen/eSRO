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

#ifndef __SLES_GAME_SERVER_STALL_PACKETS_HPP__
#define __SLES_GAME_SERVER_STALL_PACKETS_HPP__

#include <stdint.h>
#include <boost/shared_ptr.hpp>

class OPacket;
class Stall;

namespace srv_pkt
{

enum STALL_ACTION
{
    STALL_ACTION_LEAVE = 1,
    STALL_ACTION_ENTER,
    STALL_ACTION_BUY
};

enum STALL_OPERATION
{
    STALL_INSERT_ITEM = 2,
    STALL_REMOVE_ITEM
};

enum STALL_ERROR
{
    ERROR_STALL_INSUFFICIENT_GOLD = 0x11,
    ERROR_STALL_INVENTORY_FULL = 0x12
};

void StallSpawn (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const std::string& name,
    const uint32_t avatar);

void StallDespawn (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID);

void StallOpen (const boost::shared_ptr<OPacket> &pkt);

void StallAction (const boost::shared_ptr<OPacket> &pkt, uint8_t action);

void StallAction (const boost::shared_ptr<OPacket> &pkt, uint16_t status);

void StallAction (const boost::shared_ptr<OPacket> &pkt, const std::string& greetings);

void StallAction (const boost::shared_ptr<OPacket> &pkt, const uint8_t pos, const uint16_t quantity,
    const uint64_t price);

void StallItem (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Stall> &stall,
    const STALL_OPERATION operation);

void StallName (const boost::shared_ptr<OPacket> &pkt, const std::string& name, const uint32_t playerID);

void StallEnter (const boost::shared_ptr<OPacket> &pkt, const uint32_t splayerID, const uint32_t nplayerID,
    const boost::shared_ptr<Stall> &stall);

void StallLeave (const boost::shared_ptr<OPacket> &pkt);

void StallBuy (const boost::shared_ptr<OPacket> &pkt, const uint8_t pos);

void StallBuy (const boost::shared_ptr<OPacket> &pkt, const STALL_ERROR error);

void StallNotify (const boost::shared_ptr<OPacket> &pkt, const STALL_ACTION action, const uint32_t ID);

void StallNotify (const boost::shared_ptr<OPacket> &pkt, const std::string& name,
    const boost::shared_ptr<Stall> &stall);

void StallClose (const boost::shared_ptr<OPacket> &pkt);

}

#endif //__SLES_GAME_SERVER_STALL_PACKETS_HPP__
