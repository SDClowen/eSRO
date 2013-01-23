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

#ifndef __SLES_GAME_SERVER_MISC_PACKETS_HPP__
#define __SLES_GAME_SERVER_MISC_PACKETS_HPP__

#include <stdint.h>
#include <boost/shared_ptr.hpp>

class OPacket;

namespace srv_pkt
{

enum LOGOUT_ERROR_TYPE
{
    LOGOUT_ERROR_ATTACK_STATE = 1,
    LOGOUT_ERROR_TELEPORTING
};

void Logout (const boost::shared_ptr<OPacket> &pkt, const uint8_t wait_time, const uint8_t action);

void Logout (const boost::shared_ptr<OPacket> &pkt, const LOGOUT_ERROR_TYPE error);

void LogoutNotify (const boost::shared_ptr<OPacket> &pkt);

void LogoutCancel (const boost::shared_ptr<OPacket> &pkt);

}

#endif //__SLES_GAME_SERVER_MISC_PACKETS_HPP__
