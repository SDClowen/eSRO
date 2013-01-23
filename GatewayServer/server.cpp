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

#include "server.hpp"
#include "server_connection.hpp"
#include "client_connection.hpp"

#include <iostream>

Server::Server ()
    : m_client(0)
{
    m_IDS = "GatewayServer";

    /// TODO: SEND FROM MASTER SERVER
    m_client_id = "SR_Client";
    m_server_list_id = "Silkroad_Korea_Yahoo_Official";
}

Server::~Server ()
{
    if (m_client)
    {
        delete m_client;
        m_client = 0;
    }
}

void Server::ConnectToMaster ()
{
    m_client->Execute();

    m_io_service.run();
}

void Server::SendMaster (const boost::shared_ptr<OPacket> &pkt)
{
    m_client->Send(pkt);
}

void Server::SetLocale(const uint8_t locale)
{
    m_locale = locale;
}

void Server::SetClientVersion (const uint32_t version)
{
    m_client_version = version;
}

void Server::SetClientID (const std::string &ID)
{
    m_client_id = ID;
}

void Server::SetServerListID (const std::string &ID)
{
    m_server_list_id = ID;
}

const std::string& Server::ClientID () const
{
    return m_client_id;
}

const std::string& Server::ServerListID () const
{
    return m_server_list_id;
}

const std::map<uint32_t,News>& Server::NewsList () const
{
    return m_news;
}

const std::map<uint16_t,Shard>& Server::ShardList () const
{
    return m_shards;
}

void Server::InsertShard (const Shard &shard)
{
    m_shards.insert(std::make_pair(shard.id,shard));
}

void Server::RemoveShard (const uint32_t shardID)
{
    m_shards.erase(shardID);
}

void Server::UpdateShard (const uint8_t type, const uint16_t ID, const uint16_t arg)
{
    std::map<uint16_t,Shard>::iterator i = m_shards.find(ID);

    if (i != m_shards.end())
    {
        switch(type)
        {
        case SHARD_NOTIFY_USR_CNT:
            i->second.usr_cnt = arg;
            break;
        case SHARD_NOTIFY_USR_MAX:
            i->second.usr_max = arg;
            break;
        case SHARD_NOTIFY_STATE:
            i->second.state = arg;
            break;
        }
    }
}

void Server::InsertNews (const News& news)
{
    m_news.insert(std::make_pair(news.id,news));
}

void Server::RemoveNews (const uint32_t newsID)
{
    m_news.erase(newsID);
}

bool Server::OnInitialize ()
{
    if (m_LogPath.empty())
    {
        m_last_error = "Log directory entry empty. Check your configuration file.";
        return false;
    }

    return true;
}

void Server::OnConfigure (const std::map<std::string,std::string> &config_entries)
{
    typedef std::map<std::string,std::string>::const_iterator cfg_const_iterator;

    std::string host, port;

    for (cfg_const_iterator it = config_entries.begin(); it != config_entries.end(); ++it)
    {
        if (it->first == "ListenPort")
            m_port = atoi(it->second.c_str());
        else if (it->first == "ConnectionTimeout")
            m_connection_timeout = atoi(it->second.c_str());
        else if (it->first == "ConnectionMax")
            m_connection_max_count = atoi(it->second.c_str());
        else if (it->first == "Encryption")
            m_is_encryption_active = static_cast<bool>(atoi(it->second.c_str()));
        else if (it->first == "ClientHost")
            host = it->second;
        else if (it->first == "ClientPort")
            port = it->second;
        else if (it->first == "LogPath")
            m_LogPath = it->second;
    }

    m_client = new Client(m_io_service,host,port,this);
}

void Server::OnRemoveConnection (const uint32_t ID)
{
}

void Server::CreateConnection ()
{
    m_pending_conn.reset(new srv::Connection(++m_counter,m_io_service,this));
}
