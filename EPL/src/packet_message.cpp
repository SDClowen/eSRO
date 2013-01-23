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

#include "packet.hpp"
#include "packet_message.hpp"
#include "opcodes_shard_server.hpp"

namespace srv_pkt
{

void MsgSend (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_MESSAGE_SEND,ANSWER_ACCEPT);
}

void MsgSend (const boost::shared_ptr<OPacket> &pkt, const MSG_SEND_ERROR_TYPE error)
{
    pkt->WriteOpcode(SERVER_MESSAGE_SEND,ANSWER_ERROR,error);
}

void MsgBroadcast (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_MESSAGE_BROADCAST);
    pkt->Write<uint16_t>(0x01);
}

void MsgReceive (const boost::shared_ptr<OPacket> &pkt, const std::string &sender, const uint32_t model)
{
    pkt->WriteOpcode(SERVER_MESSAGE_RECEIVE);
    pkt->Write<uint8_t>(0x08);
    pkt->Write(sender);
    pkt->Write<uint32_t>(model);
    pkt->Write<uint32_t>(0);	//TIMESTAMP!!!
}

void MsgRead (const boost::shared_ptr<OPacket> &pkt, const uint8_t msg_number, const std::string &msg)
{
    pkt->WriteOpcode(SERVER_MESSAGE_READ,ANSWER_ACCEPT);
    pkt->Write<uint8_t>(msg_number);
    pkt->Write(msg);
}

void MsgDelete (const boost::shared_ptr<OPacket> &pkt, uint8_t num_msgs)
{
    pkt->WriteOpcode(SERVER_MESSAGE_DELETE,ANSWER_ACCEPT);
    pkt->Write<uint8_t>(num_msgs);
}

}
