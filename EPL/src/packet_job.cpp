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

#include "packet_job.hpp"
#include "opcodes_shard_server.hpp"

#include <packet.hpp>

namespace srv_pkt
{

void JobJoin (const boost::shared_ptr<OPacket> &pkt, const uint8_t job_type)
{
    pkt->WriteOpcode(SERVER_JOB_JOIN,ANSWER_ACCEPT);
    pkt->Write<uint8_t>(job_type);
    pkt->Write<uint32_t>(1);
    pkt->Write<uint8_t>(0x00);
}

void JobJoin (const boost::shared_ptr<OPacket> &pkt, const JOB_ERROR &error)
{
    pkt->WriteOpcode(SERVER_JOB_JOIN,ANSWER_ERROR,error);
    pkt->Write<uint8_t>(0x48);
}

void JobAlias (const boost::shared_ptr<OPacket> &pkt, const JOB_MSG_TYPE type, const std::string &alias)
{
    pkt->WriteOpcode(SERVER_JOB_ALIAS);

    if (type == JOB_ALIAS_EXISTS)
    {
        pkt->Write<uint8_t>(0x02);
        pkt->Write<uint8_t>(0x48);
        pkt->Write<uint16_t>(type);
    }
    else
    {
        pkt->Write<uint8_t>(0x01);
        pkt->Write<uint8_t>(type);
    }

    pkt->Write(alias);
}

void JobPastInfo (const boost::shared_ptr<OPacket> &pkt, const JOB_ERROR &error)
{
    pkt->WriteOpcode(SERVER_JOB_PAST_INFO,ANSWER_ERROR,error);
    pkt->Write<uint8_t>(0x48);
}

void JobRanking (const boost::shared_ptr<OPacket> &pkt, const uint8_t job, const uint8_t type,
    const std::vector<Job> &list)
{
    pkt->WriteOpcode(SERVER_JOB_RANKING,ANSWER_ACCEPT);
    pkt->Write<uint8_t>(job);
    pkt->Write<uint8_t>(type);
    pkt->Write<uint8_t>(list.size());

    for (size_t i = 0; i < list.size(); ++i)
    {
        pkt->Write<uint8_t>(i);
        pkt->Write(list[i].get_alias());
        pkt->Write<uint8_t>(list[i].get_level());
        pkt->Write<uint32_t>(list[i].get_experience());
    }
}

void JobPayment (const boost::shared_ptr<OPacket> &pkt, const uint64_t amount)
{
    pkt->WriteOpcode(SERVER_JOB_PAYMENT,ANSWER_ACCEPT);
    pkt->Write<uint64_t>(amount);
}

void JobLeave (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_JOB_LEAVE,ANSWER_ACCEPT);
}

void JobTradeScale (const boost::shared_ptr<OPacket> &pkt, uint8_t scale)
{
    pkt->WriteOpcode(SERVER_JOB_TRADE_SCALE);
    pkt->Write<uint8_t>(scale);
}

void JobXP (const boost::shared_ptr<OPacket> &pkt, const uint8_t type, const uint8_t level, const int32_t XP)
{
    pkt->WriteOpcode(SERVER_JOB_XP);
    pkt->Write<uint8_t>(type);
    pkt->Write<uint8_t>(level);
    pkt->Write<int32_t>(XP);
}

void JobProductInformation (const boost::shared_ptr<OPacket> &pkt, const Market &market)
{
    pkt->WriteOpcode(SERVER_JOB_PRODUCT_INFORMATION);
    pkt->Write<uint16_t>(market.Chart.size());

    std::map<uint32_t,MarketStock>::const_iterator it;
    for (it = market.Chart.begin(); it != market.Chart.end(); ++it)
    {
        pkt->Write<uint32_t>(it->first);
        pkt->Write<uint16_t>(Profit(it->second,it->second.QuantitySold,it->second.QuantitySold,market.Max));
    }
}

}
