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

#ifndef __PACKET_MANAGER_GATEWAY_HPP__
#define __PACKET_MANAGER_GATEWAY_HPP__

#include <news.hpp>
#include <shard.hpp>

#include <map>
#include <boost/shared_ptr.hpp>

class OPacket;

namespace srv_pkt
{

enum LOGIN_ERROR
{
    LOGIN_ERROR_WRONG_PW = 1,
    LOGIN_ERROR_BLOCKED_MSG,
    LOGIN_ERROR_USER_ALREDY_ON,
    LOGIN_ERROR_C5,
    LOGIN_ERROR_C6,
    LOGIN_ERROR_C7,
    LOGIN_ERROR_C8,
};

void WriteConfig (const boost::shared_ptr<OPacket> &p, const uint8_t locale, const uint32_t client_version,
    const std::map<uint16_t,Shard> &serv_list, const std::map<uint32_t,News> &news_list);

void WriteShard (const boost::shared_ptr<OPacket> &p, const Shard &serv);

void WriteShard (const boost::shared_ptr<OPacket> &p, const uint16_t ID);

void WriteShard (const boost::shared_ptr<OPacket> &p, const SHARD_NOTIFY type, const uint16_t ID, const uint16_t arg);

void WriteLogin (const boost::shared_ptr<OPacket> &pkt, const uint32_t clientID, const uint32_t queueID,
    const Shard &shard);

/**
 *
 *  @brief: Login error packet. (USER_ALREDY_ON,C5,C7,C8 are encrypted packets)
 *  @error: All login errors except WRONG_PW AND BLOCKED_MSG.
 *
 **/

void WriteLogin (const boost::shared_ptr<OPacket> &pkt, const uint32_t clientID, const LOGIN_ERROR error);

/**
 *
 *  @brief: WRONG PW/ACCOUNT ERROR.
 *  @error: it should be only LOGIN_ERROR_WRONG_PW.
 *  @max: Max login attempts before the accounts gets blocked.
 *  @count: Number of failed logins.
 *
 **/

void WriteLogin (const boost::shared_ptr<OPacket> &pkt, const uint32_t clientID, const LOGIN_ERROR error,
    const uint32_t max, const uint32_t count);

void WriteLogin (const boost::shared_ptr<OPacket> &pkt, const uint32_t clientID, const LOGIN_ERROR error,
    const std::string &msg, const uint16_t year, const uint8_t month, const uint8_t day, const uint8_t hour,
    const uint8_t min);

}

#endif
