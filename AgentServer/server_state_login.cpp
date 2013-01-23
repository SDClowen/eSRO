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

#include "server_state_login.hpp"
#include "server_connection.hpp"
#include "server.hpp"
#include "login_request.hpp"

/// EPL INCLUDES
#include <opcodes_shard_client.hpp>
#include <packet_login.hpp>
#include <packet_shard.hpp>

/// SRNL INCLUDES
#include <packet_authentication.hpp>

/// GLOBAL INCLUDES
#include <syslog.h>

StateLogin::StateLogin (Server *server, srv::Connection *conn)
    : srv::ConnectionState(conn), m_server(server)
{
    m_opcode_table[CLIENT_LOGIN] = boost::bind(&StateLogin::OnLogin,this,_1);
}

StateLogin::~StateLogin()
{
}

void StateLogin::Close()
{
}

int StateLogin::OnLogin(IPacket &packet)
{
    LoginRequest request;

    request.ID = packet.Read<uint32_t>();

    request.usr = packet.Read();

    request.pw = packet.Read();

    request.IP = m_connection->remote_endpoint();

    uint8_t global_version = packet.Read<uint8_t>();

    uint8_t mac[6];

    for (int i = 0; i < 6; ++i)
        mac[i] = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    if (global_version != m_server->Locale())
        return MSG_ERROR_ARG;

    LoginRequest ret = m_server->FindRequest(request.ID);

    if (ret == request)
    {
        ///CHECK IP NOT THE EXTERNAL IP!!
        if (ret.IP != request.IP)
        {
#if defined DEBUG
            syslog(LOG_INFO,"IP Mismatch, %s %s",ret.IP.c_str(),request.IP.c_str());
#endif
        }

        boost::shared_ptr<OPacket> pkt(new OPacket);

        if (!pkt)
            return MSG_ERROR_MEMORY;

        ///CHECK THE IP CONNECTION LIMIT.
        if (!m_server->IsIPSlotAvailable(ret.IP))
        {
            srv_pkt::WriteLoginShard(pkt,srv_pkt::LOGIN_ERROR_IP_LIMIT);
            m_connection->Send(pkt,true);
            return MSG_ERROR;
        }

        ///CHECK THE MAXIMUM AMOUNT OF PLAYERS, ACCESS LEVEL AND SECURITY LEVEL.
        if (!m_server->IsWorldSlotAvailable(ret.access_lv,ret.sec_lv))
        {
            srv_pkt::WriteLoginShard(pkt,srv_pkt::LOGIN_ERROR_USER_LIMIT);
            m_connection->Send(pkt,true);
            return MSG_ERROR;
        }

        m_server->UseIPSlot(ret.IP);
        m_server->UseWorldSlot();

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

        srv_pkt::WriteLoginShard(pkt);
        m_connection->Send(pkt,true);

        m_connection->SetState(srv::STATE_LOBBY);

        static_cast<srv::Connection*>(m_connection)->AccountID(ret.AccountID);
        static_cast<srv::Connection*>(m_connection)->AccessLevel(ret.access_lv);
        static_cast<srv::Connection*>(m_connection)->SecurityLevel(ret.sec_lv);
        static_cast<srv::Connection*>(m_connection)->Silk(ret.silk);

        pkt.reset(new OPacket);

        clt_pkt::WriteAccountNotify8(pkt,clt_pkt::ACCOUNT_NOTIFY_STATE,ret.AccountID,true);
        m_server->SendMaster(pkt);
    }
    else
    {
        return MSG_ERROR;
    }

    return MSG_SUCCESS;
}
