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

#include "client_state_handshake.hpp"
#include "srvlistclient.h"

StateHandshake::StateHandshake (const std::string &srvID, const std::string &cltID, SrvListClient *conn)
    : clt::IStateHandshake(srvID,cltID,conn)
{
}

StateHandshake::~StateHandshake ()
{
}

void StateHandshake::OnConfig ()
{
    OPacket pkt(0x6100);
    pkt.Write<uint8_t>(m_connection->Locale());
    pkt.Write(m_CltID);
    pkt.Write<uint32_t>(m_connection->Version());
    pkt.Write<uint32_t>(0);

    m_connection->Send(&pkt,true);

    m_connection->SetState(STATE_VERSION);
}
