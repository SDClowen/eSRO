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
#include "world.hpp"
#include "server_connection.hpp"
#include "server_channel.hpp"
#include "client_connection.hpp"

#include <packet.hpp>
#include <packet_shard.hpp>

#include <syslog.h>

Server::Server ()
    : m_connection_ip_max(0), m_client(0), m_world(new World(this)), m_db_conn(false), m_db_idle_timer(m_io_service)
{
    m_IDS = "AgentServer";
    m_shard_actives = 0;
    m_shard_count = 0;
    m_shard_info.usr_cnt = 0;
    m_shard_info.state = 1;
}

Server::~Server ()
{
    if (m_client)
        delete m_client;

    if (m_world)
    {
        m_world->Stop();
        delete m_world;
    }
}

void Server::ConnectToMaster ()
{
    m_client->Execute();

    m_world->Run();

    m_io_service.run();
}

void Server::SendMaster (const boost::shared_ptr<OPacket> &pkt)
{
    m_client->Send(pkt);
}

void Server::SendChannel (const boost::shared_ptr<OPacket> &pkt, const int32_t channelID, bool priv)
{
    boost::mutex::scoped_lock lock(m_channel_mutex);

    channel_iterator iter = m_channel_groups.find(channelID);

    if (iter != m_channel_groups.end())
        iter->second->Send(pkt,priv);
}

void Server::SendChannel (const boost::shared_ptr<OPacket> &p1, const boost::shared_ptr<OPacket> &p2,
    const boost::shared_ptr<OPacket> &p3, const int32_t channelID, bool priv)
{
    boost::mutex::scoped_lock lock(m_channel_mutex);

    channel_iterator iter = m_channel_groups.find(channelID);

    if (iter != m_channel_groups.end())
        iter->second->Send(p1,p2,p3,priv);
}

void Server::InsertChannel(const int32_t channelID, const boost::shared_ptr<Channel> &channel)
{
    boost::mutex::scoped_lock lock(m_channel_mutex);

    m_channel_groups.insert(std::make_pair(channelID,channel));
}

void Server::InsertChannelConnection (const int32_t channelID, const boost::shared_ptr<srv::IConnection> &conn)
{
    boost::mutex::scoped_lock lock(m_channel_mutex);

    channel_iterator iter = m_channel_groups.find(channelID);

    if (iter != m_channel_groups.end())
        iter->second->InsertConnection(conn);
}

void Server::RemoveChannel(const int32_t channelID)
{
    boost::mutex::scoped_lock lock(m_channel_mutex);

    m_channel_groups.erase(channelID);
}

void Server::RemoveChannelConnection (const int32_t channelID, const uint32_t connectionID)
{
    boost::mutex::scoped_lock lock(m_channel_mutex);

    channel_iterator iter = m_channel_groups.find(channelID);

    if (iter != m_channel_groups.end())
        iter->second->RemoveConnection(connectionID);
}

bool Server::JoinAdjacentChannels (const uint32_t connectionID, const uint8_t min_x, const uint8_t max_x,
    const uint8_t min_y, const uint8_t max_y)
{
    boost::mutex::scoped_lock lock(m_conn_mutex);
    boost::mutex::scoped_lock conn_lock(m_channel_mutex);

    conn_iterator iter = m_conn_list.find(connectionID);

    if (iter == m_conn_list.end())
        return false;

    int16_t zone = 0;
    channel_iterator channel_iter;

    for ( uint8_t x = min_x; x <= max_x; ++x)
    {
        for (uint8_t y = min_y; y <= max_y; ++y)
        {
            zone = (y << 8) + x;
            channel_iter = m_channel_groups.find(zone);

            if (channel_iter != m_channel_groups.end())
                channel_iter->second->InsertConnection(iter->second);
        }
    }

    return true;
}

