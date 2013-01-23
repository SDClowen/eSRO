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

#ifndef __SLES_GAME_SERVER_MESSAGE_PACKETS_HPP__
#define __SLES_GAME_SERVER_MESSAGE_PACKETS_HPP__

#include <stdint.h>
#include <boost/shared_ptr.hpp>

class OPacket;

namespace srv_pkt
{

enum MSG_SEND_ERROR_TYPE
{
    ERROR_MSG_SEND_INVALID = 0x10,
    ERROR_MSG_SEND_INBOX_FULL = 0x14
};

void MsgSend(const boost::shared_ptr<OPacket> &pkt);

void MsgSend (const boost::shared_ptr<OPacket> &pkt, const MSG_SEND_ERROR_TYPE error);

void MsgBroadcast (const boost::shared_ptr<OPacket> &pkt);

void MsgReceive (const boost::shared_ptr<OPacket> &pkt, const std::string &sender, const uint32_t model);

void MsgRead (const boost::shared_ptr<OPacket> &pkt, const uint8_t msg_number, const std::string &msg);

void MsgDelete (const boost::shared_ptr<OPacket> &pkt, uint8_t msg_number);

}

#endif //__SLES_GAME_SERVER_MESSAGE_PACKETS_HPP__
