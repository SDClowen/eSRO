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
#include "connection.hpp"

StateHandshake::StateHandshake (Connection *conn)
    : IStateHandshake(conn)
{
	m_state_table.insert(std::make_pair("AgentServer",STATE_AGENT_GM));
	m_state_table.insert(std::make_pair("EmblemServer",STATE_EMBLEM));
	m_state_table.insert(std::make_pair("GatewayServer",STATE_GATEWAY));
	m_state_table.insert(std::make_pair("ShardServer",STATE_SHARD));
	m_state_table.insert(std::make_pair("DownloadServer",STATE_DOWNLOAD));
}

StateHandshake::~StateHandshake ()
{
}

bool StateHandshake::OnClientID (const std::string &ID)
{
	std::map<std::string,uint8_t>::const_iterator i = m_state_table.find(ID);

	if (i == m_state_table.end())
        return false;

    m_connection->SetState(i->second);

    return true;
}

