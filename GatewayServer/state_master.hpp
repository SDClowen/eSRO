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

#ifndef __GATEWAY_SERVER_STATE_MASTER_HPP__
#define __GATEWAY_SERVER_STATE_MASTER_HPP__

/// SRNL INCLUDES
#include <client_connection_state.hpp>

class Server;
class Client;

class StateMaster : public clt::ConnectionState
{
public:

    StateMaster (Server *srv, Client *conn);

    virtual ~StateMaster ();

    virtual void Close ();

protected:

    int OnConfig (IPacket &packet);

	int OnAccountProcess (IPacket &packet);

	int OnNewsInsert (IPacket &packet);

	int OnNewsRemove (IPacket &packet);

	int OnShardInsert (IPacket &packet);

	int OnShardRemove (IPacket &packet);

	int OnShardNotify (IPacket &packet);

	int OnPatchInsert (IPacket &packet);

	int OnPatchRemove (IPacket &packet);

	int OnPatchNotify (IPacket &packet);

private:

    Server *m_server;
};

#endif ///__GATEWAY_SERVER_STATE_MASTER_HPP__
