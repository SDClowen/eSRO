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

#include "server_state_version.hpp"
#include "server_connection.hpp"

/// SRNL INCLUDES
#include <packet_authentication.hpp>
#include <opcodes_global_client.hpp>

StateVersion::StateVersion (const std::string &clientID, const uint8_t global_version, srv::Connection *conn)
    : srv::ConnectionState(conn), m_client_id(clientID), m_global_version(global_version)
{
    m_opcode_table[CLIENT_VERSION_INFO] = boost::bind(&StateVersion::OnVersionInfo,this,_1);
}

StateVersion::~StateVersion ()
{
}

void StateVersion::Close ()
{
}

int StateVersion::OnVersionInfo (IPacket &packet)
{
    uint8_t global_version = packet.Read<uint8_t>();

	std::string ID = packet.Read();

    uint32_t client_version = packet.Read<uint32_t>();

	if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

	if (m_global_version != global_version || ID != m_client_id)
        return MSG_ERROR_ARG;

	if (client_version == m_connection->ClientVersion())
	{
	    m_connection->SetState(srv::STATE_LOGIN);

        boost::shared_ptr<OPacket> pkt(new OPacket);

        srv_pkt::WriteVersionA(pkt);
        m_connection->Send(pkt);

        pkt.reset(new OPacket);

        srv_pkt::WriteVersionB(pkt);
        m_connection->Send(pkt);

        pkt.reset(new OPacket);

        srv_pkt::WriteVersionC(pkt);
        m_connection->Send(pkt);

        pkt.reset(new OPacket);

        srv_pkt::WriteVersionD(pkt);
        m_connection->Send(pkt);

        pkt.reset(new OPacket);

        srv_pkt::WriteVersionE(pkt);
        m_connection->Send(pkt);

        pkt.reset(new OPacket);

        srv_pkt::WriteVersionF(pkt);   ///PERFORM UPDATE PACKET!!

        m_connection->Send(pkt);
	}
	else
	{
		///REDIRECT TO UPDATE SERVER!!!
		m_connection->Stop();
	}

    return MSG_SUCCESS;
}
