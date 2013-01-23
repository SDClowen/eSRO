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

#include "server_state_handshake.hpp"
#include "server_connection.hpp"

namespace srv
{

StateHandshake::StateHandshake (const std::string &clientID, srv::Connection *conn)
    : IStateHandshake(conn), m_client_id(clientID)
{
}

StateHandshake::~StateHandshake ()
{
}

bool StateHandshake::OnClientID (const std::string &ID)
{
    if (ID != m_client_id)
        return false;

    m_connection->SetState(srv::STATE_VERSION);

    return true;
}

}
