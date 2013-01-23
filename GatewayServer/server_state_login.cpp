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
#include "server.hpp"
#include "server_connection.hpp"
#include "packet/packet_gateway.hpp"
#include "packet/packet_information.hpp"

/// SRNL INCLUDES
#include <packet_authentication.hpp>
#include <opcodes_gateway_client.hpp>

StateLogin::StateLogin (Server *srv, srv::Connection *conn)
    : srv::ConnectionState(conn), m_server(srv)
{
    m_opcode_table[CLIENT_NEWS] = boost::bind(&StateLogin::OnNews,this,_1);
    m_opcode_table[CLIENT_SERVERS_LIST] = boost::bind(&StateLogin::OnServers,this,_1);
    m_opcode_table[CLIENT_LOGIN] = boost::bind(&StateLogin::OnLogin,this,_1);
}

StateLogin::~StateLogin ()
{
}

void StateLogin::Close ()
{
}

int StateLogin::OnNews (IPacket &packet)
{
    uint8_t global_version = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    if (global_version != m_server->Locale())
         return MSG_ERROR_ARG;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    srv_pkt::WriteVersionG(pkt);
    m_connection->Send(pkt);

    pkt.reset(new OPacket);

    srv_pkt::WriteNews(pkt,m_server->NewsList());
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateLogin::OnServers (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    std::map<uint16_t,Shard> list = m_server->ShardList();

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::WriteShards(pkt,m_server->ServerListID(),m_server->Locale(),list);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateLogin::OnLogin (IPacket &packet)
{
    uint8_t global_version = packet.Read<uint8_t>();

    std::string user = packet.Read();

    std::string pass = packet.Read();

    std::string mobile = packet.Read();

    uint16_t servID = packet.Read<uint16_t>();

    uint8_t channel = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    if (global_version != m_server->Locale())
        return MSG_ERROR_ARG;

    std::map<uint16_t,Shard> srv_list = m_server->ShardList();

    std::map<uint16_t,Shard>::const_iterator i = srv_list.find(servID);

    if (i == srv_list.end())
        return MSG_ERROR_ARG;

    std::string IP = m_connection->remote_endpoint();

    boost::shared_ptr<OPacket> pkt(new OPacket);

    clt_pkt::WriteQueue(pkt,m_connection->ID(),user,pass,servID,IP);
    m_server->SendMaster(pkt);

    return MSG_SUCCESS;
}
