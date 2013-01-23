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

#ifndef __SRLN_SERVER_INFORMATION_HPP__
#define __SRLN_SERVER_INFORMATION_HPP__

#include <stdint.h>
#include <string>

enum SHARD_NOTIFY
{
    SHARD_NOTIFY_USR_CNT,
    SHARD_NOTIFY_USR_MAX,
    SHARD_NOTIFY_STATE
};

struct Shard
{
    bool is_new;		///Flag that indicates if its a new server.
    std::string name;	///Server name
    uint16_t id;		///Server Identification number.
    uint16_t usr_cnt;	///Current users count.
    uint16_t usr_max;	///Max number of users spaces available.
    uint8_t state;		///Server state (01 - Online, 00 - Check).
    std::string ip;		///Server IP to connect.
    uint16_t port;		///Server
};

#endif //__SRLN_SERVER_INFORMATION_HPP__
