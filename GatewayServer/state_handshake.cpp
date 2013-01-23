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

#include "state_handshake.hpp"
#include "client_connection.hpp"
#include "packet/packet_gateway.hpp"

StateHandshake::StateHandshake (const std::string &srvID, const std::string &cltID, Client *conn)
    : clt::IStateHandshake(srvID,cltID,conn)
{
}

StateHandshake::~StateHandshake ()
{
}

void StateHandshake::OnConfig ()
{
    boost::shared_ptr<OPacket> p(new OPacket);
    clt_pkt::WriteConfig(p);

    m_connection->Send(p);

    m_connection->SetState(STATE_MASTER);
}
