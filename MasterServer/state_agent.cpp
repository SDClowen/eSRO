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

#include "state_agent.hpp"
#include "connection.hpp"
#include "server.hpp"

StateAgent::StateAgent (Connection *conn, Server *srv)
    : srv::ConnectionState(conn), m_server(srv)
{
}

StateAgent::~StateAgent ()
{
}

void StateAgent::Close ()
{
}

int StateAgent::OnCommand (IPacket &packet)
{
    return MSG_SUCCESS;
}

int StateAgent::OnCommand_SrvList (IPacket &packet)
{
    return MSG_SUCCESS;
}

int StateAgent::OnCommand_SrvStats (IPacket &packet)
{
    return MSG_SUCCESS;
}

int StateAgent::OnCommand_SrvMsg (IPacket &packet)
{
    return MSG_SUCCESS;
}

int StateAgent::OnCommand_PlayerList (IPacket &packet)
{
    return MSG_SUCCESS;
}

int StateAgent::OnCommand_PlayerStats (IPacket &packet)
{
    return MSG_SUCCESS;
}

int StateAgent::OnCommand_PlayerMsg (IPacket &packet)
{
    return MSG_SUCCESS;
}

int StateAgent::OnCommand_AccountStats (IPacket &packet)
{
    return MSG_SUCCESS;
}

int StateAgent::OnCommand_AccountBlock (IPacket &packet)
{
    return MSG_SUCCESS;
}
