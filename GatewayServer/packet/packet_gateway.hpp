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

#ifndef __SRNL_PACKET_GATEWAY_HPP__
#define __SRNL_PACKET_GATEWAY_HPP__

#include <stdint.h>
#include <string>
#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>

class OPacket;

/**
 *
 *	PACKET SENT FROM THE GATEWAY TO THE MANAGER SERVER
 *
 **/

namespace clt_pkt
{

typedef boost::tuple<uint32_t,uint8_t,uint8_t,uint32_t> account_ext_type;

/**
 *
 *	Gateway configuration/initial data request.
 *
 **/

void WriteConfig (const boost::shared_ptr<OPacket> &p);

void WriteQueue (const boost::shared_ptr<OPacket> &p, const uint32_t clientID, const std::string &usr,
    const std::string &pw, const uint32_t shardID, const std::string &IP);

}

#endif
