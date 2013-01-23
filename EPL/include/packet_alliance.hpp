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

#ifndef __SLES_GAME_SERVER_UNION_PACKETS_HPP__
#define __SLES_GAME_SERVER_UNION_PACKETS_HPP__

#include <boost/shared_ptr.hpp>

class OPacket;
class Union;
class Guild;

namespace srv_pkt
{

enum ALLIANCE_ERROR
{
    ERROR_ALLIANCE_PLAYER_NOT_AUTHORIZED = 0x24,
    ERROR_ALLIANCE_GUILD_SAME = 0x26,
    ERROR_ALLIANCE_SAME = 0x28,
    ERROR_ALLIANCE_FULL = 0x29,
    ERROR_ALLIANCE_MASTER_GUILD_LEVEL = 0x2A,
    ERROR_ALLIANCE_SLAVE_GUILD_LEVEL = 0x2B,
    ERROR_ALLIANCE_GUILD_HOSTILE = 0x42
};

void AllianceList (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Union> &ally);

void AllianceInvite (const boost::shared_ptr<OPacket> &pkt, const ALLIANCE_ERROR error);

void AllianceExpel (const boost::shared_ptr<OPacket> &pkt);

void AllianceSecede (const boost::shared_ptr<OPacket> &pkt);

}

#endif //__SLES_GAME_SERVER_UNION_PACKETS_HPP__
