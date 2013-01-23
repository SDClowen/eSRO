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

#ifndef __MASTER_SERVER_STATE_EMBLEM_HPP__
#define __MASTER_SERVER_STATE_EMBLEM_HPP__

#include <server_connection_state.hpp>

class Server;
class Connection;

class StateEmblem : public srv::ConnectionState
{
public:

    StateEmblem (Connection *conn, Server *srv);

    virtual ~StateEmblem ();

    virtual void Close ();

private:

    Server *m_server;
};

#endif //__MASTER_SERVER_STATE_EMBLEM_HPP__
