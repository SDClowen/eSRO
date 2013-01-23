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

#ifndef __SHARD_SERVER_CLIENT_STATE_MASTER_HPP__
#define __SHARD_SERVER_CLIENT_STATE_MASTER_HPP__

#include <client_connection_state.hpp>

class Server;

namespace clt
{

class Connection;

class StateMaster : public clt::ConnectionState
{
public:

    StateMaster (Server *srv, Connection *conn);

    virtual ~StateMaster ();

    virtual void Close ();

private:

    int OnConfig (IPacket &packet);

	int OnQueue (IPacket &packet);

	Server *m_server;
};

}

#endif  //__SHARD_SERVER_CLIENT_STATE_MASTER_HPP__
