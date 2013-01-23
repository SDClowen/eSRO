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

#ifndef __SRNL_PACKET_MSG_HPP__
#define __SRNL_PACKET_MSG_HPP__

#include <stdint.h>
#include <boost/shared_ptr.hpp>

class OPacket;

namespace srv_pkt
{

enum MSG_TYPE
{
    MSG_PUBLIC = 0x01,
    MSG_PRIVATE,
    MSG_PUBLIC_MULTIBYTE,
    MSG_PARTY,
    MSG_GUILD,
    MSG_GLOBAL,
    MSG_NOTICE,
    MSG_STALL,
    MSG_ACADEMY,
    MSG_UNION = 0x0B
};

/**
 *
 *  @brief Construct a public chat packet.
 *  @playerID Player ID thats sending the msg.
 *  @msg Messge to be sent.
 *
 **/

void Msg (const boost::shared_ptr<OPacket> &pkt, uint32_t playerID, const std::string& msg);

void Msg (const boost::shared_ptr<OPacket> &pkt, const std::string &msg);

void Msg (const boost::shared_ptr<OPacket> &pkt, const MSG_TYPE type, const std::string& sender,
    const std::string& msg);

void Msg (const boost::shared_ptr<OPacket> &pkt, const MSG_TYPE type, uint8_t num_msgs);

}

#endif  ///__SRNL_PACKET_MSG_HPP__
