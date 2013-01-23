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

/// SRNL INCLUDES
#include <news.hpp>
#include <shard.hpp>
#include <server_interface.hpp>

class OPacket;
class Client;

class Server : public srv::IServer
{
public:

    Server ();

    virtual ~Server ();

    void ConnectToMaster ();

    void SendMaster (const boost::shared_ptr<OPacket> &pkt);

    void SetLocale (const uint8_t locale);

    void SetClientVersion (const uint32_t version);

    void SetClientID (const std::string &ID);

    void SetGlobalVersion (const uint8_t version);

    void SetServerListID (const std::string &ID);

    const std::string& ClientID () const;

    const std::string& ServerListID () const;

    const std::map<uint32_t,News>& NewsList () const;

    const std::map<uint16_t,Shard>& ShardList () const;

    void InsertShard (const Shard &shard);

    void RemoveShard (const uint32_t shardID);

    void UpdateShard (const uint8_t type, const uint16_t shardID, const uint16_t arg);

    void InsertNews (const News& news);

    void RemoveNews (const uint32_t newsID);

protected:

    virtual bool OnInitialize ();

    virtual void OnConfigure (const std::map<std::string,std::string> &config_entries);

    virtual void OnRemoveConnection (const uint32_t ID);

    virtual void CreateConnection ();

private:

    std::string m_client_id;
    std::string m_server_list_id;

    std::map<uint32_t,News> m_news;
    std::map<uint16_t,Shard> m_shards;

    Client *m_client;
};

#endif //__GATEWAY_SERVER_HPP__
