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

#ifndef __SRNL_PACKET_LOGIN_HPP__
#define __SRNL_PACKET_LOGIN_HPP__

#include <boost/shared_ptr.hpp>

class OPacket;

namespace srv_pkt
{

enum LOGIN_ERROR
{
    LOGIN_ERROR_C9 = 1,
    LOGIN_ERROR_C10,
    LOGIN_ERROR_C11,
    LOGIN_ERROR_USER_LIMIT,
    LOGIN_ERROR_IP_LIMIT
};

void WriteLoginShard (const boost::shared_ptr<OPacket> &pkt);

void WriteLoginShard (const boost::shared_ptr<OPacket> &pkt, const LOGIN_ERROR error);

}

#endif
