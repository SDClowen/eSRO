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

#include "server_state_game.hpp"

/// SRDL INCLUDES
#include <message_query.hpp>

/// SRNL INCLUDES
#include <packet_message.hpp>

/*
void MessageRead::operator() (IPacket &packet)
{
	uint8_t msg_number = packet.Read<uint8_t>();

	if (!packet.EndOfStream())
		throw Exception::InvalidPacketSize();

	DB::MESSAGE::Select select_query;
	std::string msg = select_query(m_conn,m_player->get_name(),msg_number);

	if (!msg.empty())
		send(PacketMsgRead(msg_number,msg));
}

void MessageDelete::operator() (IPacket &packet)
{
	enum { MAX_MSGS = 20 };

	uint8_t msg_num = packet.Read<uint8_t>();

	if (!packet.EndOfStream())
		throw Exception::InvalidPacketSize();

	if (msg_num >= MAX_MSGS)
		throw Exception::InvalidPacketArg();

	DB::MESSAGE::Delete query;
	query(m_conn,m_player->get_name(),msg_num);

	send(PacketMsgDelete(msg_num));
}

void MessageSend::operator() (IPacket &packet)
{
	std::string name = packet.Read();

	std::string msg = packet.Read();

	if (!packet.EndOfStream())
		throw Exception::InvalidPacketSize();

	DB::MESSAGE::Valid valid_query;

	if (valid_query(m_conn,name))
	{
		DB::MESSAGE::Max max_query;

		if (max_query(m_conn,name))
			send(PacketMsgSend(ERROR_MSG_SEND_INBOX_FULL));
		else
		{
			DB::MESSAGE::Insert insert_query;
			insert_query(m_conn,name,m_player->get_name(),msg,m_player->get_model());

			send(PacketMsgSend());

			send_to(PacketMsgReceive(m_player->get_name(),m_player->get_model()),name);
		}
	}
	else
		send(PacketMsgSend(ERROR_MSG_SEND_INVALID));
}

void MessageBroadcast::operator() (IPacket &packet)
{
	enum
	{
		MESSAGE_GUILD = 1,
		MESSAGE_UNION
	};

	uint8_t type = packet.Read<uint8_t>();

	uint8_t recv_size = packet.Read<uint8_t>();

	std::string name;

	std::vector<std::string> name_list;
	for ( int j = 0; j < recv_size; ++j)
	{
		name = packet.Read();

		name_list.push_back(name);
	}

	std::string msg = packet.Read();

	if (!packet.EndOfStream())
		throw Exception::InvalidPacketSize();

	///if (type == MESSAGE_GUILD && m_player->get_guild().empty())
	///	return;

	DB::MESSAGE::Max max_query;
	for (std::vector<std::string>::const_iterator i = name_list.begin(); i != name_list.end(); ++i)
	{
		if (max_query(m_conn,*i))
			return;
	}

	std::for_each (name_list.begin(),name_list.end(),
				   boost::bind(&MessageBroadcast::send_message,this,_1,msg));

	send(PacketMsgBroadcast());
}

void MessageBroadcast::send_message (const std::string &name, const std::string &msg)
{
	DB::MESSAGE::Insert insert_query;
	insert_query(m_conn,name,m_player->get_name(),msg,m_player->get_model());

	send_to(PacketMsgReceive(m_player->get_name(),m_player->get_model()),name);
}

*/
