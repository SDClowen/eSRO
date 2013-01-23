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

#include "client_state_handshake_interface.hpp"
#include "client_connection_interface.hpp"
#include "opcodes_global_client.hpp"

/// SRNL INCLUDES
#include <security.hpp>

namespace clt
{

IStateHandshake::IStateHandshake (const std::string &srvID, const std::string &cltID, IConnection *conn)
    : clt::ConnectionState(conn), m_SrvID(srvID), m_CltID(cltID)
{
    m_opcode_table[CLIENT_HANDSHAKE] = boost::bind(&IStateHandshake::OnHandshake,this,_1);
    m_opcode_table[CLIENT_WHOIS] = boost::bind(&IStateHandshake::OnWhoIs,this,_1);
}

IStateHandshake::~IStateHandshake ()
{
}

void IStateHandshake::Close ()
{
}

int IStateHandshake::OnHandshake (IPacket &packet)
{
    enum
    {
        HANDSHAKE_CLEAN = 0x01,
        HANDSHAKE_SERVER_KEY = 0x0E,
        HANDSHAKE_SERVER_SIGNATURE = 0x10
    };

    int ret = MSG_SUCCESS;
    uint8_t type = packet.Read<uint8_t>();

    switch(type)
    {
    case HANDSHAKE_CLEAN:
        ret = OnHandshake_Clean(packet);
        break;
    case HANDSHAKE_SERVER_KEY:
        ret = OnHandshake_Key(packet);
        break;
    case HANDSHAKE_SERVER_SIGNATURE:
        ret = OnHandshake_Signature(packet);
        break;
    default:
        ret = MSG_ERROR_ARG;
        break;
    }

    return ret;
}

int IStateHandshake::OnHandshake_Clean (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    SendID();

    return MSG_SUCCESS;
}

int IStateHandshake::OnHandshake_Key (IPacket &packet)
{
    uint32_t public_key[2];

    for (int i = 0; i < 2; ++i)
        public_key[i] = packet.Read<uint32_t>();

    uint32_t count = packet.Read<uint32_t>();

    uint32_t crc = packet.Read<uint32_t>();

    for (int i = 0; i < 2; ++i)
        m_base_key[i] = packet.Read<uint32_t>();

    uint32_t G = packet.Read<uint32_t>();

    uint32_t P = packet.Read<uint32_t>();

    uint32_t A = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    m_connection->SetEncryption(true);

    m_connection->SetSeed(count,crc);

    ///TODO: GENERATE A UNIQUIE IDENTIFIER NUMBER WITH THE LINUX FUNCTIONS!!
    uint32_t b = 0x370;

//    if(dwRand == 0)
//    {
//        CoCreateGuid(&guid);
//        dwRand = guid.Data1 & 0x7FFFFFFF;
//    }

    m_connection->InitializeKey((LPBYTE)public_key, 8);

    uint8_t keyByte = 0;
    uint32_t B = ModExp(P,b,G);
    m_S = ModExp(P,b,A);

    uint32_t priv_key[2] = {0};
    priv_key[0] = A;
    priv_key[1] = B;

    keyByte = LOBYTE(LOWORD(m_S)) & 0x03;
    Func_X_2((LPBYTE)priv_key, m_S, keyByte);

    m_connection->InitializeKey((LPBYTE)priv_key, 8);

    // Generate the private blowfish data
    uint32_t signature[2] = {0};
    signature[0] = B;
    signature[1] = A;

    keyByte = LOBYTE(LOWORD(B)) & 0x07;
    Func_X_2((LPBYTE)signature, m_S, keyByte);

    m_connection->Encode((LPBYTE)signature, (LPBYTE)signature, 8);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (!pkt)
        return MSG_ERROR_MEMORY;

    pkt->WriteOpcode(0x5000);
    pkt->Write<uint32_t>(B);
    pkt->Write<uint32_t>(signature[0]);
    pkt->Write<uint32_t>(signature[1]);

    m_connection->Send(pkt);

    m_srv_signature[0] = A;
    m_srv_signature[1] = B;

    keyByte = LOBYTE(LOWORD(A)) & 0x07;
    Func_X_2((LPBYTE)m_srv_signature,m_S, keyByte);

    m_connection->Encode((LPBYTE)m_srv_signature, (LPBYTE)m_srv_signature, 8);

    return MSG_SUCCESS;
}

int IStateHandshake::OnHandshake_Signature (IPacket &packet)
{
    uint32_t signature[2] = { 0 };

    for (int i=0; i<2; ++i)
        signature[i] = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    // Simple check to see if the handshake is good
    if(m_srv_signature[0] != signature[0] || m_srv_signature[1] != signature[1])
    {
#if defined DEBUG
        syslog(LOG_DEBUG,"clt::IStateHandshake::OnHandshake_Signature - Invalid Server Signature.");
        syslog(LOG_DEBUG,"Server Key = %i %i, Client Key = %i %i",m_srv_signature[0],m_srv_signature[1],
            signature[0],signature[1]);
#endif
        return MSG_ERROR_ARG;
    }

    Func_X_2((LPBYTE)m_base_key, m_S, 0x3);

    m_connection->InitializeKey((uint8_t*)m_base_key,8);

    SendID();

    return MSG_SUCCESS;
}

int IStateHandshake::OnWhoIs (IPacket &packet)
{
    std::string srvID = packet.Read();

    uint8_t resv = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    if (resv || srvID != m_SrvID)
        return MSG_ERROR_ARG;

    m_connection->KeepAlive();

    OnConfig();

    return MSG_SUCCESS;
}

void IStateHandshake::SendID ()
{
    boost::shared_ptr<OPacket> p(new OPacket);
    p->WriteOpcode(0x9000);

    m_connection->Send(p);

    p.reset(new OPacket);

    p->WriteOpcode(0x2001);
    p->Write(m_CltID);
    p->Write<uint8_t>(0);

    m_connection->Send(p,true);
}

}
