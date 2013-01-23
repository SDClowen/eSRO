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

#include "packet_shard.hpp"
#include "opcodes_manager_gateway_client.hpp"

#include <packet.hpp>

namespace clt_pkt
{

void WriteConfig (const boost::shared_ptr<OPacket> &pkt, const Shard &serv)
{
    pkt->WriteOpcode(GM_CL_CONFIG);
    pkt->Write<uint8_t>(serv.is_new);
    pkt->Write(serv.name);
    pkt->Write<uint16_t>(serv.usr_cnt);
    pkt->Write<uint16_t>(serv.usr_max);
    pkt->Write<uint8_t>(serv.state);
    pkt->Write(serv.ip);
    pkt->Write<uint16_t>(serv.port);
}

void WriteAccountNotify8 (const boost::shared_ptr<OPacket> &pkt, const ACCOUNT_NOTIFY type, const uint32_t AccountID,
    const uint8_t arg)
{
    pkt->WriteOpcode(GM_CL_ACCOUNT_NOTIFY);
    pkt->Write<uint8_t>(type);
    pkt->Write<uint32_t>(AccountID);
    pkt->Write<uint8_t>(arg);
}

void WriteAccountNotify32 (const boost::shared_ptr<OPacket> &pkt, const ACCOUNT_NOTIFY type, const uint32_t AccountID,
    const uint32_t arg)
{
    pkt->WriteOpcode(GM_CL_ACCOUNT_NOTIFY);
    pkt->Write<uint8_t>(type);
    pkt->Write<uint32_t>(AccountID);
    pkt->Write<uint32_t>(arg);
}

void WriteShardNotify8 (const boost::shared_ptr<OPacket> &pkt, const SHARD_NOTIFY type, const uint32_t ID,
    const uint8_t arg)
{
    pkt->WriteOpcode(GM_CL_SHARD_NOTIFY);
    pkt->Write<uint8_t>(type);
    pkt->Write<uint16_t>(ID);
    pkt->Write<uint8_t>(arg);
}

void WriteShardNotify16 (const boost::shared_ptr<OPacket> &pkt, const SHARD_NOTIFY type, const uint32_t ID,
    const uint16_t arg)
{
    pkt->WriteOpcode(GM_CL_SHARD_NOTIFY);
    pkt->Write<uint8_t>(type);
    pkt->Write<uint16_t>(ID);
    pkt->Write<uint16_t>(arg);
}

}
