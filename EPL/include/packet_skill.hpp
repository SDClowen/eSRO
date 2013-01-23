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

#ifndef __SLES_GAME_SERVER_SKILL_PACKET_HPP__
#define __SLES_GAME_SERVER_SKILL_PACKET_HPP__

#include <stdint.h>
#include <boost/shared_ptr.hpp>

class OPacket;

namespace srv_pkt
{

void IncSkill (const boost::shared_ptr<OPacket> &pkt, const uint32_t skillID);

void InitBuff (const boost::shared_ptr<OPacket> &pkt, const uint32_t casterID, const uint32_t skillID,
    const uint32_t buffID);

void BuffTimeout (const boost::shared_ptr<OPacket> &pkt, const uint32_t CastID, const uint32_t time);

void TermBuff (const boost::shared_ptr<OPacket> &pkt, const uint32_t buffID);

}

#endif //__SLES_GAME_SERVER_SKILL_PACKET_HPP__
