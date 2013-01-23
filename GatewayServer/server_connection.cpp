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

#include "server_connection.hpp"
#include "server.hpp"
#include "server_state_handshake.hpp"
#include "server_state_version.hpp"
#include "server_state_login.hpp"

namespace srv
{

Connection::Connection (const uint32_t ID, boost::asio::io_service &io_service, Server *srv)
    : srv::IConnection(ID,io_service,srv)
{
    m_State.reset(new srv::StateHandshake(srv->ClientID(),this));

    boost::shared_ptr<srv::ConnectionState> st;

    st.reset(new StateVersion(srv->ClientID(),srv->Locale(),this));

    m_state_table.insert(std::make_pair(srv::STATE_VERSION,st));

    st.reset(new StateLogin(srv,this));

    m_state_table.insert(std::make_pair(srv::STATE_LOGIN,st));
}

Connection::~Connection ()
{
}

}
