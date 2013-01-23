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

#include "packet_block.hpp"
#include "opcodes_shard_server.hpp"

#include <packet.hpp>

namespace srv_pkt
{

void Block (const boost::shared_ptr<OPacket> &pkt, const BLOCK_ACTION action, const BLOCK_MSG msg,
    const std::string &name)
{
    pkt->WriteOpcode(SERVER_BLOCK);
    pkt->Write<uint8_t>(action);
    pkt->Write<uint8_t>(msg);
    pkt->Write(name);
}

void Block (const boost::shared_ptr<OPacket> &pkt, const BLOCK_ACTION action, const BLOCK_MSG msg)
{
    pkt->WriteOpcode(SERVER_BLOCK);
    pkt->Write<uint8_t>(action);
    pkt->Write<uint8_t>(msg);
}

}
