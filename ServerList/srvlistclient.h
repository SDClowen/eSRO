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

#ifndef SRVLISTCLIENT_H
#define SRVLISTCLIENT_H

#include <shard.hpp>
#include <host_info.hpp>
#include <client_connection_interface.hpp>

enum CONNECTION_STATES
{
    STATE_HANDSHAKE,
    STATE_VERSION,
    STATE_DOWNLOAD,
    STATE_LOGIN
};

class networkthread;

class SrvListClient : public clt::IConnection
{
public:

    SrvListClient(boost::asio::io_service &io_service, const HostDivisionInfo *info, networkthread *thread);

    virtual ~SrvListClient ();

    void SetShardList(const std::vector<Shard> &list);

    const std::vector<Shard>& ShardList () const;

private:

    std::vector<std::string> m_gw_list;
    std::vector<Shard> m_shard_list;

    networkthread *m_thread;
};

#endif // SRVLISTCLIENT_H
