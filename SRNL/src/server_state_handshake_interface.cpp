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

#include "server_state_handshake_interface.hpp"
#include "server_connection_interface.hpp"
#include "security.hpp"
#include "opcodes_global_client.hpp"
#include "packet_authentication.hpp"

IStateHandshake::IStateHandshake (srv::IConnection *conn)
	:   ConnectionState(conn),
        m_authenticated(false)
{
	m_opcode_table[CLIENT_HANDSHAKE] = boost::bind(&IStateHandshake::OnHandshake,this,_1);
	m_opcode_table[CLIENT_HANDSHAKE_VALIDATED] = boost::bind(&IStateHandshake::OnHandshakeValidated,this,_1);
	m_opcode_table[CLIENT_WHOIS] = boost::bind(&IStateHandshake::OnWhoIs,this,_1);
}

IStateHandshake::~IStateHandshake ()
{
}

void IStateHandshake::SetBaseKey (const uint32_t key[2])
{
    m_base_key[0] = key[0];
    m_base_key[1] = key[1];
}

void IStateHandshake::SetSharedData (uint32_t a, uint32_t P, uint32_t G, uint32_t A)
{
    m_a = a;
    m_p = P;
    m_g = G;
    m_A = A;
}

int IStateHandshake::OnHandshake (IPacket &packet)
{
    if (!m_connection->IsEncryptionActive())
        return MSG_ERROR_ORDER;

    uint32_t B = packet.Read<uint32_t>();

    uint32_t client_signature[2] = {0};

    for (int i = 0; i < 2; ++i)
        client_signature[i] = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    uint32_t S = 0;
    uint8_t keybyte = 0;
    uint32_t priv_key[2] = { 0 };

    S = ModExp(m_p,m_a,B);

    keybyte = LOBYTE(LOWORD(S)) & 0x03;

    priv_key[0] = ModExp(m_p,m_a,m_g);
    priv_key[1] = B;

    Func_X_2((uint8_t*)priv_key,S,keybyte);

    m_connection->InitializeKey((uint8_t*)priv_key,8);

    uint32_t srv_signature[2] = { 0 };
    srv_signature[0] = B;
    srv_signature[1] = m_A;

    keybyte = LOBYTE(LOWORD(B)) & 0x07;
    Func_X_2((uint8_t*)srv_signature, S, keybyte);

    m_connection->Encode((uint8_t*)srv_signature,(uint8_t*)srv_signature,8);

    if (srv_signature[0] != client_signature[0] || srv_signature[1] != client_signature[1])
    {
#if defined DEBUG
        syslog(LOG_DEBUG,"srv::IStateHandshake::OnHandshake - Invalid Client Signature.");
        syslog(LOG_DEBUG,"Server Key = %i %i, Client Key = %i %i",srv_signature[0],srv_signature[1],
            client_signature[0],client_signature[1]);
#endif
        return MSG_ERROR_ARG;
    }

    srv_signature[0] = m_A;
    srv_signature[1] = B;

    keybyte = LOBYTE(LOWORD(m_A)) & 0x07;
    Func_X_2((LPBYTE)srv_signature,S,keybyte);

    m_connection->Encode((uint8_t*)srv_signature,(uint8_t*)srv_signature,8);

    boost::shared_ptr<OPacket> p(new OPacket);
    srv_pkt::WriteHandshake(p,srv_signature);

    m_connection->Send(p);

    Func_X_2((uint8_t*)m_base_key,S,0x3);

    m_connection->InitializeKey((uint8_t*)m_base_key,8);

    return MSG_SUCCESS;
}

int IStateHandshake::OnHandshakeValidated (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    if (m_authenticated)
        return MSG_ERROR_ORDER;

    m_authenticated = true;

    return MSG_SUCCESS;
}

int IStateHandshake::OnWhoIs (IPacket &packet)
{
    if (!m_authenticated)
        return MSG_ERROR_ORDER;

    std::string ID = packet.Read();

    uint8_t reserved = packet.Read<uint8_t>();

    if (reserved)
        return MSG_ERROR_ARG;

    if(!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<OPacket> p(new OPacket);
    srv_pkt::WriteWhoIs(p,m_connection->SrvIDS());

    m_connection->Send(p);

    if(!OnClientID(ID))
        return MSG_ERROR_ARG;

    return MSG_SUCCESS;
}

void IStateHandshake::Close ()
{
}
