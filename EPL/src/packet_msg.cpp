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

#include "packet_msg.hpp"
#include "opcodes_shard_server.hpp"

#include <packet.hpp>

namespace srv_pkt
{

void Msg (const boost::shared_ptr<OPacket> &pkt, uint32_t playerID, const std::string& msg)
{
    pkt->WriteOpcode(SERVER_PLAYER_MSG);
    pkt->Write<uint8_t>(MSG_PUBLIC);
    pkt->Write<uint32_t>(playerID);
    pkt->Write(msg);
}

void Msg (const boost::shared_ptr<OPacket> &pkt, const std::string &msg)
{
    pkt->WriteOpcode(SERVER_PLAYER_MSG);
    pkt->Write<uint8_t>(MSG_NOTICE);
    pkt->Write(msg);
}

void Msg (const boost::shared_ptr<OPacket> &pkt, const MSG_TYPE type, const std::string& sender,
    const std::string& msg)
{
    pkt->WriteOpcode(SERVER_PLAYER_MSG);
    pkt->Write<uint8_t>(type);
    pkt->Write(sender);
    pkt->Write(msg);
}

void Msg (const boost::shared_ptr<OPacket> &pkt, const MSG_TYPE type, uint8_t num_msgs)
{
    pkt->WriteOpcode(SERVER_PLAYER_MSG_ACK,ANSWER_ACCEPT);
    pkt->Write<uint8_t>(type);
    pkt->Write<uint8_t>(num_msgs);
}

}
