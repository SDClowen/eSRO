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

#include "packet_manager_gateway.hpp"

/// SRNL INCLUDES
#include <packet.hpp>
#include <opcodes_manager_gateway_server.hpp>

namespace srv_pkt
{

void WriteConfig (const boost::shared_ptr<OPacket> &p, const uint8_t locale, const uint32_t client_version,
    const std::map<uint16_t,Shard> &serv_list, const std::map<uint32_t,News> &news_list)
{
    p->WriteOpcode(SERVER_CONFIG);
    p->Write<uint8_t>(locale);
    p->Write<uint32_t>(client_version);

	p->Write<uint8_t>(serv_list.size());
    for (std::map<uint16_t,Shard>::const_iterator i = serv_list.begin(); i != serv_list.end(); ++i)
	{
		p->Write<uint8_t>(i->second.is_new);
		p->Write(i->second.name);
		p->Write<uint16_t>(i->second.id);
		p->Write<uint16_t>(i->second.usr_cnt);
		p->Write<uint16_t>(i->second.usr_max);
		p->Write<uint8_t>(i->second.state);
		p->Write(i->second.ip);
		p->Write<uint16_t>(i->second.port);
	}

	p->Write<uint8_t>(news_list.size());
    for (std::map<uint32_t,News>::const_iterator i = news_list.begin(); i != news_list.end(); ++i)
	{
		p->Write<uint32_t>(i->second.id);
		p->Write(i->second.heading);
		p->Write(i->second.message);
		p->Write<uint16_t>(i->second.month);
		p->Write<uint16_t>(i->second.day);
	}
}

void WriteShard (const boost::shared_ptr<OPacket> &p, const Shard &serv)
{
    p->WriteOpcode(SERVER_SHARD_INSERT);
    p->Write<uint8_t>(serv.is_new);
    p->Write(serv.name);
    p->Write<uint16_t>(serv.id);
    p->Write<uint16_t>(serv.usr_cnt);
    p->Write<uint16_t>(serv.usr_max);
    p->Write<uint8_t>(serv.state);
    p->Write(serv.ip);
    p->Write<uint16_t>(serv.port);
}

void WriteShard (const boost::shared_ptr<OPacket> &p, const uint16_t ID)
{
    p->WriteOpcode(SERVER_SHARD_REMOVE);
    p->Write<uint16_t>(ID);
}

void WriteShard (const boost::shared_ptr<OPacket> &p, const SHARD_NOTIFY type, const uint16_t ID, const uint16_t arg)
{
    p->WriteOpcode(SERVER_SHARD_NOTIFY);
    p->Write<uint8_t>(type);
    p->Write<uint16_t>(ID);
    p->Write<uint16_t>(arg);
}

void WriteLogin (const boost::shared_ptr<OPacket> &pkt, const uint32_t clientID, const uint32_t queueID,
    const Shard &shard)
{
    pkt->WriteOpcode(SERVER_ACCOUNT_PROCESS);
    pkt->Write<uint32_t>(clientID);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
    pkt->Write<uint32_t>(queueID);
    pkt->Write(shard.ip);
    pkt->Write<uint16_t>(shard.port);
	pkt->Write<uint8_t>(0x03);	//NOT SURE I ONLY SEE IT ON KSRO
}

void WriteLogin (const boost::shared_ptr<OPacket> &pkt, const uint32_t clientID, const LOGIN_ERROR error)
{
    pkt->WriteOpcode(SERVER_ACCOUNT_PROCESS);
    pkt->Write<uint32_t>(clientID);
    pkt->Write<uint8_t>(ANSWER_ERROR);
    pkt->Write<uint8_t>(error);
}

void WriteLogin (const boost::shared_ptr<OPacket> &pkt, const uint32_t clientID, const LOGIN_ERROR error,
    const uint32_t max, const uint32_t count)
{
    pkt->WriteOpcode(SERVER_ACCOUNT_PROCESS);
    pkt->Write<uint32_t>(clientID);
    pkt->Write<uint8_t>(ANSWER_ERROR);
    pkt->Write<uint8_t>(error);
    pkt->Write<uint32_t>(max);
    pkt->Write<uint32_t>(count);
}

void WriteLogin (const boost::shared_ptr<OPacket> &pkt, const uint32_t clientID, const LOGIN_ERROR error,
    const std::string &msg, const uint16_t year, const uint8_t month, const uint8_t day, const uint8_t hour,
    const uint8_t min)
{
    pkt->WriteOpcode(SERVER_ACCOUNT_PROCESS);
    pkt->Write<uint32_t>(clientID);
    pkt->Write<uint8_t>(ANSWER_ERROR);
    pkt->Write<uint8_t>(error);
    pkt->Write<uint8_t>(1);
    pkt->Write(msg);
    pkt->Write<uint16_t>(year);
    pkt->Write<uint16_t>(month);
    pkt->Write<uint16_t>(day);
    pkt->Write<uint16_t>(hour);
    pkt->Write<uint16_t>(min);
    pkt->Write<uint16_t>(0);
    pkt->Write<uint16_t>(0);
    pkt->Write<uint16_t>(0);
}

}
