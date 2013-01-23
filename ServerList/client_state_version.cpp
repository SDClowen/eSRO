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

#include "client_state_version.hpp"
#include "srvlistclient.h"

/// SRNL INCLUDES
#include "packet/client/packet_login.hpp"

StateVersion::StateVersion (const std::string &clientID, SrvListClient *conn)
    : clt::ConnectionState(conn), m_count(0), m_clientID(clientID)
{
    m_opcode_table[0x600D] = boost::bind(&StateVersion::OnVersionCheck,this,_1);
}

StateVersion::~StateVersion ()
{
}

void StateVersion::Close ()
{
}

int StateVersion::OnVersionCheck (IPacket &packet)
{
    enum
    {
        PKT_VERSION_A,
        PKT_VERSION_B,
        PKT_VERSION_C,
        PKT_VERSION_D,
        PKT_VERSION_E,
        PKT_VERSION_F
    };

    switch(m_count)
    {
    case PKT_VERSION_A:
        OnVersionCheckA(packet);
        break;
    case PKT_VERSION_B:
        OnVersionCheckB(packet);
        break;
    case PKT_VERSION_C:
        OnVersionCheckC(packet);
        break;
    case PKT_VERSION_D:
        OnVersionCheckD(packet);
        break;
    case PKT_VERSION_E:
        OnVersionCheckE(packet);
        break;
    case PKT_VERSION_F:
        OnVersionCheckF(packet);
        break;
    default:
        break;
    }

    ++m_count;

    return MSG_SUCCESS;
}

int StateVersion::OnVersionCheckA (IPacket &packet)
{
    uint16_t flag = packet.Read<uint16_t>();
    uint8_t type = packet.Read<uint8_t>();

    return MSG_SUCCESS;
}

int StateVersion::OnVersionCheckB (IPacket &packet)
{
    uint16_t flag = packet.Read<uint16_t>();
    uint8_t type = packet.Read<uint8_t>();

    return MSG_SUCCESS;
}

int StateVersion::OnVersionCheckC (IPacket &packet)
{
    uint16_t flag = packet.Read<uint16_t>();
    uint8_t type = packet.Read<uint8_t>();

    return MSG_SUCCESS;
}

int StateVersion::OnVersionCheckD (IPacket &packet)
{
    uint16_t flag = packet.Read<uint16_t>();
    uint8_t type = packet.Read<uint8_t>();

    return MSG_SUCCESS;
}

int StateVersion::OnVersionCheckE (IPacket &packet)
{
    uint16_t flag = packet.Read<uint16_t>();
    uint8_t type = packet.Read<uint8_t>();

    if (flag == 0x0200 && type == 0x02)
    {
        std::string IP = packet.Read(); ///DOWNLOAD SERVER IP

        uint16_t port = packet.Read<uint16_t>(); ///DOWNLOAD SERVER PORT

        uint32_t count = packet.Read<uint32_t>();

        for (size_t i = 0; i < count; ++i)
        {
            uint8_t begin = packet.Read<uint8_t>();

            uint32_t ID = packet.Read<uint32_t>();

            std::string name = packet.Read();

            std::string path = packet.Read();

            uint32_t resv = packet.Read<uint32_t>();

            uint8_t end = packet.Read<uint8_t>();
        }
    }

    return MSG_SUCCESS;
}

int StateVersion::OnVersionCheckF (IPacket &packet)
{
    uint16_t flag = packet.Read<uint16_t>();
    uint8_t type = packet.Read<uint8_t>();

    OPacket pkt;
    clt_pkt::WriteRequestShardList(&pkt);

    m_connection->Send(&pkt,true);

    m_connection->SetState(STATE_LOGIN);

    return MSG_SUCCESS;
}
