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

#include "packet_skill.hpp"
#include "opcodes_shard_server.hpp"

#include <packet.hpp>

namespace srv_pkt
{

void IncSkill (const boost::shared_ptr<OPacket> &pkt, const uint32_t skillID)
{
    pkt->WriteOpcode(SERVER_SKILL_INCREASE);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
    pkt->Write<uint32_t>(skillID);
}

void InitBuff (const boost::shared_ptr<OPacket> &pkt, const uint32_t casterID, const uint32_t skillID,
    const uint32_t buffID)
{
    pkt->WriteOpcode(SERVER_SKILL_INIT_BUFF);
    pkt->Write<uint32_t>(casterID);
    pkt->Write<uint32_t>(skillID);
    pkt->Write<uint32_t>(buffID);
}

void BuffTimeout (const boost::shared_ptr<OPacket> &pkt, const uint32_t CastID, const uint32_t time)
{
    pkt->WriteOpcode(SERVER_SKILL_BUFF_TIMEOUT);
    pkt->Write<uint8_t>(0);
    pkt->Write<uint32_t>(CastID);
    pkt->Write<uint32_t>(time);
}

void TermBuff (const boost::shared_ptr<OPacket> &pkt, const uint32_t buffID)
{
    pkt->WriteOpcode(SERVER_SKILL_TERMINATE_BUFF);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
    pkt->Write<uint32_t>(buffID);
}

}
