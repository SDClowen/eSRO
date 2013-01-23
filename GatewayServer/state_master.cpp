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

#include "state_master.hpp"
#include "server.hpp"
#include "client_connection.hpp"

/// SRNL INCLUDES
#include <shard.hpp>
#include <news.hpp>
#include <opcodes_gateway_server.hpp>
#include <opcodes_manager_gateway_server.hpp>
#include <iostream>
StateMaster::StateMaster (Server *srv, Client *conn)
    : clt::ConnectionState(conn), m_server(srv)
{
    using boost::bind;

    m_opcode_table[SERVER_CONFIG] = bind(&StateMaster::OnConfig,this,_1);
    m_opcode_table[SERVER_ACCOUNT_PROCESS] = bind(&StateMaster::OnAccountProcess,this,_1);
    m_opcode_table[SERVER_NEWS_INSERT] = bind(&StateMaster::OnNewsInsert,this,_1);
    m_opcode_table[SERVER_NEWS_REMOVE] = bind(&StateMaster::OnNewsRemove,this,_1);
    m_opcode_table[SERVER_SHARD_INSERT] = bind(&StateMaster::OnShardInsert,this,_1);
    m_opcode_table[SERVER_SHARD_REMOVE] = bind(&StateMaster::OnShardRemove,this,_1);
    m_opcode_table[SERVER_SHARD_NOTIFY] = bind(&StateMaster::OnShardNotify,this,_1);
    m_opcode_table[SERVER_PATCH_INSERT] = bind(&StateMaster::OnPatchInsert,this,_1);
    m_opcode_table[SERVER_PATCH_REMOVE] = bind(&StateMaster::OnPatchRemove,this,_1);
    m_opcode_table[SERVER_PATCH_NOTIFY] = bind(&StateMaster::OnPatchNotify,this,_1);
}

StateMaster::~StateMaster ()
{
}

void StateMaster::Close ()
{
}

int StateMaster::OnConfig (IPacket &packet)
{
    uint8_t locale = packet.Read<uint8_t>();

    uint32_t client_version = packet.Read<uint32_t>();

    uint8_t serv_size = packet.Read<uint8_t>();

    Shard serv;
    std::vector<Shard> shard_list;

    for (int i = 0; i < serv_size; ++i)
    {
        serv.is_new = packet.Read<uint8_t>();
        serv.name = packet.Read();
        serv.id = packet.Read<uint16_t>();
        serv.usr_cnt = packet.Read<uint16_t>();
        serv.usr_max = packet.Read<uint16_t>();
        serv.state = packet.Read<uint8_t>();
        serv.ip = packet.Read();
        serv.port = packet.Read<uint16_t>();

        shard_list.push_back(serv);
    }

    uint8_t news_size = packet.Read<uint8_t>();

    News news;
    std::vector<News> news_list;

    for (int i = 0; i < news_size; ++i)
    {
        news.id = packet.Read<uint32_t>();
        news.heading = packet.Read();
        news.message = packet.Read();
        news.month = packet.Read<uint16_t>();
        news.day = packet.Read<uint16_t>();

        news_list.push_back(news);
    }

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    m_server->SetLocale(locale);

    m_server->SetClientVersion(client_version);

    std::for_each(shard_list.begin(),shard_list.end(),boost::bind(&Server::InsertShard,m_server,_1));

    std::for_each(news_list.begin(),news_list.end(),boost::bind(&Server::InsertNews,m_server,_1));

    m_server->Execute(false);

    return MSG_SUCCESS;
}

int StateMaster::OnAccountProcess (IPacket &packet)
{
    /// FOWARD THE RESPONSE FROM THE MASTER SERVER TO THE RESPECTIVE CLIENT
    uint32_t clientID = packet.Read<uint32_t>();

    uint8_t ret = packet.Read<uint8_t>();

    boost::shared_ptr<OPacket> pkt(new OPacket);
    pkt->WriteOpcode(SERVER_LOGIN);
    pkt->Write<uint8_t>(ret);

    while (!packet.EndOfStream())
        pkt->Write<uint8_t>(packet.Read<uint8_t>());

    m_server->Send(pkt,clientID);

    return MSG_SUCCESS;
}

int StateMaster::OnNewsInsert (IPacket &packet)
{
    News news;
	news.id = packet.Read<uint32_t>();
	news.heading = packet.Read();
	news.message = packet.Read();
	news.month = packet.Read<uint16_t>();
	news.day = packet.Read<uint16_t>();

	if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

	m_server->InsertNews(news);

    return MSG_SUCCESS;
}

int StateMaster::OnNewsRemove (IPacket &packet)
{
    uint32_t ID = packet.Read<uint32_t>();

	if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

	m_server->RemoveNews(ID);

    return MSG_SUCCESS;
}

int StateMaster::OnShardInsert (IPacket &packet)
{
    std::cout << "OnShardInsert()" << std::endl;
	Shard serv;
	serv.is_new = packet.Read<uint8_t>();
	serv.name = packet.Read();
	serv.id = packet.Read<uint16_t>();
	serv.usr_cnt = packet.Read<uint16_t>();
	serv.usr_max = packet.Read<uint16_t>();
	serv.state = packet.Read<uint8_t>();
	serv.ip = packet.Read();
	serv.port = packet.Read<uint16_t>();

	if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

	m_server->InsertShard(serv);

    return MSG_SUCCESS;
}

int StateMaster::OnShardRemove (IPacket &packet)
{
    std::cout << "OnShardRemove()" << std::endl;

	uint16_t ID = packet.Read<uint16_t>();

	if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

	m_server->RemoveShard(ID);

    return MSG_SUCCESS;
}

int StateMaster::OnShardNotify (IPacket &packet)
{
    enum
	{
		SHARD_NOTIFY_USR_CNT,
		SHARD_NOTIFY_USR_MAX,
		SHARD_NOTIFY_STATE
	};

    uint8_t action = packet.Read<uint8_t>();

	uint16_t ID = packet.Read<uint16_t>();

	uint16_t arg = packet.Read<uint16_t>();

	if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

	switch(action)
	{
	case SHARD_NOTIFY_USR_CNT:
	case SHARD_NOTIFY_USR_MAX:
	case SHARD_NOTIFY_STATE:
		m_server->UpdateShard(ID,action,arg);
		break;
	default:
		break;
	}

    return MSG_SUCCESS;
}

int StateMaster::OnPatchInsert (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    return MSG_SUCCESS;
}

int StateMaster::OnPatchRemove (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    return MSG_SUCCESS;
}

int StateMaster::OnPatchNotify (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    return MSG_SUCCESS;
}
