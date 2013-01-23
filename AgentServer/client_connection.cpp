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

#include "client_connection.hpp"
#include "state_handshake.hpp"
#include "state_master.hpp"

namespace clt
{

Connection::Connection (boost::asio::io_service &io_service, const std::string &host, const std::string &port, Server *srv)
    : IConnection(io_service,host,port)
{
    m_State.reset(new StateMaster(srv,this));

    m_state_table.insert(std::make_pair(STATE_MASTER,m_State));

    m_State.reset(new StateHandshake("ManagerServer","ShardServer",srv,this));
}

Connection::~Connection ()
{
}

}
