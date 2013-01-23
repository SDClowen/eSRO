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

#include "packet_talk.hpp"
#include "opcodes_shard_server.hpp"

#include <packet.hpp>

namespace srv_pkt
{

void TalkInitiate (const boost::shared_ptr<OPacket> &pkt, uint8_t action)
{
    pkt->WriteOpcode(SERVER_NPC_TALK_INITIATE);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
    pkt->Write<uint8_t>(action);

    if (action == 0x0C)
        pkt->Write<uint8_t>(0);
}

void TalkTerminate (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_NPC_TALK_TERMINATE);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
}

}
