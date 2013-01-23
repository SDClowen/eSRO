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

#include "packet_misc.hpp"
#include "opcodes_shard_server.hpp"

#include <packet.hpp>

namespace srv_pkt
{

void Logout (const boost::shared_ptr<OPacket> &pkt, const uint8_t wait_time, const uint8_t action)
{
    pkt->WriteOpcode(SERVER_LOGOUT);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
    pkt->Write<uint32_t>(wait_time);
    pkt->Write<uint8_t>(action);
}

void Logout (const boost::shared_ptr<OPacket> &pkt, const LOGOUT_ERROR_TYPE error)
{
    pkt->WriteOpcode(SERVER_LOGOUT,ANSWER_ERROR,error);
    pkt->Write<uint8_t>(0x08);
}

void LogoutNotify (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_LOGOUT_NOTIFY);
}

void LogoutCancel (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_LOGOUT_CANCEL);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
}

}
