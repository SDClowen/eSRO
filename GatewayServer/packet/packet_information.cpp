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

#include "packet_information.hpp"
#include "packet.hpp"
#include "opcodes_global_server.hpp"
#include "opcodes_gateway_server.hpp"
#include "opcodes_gateway_server.hpp"

namespace srv_pkt
{

void WriteNews (const boost::shared_ptr<OPacket> &pkt, const std::map<uint32_t,News> &news)
{
    pkt->WriteOpcode(SERVER_INFORMATION);
	pkt->Write<uint8_t>(0x00);
	pkt->Write<uint8_t>(news.size());

    for (std::map<uint32_t,News>::const_iterator i = news.begin(); i != news.end(); ++i)
	{
		pkt->Write(i->second.heading);
		pkt->Write(i->second.message);
		pkt->Write<uint16_t>(0x07D8);
		pkt->Write<uint16_t>(i->second.month);	//Month
		pkt->Write<uint16_t>(i->second.day);	//Day
		pkt->Write<uint32_t>(0x0025000B);
		pkt->Write<uint32_t>(0xAFC00012);
		pkt->Write<uint16_t>(0x35D2);
	}
}

void WriteShards (const boost::shared_ptr<OPacket> &pkt, const std::string &name, const uint8_t locale,
    const std::map<uint16_t,Shard> &servers)
{
    pkt->WriteOpcode(SERVER_GAME_SERVER_LIST);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
	pkt->Write<uint8_t>(locale);
	pkt->Write(name);
	pkt->Write<uint8_t>(0x00);

    for (std::map<uint16_t,Shard>::const_iterator i = servers.begin(); i != servers.end(); ++i)
	{
		pkt->Write<uint8_t>(i->second.is_new);
		pkt->Write<uint16_t>(i->second.id);
		pkt->Write(i->second.name);
		pkt->Write<uint16_t>(i->second.usr_cnt);
		pkt->Write<uint16_t>(i->second.usr_max);
		pkt->Write<uint8_t>(i->second.state);
	}

	pkt->Write<uint8_t>(0x00);
}



}
