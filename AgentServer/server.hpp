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

#ifndef __GATEWAY_SERVER_HPP__
#define __GATEWAY_SERVER_HPP__

#include "login_request.hpp"

/// SRNL INCLUDES
#include <shard.hpp>
#include <server_interface.hpp>

/// MYSQL INCLUDES
#include <mysql++/mysql++.h>

class OPacket;
class Channel;

class World;

namespace clt
{
    class Connection;
}

namespace mysqlpp
{
    class Connection;
}

class Server : public srv::IServer
{
public:

    typedef std::map<int32_t,boost::shared_ptr<Channel> >::iterator channel_iterator;
    typedef std::map<int32_t,boost::shared_ptr<Channel> >::const_iterator channel_const_iterator;

    Server ();

    virtual ~Server ();

    void ConnectToMaster ();

    void SendMaster (const boost::shared_ptr<OPacket> &pkt);

    void SendChannel (const boost::shared_ptr<OPacket> &pkt, const int32_t channelID, bool priv = false);

    void SendChannel (const boost::shared_ptr<OPacket> &p1, const boost::shared_ptr<OPacket> &p2,
        const boost::shared_ptr<OPacket> &p3, const int32_t channelID, bool priv = false);

    void InsertChannel (const int32_t channelID, const boost::shared_ptr<Channel> &channel);

    void InsertChannelConnection (const int32_t channelID, const boost::shared_ptr<srv::IConnection> &conn);

    void RemoveChannel (const int32_t channelID);

    void RemoveChannelConnection (const int32_t channelID, const uint32_t connectionID);

    bool JoinAdjacentChannels (const uint32_t connectionID, const uint8_t min_x, const uint8_t max_x,
        const uint8_t min_y, const uint8_t max_y);

    bool LeaveAdjacentChannels (const uint32_t connectionID, const uint8_t min_x, const uint8_t max_x,
        const uint8_t min_y, const uint8_t max_y);

    /**
     *
     *  @brief Send a packets to all players current ingame.
     *  @pkt: packet to send, its cleared after broadcasting.
     *
     **/

    void Broadcast (const boost::shared_ptr<OPacket> &pkt);

    /**
     *
     *  @brief Get Shard server information, max players, IP, port, etc
     *
     **/

    const Shard& GetShardInformation () const;

     /**
     *
     *  @brief Set server configuration.
     *
     *  @ID assigned global ID.
     *  @filter_ch string that contains the invalid name characters.
     *  @vstr list that contains filtered names or substrs.
     *
     **/

    void SetConfig (const uint16_t ID, const uint8_t locale, const uint8_t allowed_race,
        const std::string &filter_ch, const std::vector<std::string>& vstr);

    void QueueRequest (const LoginRequest &request);

    LoginRequest FindRequest (const uint32_t ID);

    bool IsIPSlotAvailable (const std::string &IP) const;

    void UseIPSlot (const std::string &IP);

    bool IsWorldSlotAvailable (const uint8_t access_lv , const uint8_t sec_lv) const;

    void UseWorldSlot ();

    mysqlpp::Connection& DBConnection ();

    World* GetWorld();

    boost::asio::io_service& GetService ();

protected:

    virtual bool OnInitialize ();

    virtual void OnConfigure (const std::map<std::string,std::string> &config_entries);

    virtual void OnRemoveConnection (const uint32_t ID);

    virtual void CreateConnection ();

    void OnIdleTimeout (const boost::system::error_code &error);

private:

    int m_connection_ip_max;
    uint8_t m_shard_count;          ///Number of shards assigned.
    uint8_t m_shard_actives;        ///Shards current initialized and ready to run.
    Shard m_shard_info;
    clt::Connection *m_client;

    std::map<uint32_t,LoginRequest> m_pending_logins;       ///Pending Login Requests.
    std::map<std::string,uint8_t> m_ip_histogram;           ///Numbers of connection per IP.

    uint8_t m_allowed_race;
    std::string m_ch_filter;                            ///< Character filter string.
    std::vector<std::string> m_substr_filter_list;      ///< Name filter list.

    World *m_world;
    mysqlpp::Connection m_db_conn;

    std::string m_db;
    std::string m_db_host;
    std::string m_db_usr;
    std::string m_db_pw;

    boost::mutex m_channel_mutex;
    std::map<int32_t,boost::shared_ptr<Channel> > m_channel_groups;

    boost::asio::deadline_timer m_db_idle_timer;
};

#endif //__GATEWAY_SERVER_HPP__