bool Server::LeaveAdjacentChannels (const uint32_t connectionID, const uint8_t min_x, const uint8_t max_x,
    const uint8_t min_y, const uint8_t max_y)
{
    boost::mutex::scoped_lock lock(m_conn_mutex);
    boost::mutex::scoped_lock conn_lock(m_channel_mutex);

    int16_t zone = 0;
    channel_iterator channel_iter;

    for ( uint8_t x = min_x; x <= max_x; ++x)
    {
        for (uint8_t y = min_y; y <= max_y; ++y)
        {
            zone = (y << 8) + x;
            channel_iter = m_channel_groups.find(zone);

            if (channel_iter != m_channel_groups.end())
                channel_iter->second->RemoveConnection(connectionID);
        }
    }

    return true;
}

void Server::Broadcast(const boost::shared_ptr<OPacket> &pkt)
{
    boost::mutex::scoped_lock lock(m_conn_mutex);

    for (conn_iterator it = m_conn_list.begin(); it != m_conn_list.end(); ++it)
    {
        if(it->second->GetCurrentState() == srv::STATE_GAME)
            it->second->Send(pkt,false,false);
    }
}

const Shard& Server::GetShardInformation () const
{
    return m_shard_info;
}

void Server::SetConfig (const uint16_t ID, const uint8_t locale, const uint8_t allowed_race,
        const std::string &ch_filter, const std::vector<std::string>& vstr)
{
    m_shard_info.id = ID;
    m_locale = locale;
    m_allowed_race = allowed_race;
    m_ch_filter = ch_filter;
    m_substr_filter_list = vstr;

    boost::shared_ptr<OPacket> pkt(new OPacket);
    clt_pkt::WriteShardNotify8(pkt,clt_pkt::SHARD_NOTIFY_STATE,m_shard_info.id,true);
    m_client->Send(pkt,true);
}

void Server::QueueRequest (const LoginRequest &request)
{
    m_pending_logins.insert(std::make_pair(request.ID,request));
}

LoginRequest Server::FindRequest(const uint32_t ID)
{
    LoginRequest ret;
    std::map<uint32_t,LoginRequest>::iterator iter = m_pending_logins.find(ID);

    if (iter != m_pending_logins.end())
    {
        ret = iter->second;
        m_pending_logins.erase(iter);
    }

    return ret;
}

bool Server::IsIPSlotAvailable (const std::string &IP) const
{
    enum { MAX_CONN_PER_IP = 2 };

    std::map<std::string,uint8_t>::const_iterator i = m_ip_histogram.find(IP);

    if (i != m_ip_histogram.end() && i->second >= 2)
        return false;

    return true;
}

void Server::UseIPSlot(const std::string &IP)
{
    ++m_ip_histogram[IP];
}

bool Server::IsWorldSlotAvailable(const uint8_t access_lv, const uint8_t sec_lv) const
{
    if (access_lv >= 1 || sec_lv >= 1)
        return true;

    if (m_shard_info.usr_cnt > m_shard_info.usr_max)
        return false;

    return true;
}

void Server::UseWorldSlot()
{
    ++m_shard_info.usr_cnt;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        clt_pkt::WriteShardNotify16(pkt,clt_pkt::SHARD_NOTIFY_USR_CNT,m_shard_info.id,m_shard_info.usr_cnt);
        m_client->Send(pkt);
    }
}

mysqlpp::Connection& Server::DBConnection()
{
    return m_db_conn;
}

World* Server::GetWorld()
{
    return m_world;
}

boost::asio::io_service& Server::GetService()
{
    return m_io_service;
}

