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
#include "connection.hpp"
#include "packet/packet_manager_gateway.hpp"

#include "query/account_query.hpp"

/// SRNL INCLUDES
#include <packet.hpp>

#include <boost/bind.hpp>

Server::Server ()
    :   m_db_conn(true),m_db_idle_timer(m_io_service)
{
    m_IDS = "ManagerServer";
}

Server::~Server ()
{
}

void Server::BroadcastShard (const boost::shared_ptr<OPacket> &p)
{
    for (conn_iterator i = m_conn_list.begin(); i != m_conn_list.end(); ++i)
    {
        if (i->second->GetCurrentState() == STATE_SHARD)
            i->second->Send(p);
    }
}

void Server::BroadcastGateway (const boost::shared_ptr<OPacket> &p)
{
    for (conn_iterator i = m_conn_list.begin(); i != m_conn_list.end(); ++i)
    {
        if (i->second->GetCurrentState() == STATE_GATEWAY)
            i->second->Send(p,false);
    }
}

void Server::BroadcastEmblem (const boost::shared_ptr<OPacket> &p)
{
    for (conn_iterator i = m_conn_list.begin(); i != m_conn_list.end(); ++i)
    {
        if (i->second->GetCurrentState() == STATE_EMBLEM)
            i->second->Send(p);
    }
}

void Server::BroadcastDownload (const boost::shared_ptr<OPacket> &p)
{
    for (conn_iterator i = m_conn_list.begin(); i != m_conn_list.end(); ++i)
    {
        if (i->second->GetCurrentState() == STATE_DOWNLOAD)
            i->second->Send(p);
    }
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

    boost::shared_ptr<OPacket> p(new OPacket);
    srv_pkt::WriteShard(p,shard);

    BroadcastGateway(p);
}

void Server::UpdateShard (const uint8_t type, const uint16_t ID, const uint16_t arg)
{
    shard_iterator i = m_shards.find(ID);

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

        boost::shared_ptr<OPacket> p(new OPacket);
        srv_pkt::WriteShard(p,(SHARD_NOTIFY)type,ID,arg);

        BroadcastGateway(p);
    }
}

mysqlpp::Connection* Server::DBConnection ()
{
    return &m_db_conn;
}

bool Server::OnInitialize ()
{
    if (m_LogPath.empty())
    {
        m_last_error = "Log directory entry empty. Check your configuration file.";
        return false;
    }

    try
    {
        m_db_conn.connect(m_db_name.c_str(),m_db_ip.c_str(),m_db_user.c_str(),m_db_password.c_str());

        mysqlpp::Query query = m_db_conn.query("select * from news order by date limit 0,4");

        mysqlpp::StoreQueryResult res = query.store();

        for (size_t i = 0; i < res.num_rows(); ++i)
        {
            News news;
            news.id = res[i]["id"];
            news.heading = (std::string)res[i]["title"];
            news.message = (std::string)res[i]["message"];
            news.day = 7;
            news.month = 9;

            m_news.insert(std::make_pair(news.id,news));
        }

        DB::Account::Clear clr_query;
        clr_query(&m_db_conn);

        m_db_idle_timer.expires_from_now(boost::posix_time::seconds(5));
        m_db_idle_timer.async_wait(boost::bind(&Server::OnIdleTimeout,this,boost::asio::placeholders::error));
    }
    catch (const mysqlpp::ConnectionFailed &error)
    {
        m_last_error = "Failed to establish a connection to the database.";
        return false;
    }
    catch (const mysqlpp::BadQuery &error)
    {
        m_last_error = error.what();
        return false;
    }

    return true;
}

void Server::OnConfigure (const std::map<std::string,std::string> &config_entries)
{
    typedef std::map<std::string,std::string>::const_iterator conf_const_iterator;

    for (conf_const_iterator it = config_entries.begin(); it != config_entries.end(); ++it)
    {
        if (it->first == "ListenPort")
            m_port = atoi(it->second.c_str());    ///TODO: CHECK FOR ATOI FAILURE
        else if (it->first == "ConnectionTimeout")
            m_connection_timeout = atoi(it->second.c_str());
        else if (it->first == "ConnectionMax")
            m_connection_max_count = atoi(it->second.c_str());
        else if (it->first == "Encryption")
            m_is_encryption_active = static_cast<bool>(atoi(it->second.c_str()));
        else if (it->first == "DatabaseIP")
            m_db_ip = it->second;
        else if (it->first == "DatabaseName")
            m_db_name = it->second;
        else if (it->first == "DatabaseUser")
            m_db_user = it->second;
        else if (it->first == "DatabasePassword")
            m_db_password = it->second;
        else if (it->first == "ClientVersion")
            m_client_version = atoi(it->second.c_str());
        else if (it->first == "Locale")
            m_locale = atoi(it->second.c_str());
        else if (it->first == "LogPath")
            m_LogPath = it->second;
    }
}

void Server::OnRemoveConnection (const uint32_t ID)
{
    conn_iterator it = m_conn_list.find(ID);

    if (it != m_conn_list.end())
    {
        if (it->second->GetCurrentState() == STATE_SHARD)
        {
            boost::shared_ptr<OPacket> p(new OPacket);
            srv_pkt::WriteShard(p,ID);

            BroadcastGateway(p);
        }
    }
}

void Server::OnIdleTimeout (const boost::system::error_code &error)
{
    if (!error)
    {
        mysqlpp::Query query = m_db_conn.query();

        query << "select * from serv_config";

        query.store();

        m_db_idle_timer.expires_from_now(boost::posix_time::hours(5));
        m_db_idle_timer.async_wait(boost::bind(&Server::OnIdleTimeout,this,boost::asio::placeholders::error));
    }
}

void Server::CreateConnection ()
{
    m_pending_conn.reset(new Connection(++m_counter,m_io_service,this));
}
