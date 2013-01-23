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

#ifndef __MASTER_SERVER_HPP__
#define __MASTER_SERVER_HPP__

///SRNL INCLUDES
#include <news.hpp>
#include <shard.hpp>
#include <server_interface.hpp>

/// MYSQL INCLUDES
#include <mysql++/mysql++.h>

class OPacket;

class Server : public srv::IServer
{
public:

    typedef std::map<uint16_t,Shard> ::iterator shard_iterator;
    typedef std::map<uint16_t,Shard>::const_iterator shard_const_iterator;

    Server ();

    virtual ~Server ();

    void BroadcastShard (const boost::shared_ptr<OPacket> &p);

    void BroadcastGateway (const boost::shared_ptr<OPacket> &p);

    void BroadcastEmblem (const boost::shared_ptr<OPacket> &p);

    void BroadcastDownload (const boost::shared_ptr<OPacket> &p);

    const std::map<uint32_t,News>& NewsList () const;

    const std::map<uint16_t,Shard>& ShardList () const;

    void InsertShard (const Shard &shard);

    void UpdateShard (const uint8_t type, const uint16_t ID, const uint16_t arg);

    mysqlpp::Connection* DBConnection ();

protected:

    virtual bool OnInitialize ();

    virtual void OnConfigure (const std::map<std::string,std::string> &config_entries);

    virtual void OnRemoveConnection (const uint32_t ID);

    virtual void CreateConnection ();

    void OnIdleTimeout (const boost::system::error_code &error);

private:

    mysqlpp::Connection m_db_conn;

    std::string m_db_ip;
    std::string m_db_name;
    std::string m_db_user;
    std::string m_db_password;

    std::map<uint32_t,News> m_news;
    std::map<uint16_t,Shard> m_shards;

    boost::asio::deadline_timer m_db_idle_timer;
};

#endif  ///__MASTER_SERVER_HPP__