bool Server::OnInitialize ()
{
    if (!m_client)
        return false;

    if (m_LogPath.empty())
    {
        m_last_error = "Log directory entry empty. Check your configuration file.";
        return false;
    }

    if (!m_connection_ip_max)
    {
        m_last_error = "Set the max amount of connections per IP.";
        return false;
    }

    if (!m_db_conn.connect(m_db.c_str(),m_db_host.c_str(),m_db_usr.c_str(),m_db_pw.c_str()))
    {
        return false;
    }

    if (!m_world->Initialize())
    {
        m_last_error = m_world->GetLastError();
        return false;
    }

    m_db_idle_timer.expires_from_now(boost::posix_time::hours(5));
    m_db_idle_timer.async_wait(boost::bind(&Server::OnIdleTimeout,this,boost::asio::placeholders::error));

    return true;
}

void Server::OnConfigure (const std::map<std::string,std::string> &config_entries)
{
    typedef std::map<std::string,std::string>::const_iterator const_iter;

    std::string host, port;

    for (const_iter it = config_entries.begin(); it != config_entries.end(); ++it)
    {
        if (it->first == "ListenPort")
            m_port = m_shard_info.port = atoi(it->second.c_str());
        else if (it->first == "ListenIP")
            m_shard_info.ip = it->second;
        else if (it->first == "ConnectionTimeout")
            m_connection_timeout = atoi(it->second.c_str());
        else if (it->first == "ConnectionMax")
            m_connection_max_count = m_shard_info.usr_max = atoi(it->second.c_str());
        else if (it->first == "ConnectionMaxPerIP")
            m_connection_ip_max = atoi(it->second.c_str());
        else if (it->first == "Encryption")
            m_is_encryption_active = static_cast<bool>(atoi(it->second.c_str()));
        else if (it->first == "ClientHost")
            host = it->second;
        else if (it->first == "ClientPort")
            port = it->second;
        else if (it->first == "ShardName")
            m_shard_info.name = it->second;
        else if (it->first == "ShardNew")
            m_shard_info.is_new = static_cast<bool>(atoi(it->second.c_str()));
        else if (it->first == "DatabaseName")
            m_db = it->second;
        else if (it->first == "DatabaseIP")
            m_db_host = it->second;
        else if (it->first == "DatabaseUser")
            m_db_usr = it->second;
        else if (it->first == "DatabasePassword")
            m_db_pw = it->second;
        else if (it->first == "LogPath")
            m_LogPath = it->second;
    }

    m_client = new clt::Connection(m_io_service,host,port,this);

    m_world->Configure(config_entries);
}

void Server::OnRemoveConnection (const uint32_t ID)
{
    conn_const_iterator it = m_conn_list.find(ID);

    if (it != m_conn_list.end())
    {
        boost::shared_ptr<srv::Connection> conn;
        conn = boost::shared_dynamic_cast<srv::Connection,srv::IConnection>(it->second);

        uint32_t accID = conn->AccountID();

        if (accID)
        {
#ifdef DEBUG
            syslog(LOG_INFO,"Connection Terminated.");
#endif
            boost::shared_ptr<OPacket> pkt(new OPacket);

            if (pkt)
            {
                clt_pkt::WriteAccountNotify8(pkt,clt_pkt::ACCOUNT_NOTIFY_STATE,accID,false);
                m_client->Send(pkt);
            }

            if (m_shard_info.usr_cnt)
                --m_shard_info.usr_cnt;
        }

        if (m_ip_histogram[conn->remote_endpoint()])
            m_ip_histogram[conn->remote_endpoint()] -= 1;
    }
}

void Server::OnIdleTimeout (const boost::system::error_code &error)
{
    if (!error)
    {
        mysqlpp::Query query = m_db_conn.query();
        
        query << "select * from reflevelgold where Level=1";

        query.store();

        m_db_idle_timer.expires_from_now(boost::posix_time::hours(5));
        m_db_idle_timer.async_wait(boost::bind(&Server::OnIdleTimeout,this,boost::asio::placeholders::error));
    }
}

void Server::CreateConnection ()
{
    m_pending_conn.reset(new srv::Connection(++m_counter,m_io_service,this));

    if (m_pending_conn)
        m_pending_conn->SetMonitor(true,m_LogPath);
}
