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

#ifndef __SLES_GAME_SERVER_JOB_PACKETS_HPP__
#define __SLES_GAME_SERVER_JOB_PACKETS_HPP__

/// SOL INCLUDES
#include <job.hpp>
#include <market_stock.hpp>

/// GLOBAL INCLUDES
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>

class OPacket;

namespace srv_pkt
{

enum JOB_ERROR
{
    JOB_ERROR_INSUFFICIENT_GOLD = 0x07,
    JOB_ERROR_INSUFFICIENT_LEVEL = 0x19,
    JOB_ERROR_ALREDY_JOINED_A_LEAGUE,
    JOB_ERROR_NEED_JOB_LEAGUE,
    JOB_ERROR_JOB_CAPE_UNEQUIPPED,
    JOB_ERROR_INVALID_NAME,
    JOB_ERROR_JOIN_PENALTY,
    JOB_ERROR_ALIAS_EXISTS,
    JOB_ERROR_NO_PAST_JOB_INFORMATION = 0x29,
    JOB_ERROR_MAX_SAFE_TRADES = 0x41
};

enum JOB_MSG_TYPE
{
    JOB_ALIAS_VALID,
    JOB_ALIAS_CREATE,
    JOB_ALIAS_EXISTS = 0x25
};

void JobJoin (const boost::shared_ptr<OPacket> &pkt, const uint8_t job_type);

void JobJoin (const boost::shared_ptr<OPacket> &pkt, const JOB_ERROR &error);

void JobAlias (const boost::shared_ptr<OPacket> &pkt, const JOB_MSG_TYPE type, const std::string &alias);

void JobPastInfo (const boost::shared_ptr<OPacket> &pkt, const JOB_ERROR &error);

void JobRanking (const boost::shared_ptr<OPacket> &pkt, const uint8_t job, const uint8_t type,
    const std::vector<Job> &list);

void JobPayment (const boost::shared_ptr<OPacket> &pkt, const uint64_t amount);

void JobLeave (const boost::shared_ptr<OPacket> &pkt);

void JobXP (const boost::shared_ptr<OPacket> &pkt, const uint8_t type, const uint8_t level, const int32_t XP);

void JobTradeScale (const boost::shared_ptr<OPacket> &pkt, uint8_t scale);

void JobProductInformation (const boost::shared_ptr<OPacket> &pkt, const Market &market);

}

#endif //__SLES_GAME_SERVER_JOB_PACKETS_HPP__
