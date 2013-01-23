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

#include "state_gateway.hpp"
#include "connection.hpp"
#include "server.hpp"
#include "query/account_query.hpp"
#include "packet/packet_manager_gateway.hpp"
#include "packet/packet_manager_shard.hpp"

/// SRNL INCLUDES
#include <opcodes_manager_gateway_client.hpp>

/// GLOBAL INCLUDES
#include <boost/tuple/tuple.hpp>

StateGateway::StateGateway (Connection *conn, Server *srv)
    : srv::ConnectionState(conn), m_server(srv)
{
    using boost::bind;

    m_opcode_table[GW_CL_CONFIG] = bind(&StateGateway::OnConfig,this,_1);
    m_opcode_table[GW_CL_LOGIN_QUEUE] = bind(&StateGateway::OnQueue,this,_1);
}

StateGateway::~StateGateway ()
{
}

void StateGateway::Close ()
{
}

int StateGateway::OnConfig (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    uint8_t locale = m_server->Locale();

    uint32_t client_version = m_server->ClientVersion();

    std::map<uint32_t,News> news_list = m_server->NewsList();

    std::map<uint16_t,Shard> serv_list = m_server->ShardList();

    boost::shared_ptr<OPacket> p(new OPacket);
    srv_pkt::WriteConfig(p,locale,client_version,serv_list,news_list);

    m_connection->Send(p);

    return MSG_SUCCESS;
}

int StateGateway::OnQueue (IPacket &packet)
{
    uint32_t clientID = packet.Read<uint32_t>();

    uint32_t shardID = packet.Read<uint32_t>();

    std::string usr = packet.Read();

    std::string pw = packet.Read();

    std::string IP = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    std::map<uint16_t,Shard> srv_list = m_server->ShardList();

    std::map<uint16_t,Shard>::iterator i = srv_list.find(shardID);

    DB::Account::Login login_query;
    DB::Account::LOGIN_RET ret = login_query(m_server->DBConnection(),usr,pw,IP);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (i == srv_list.end())
    {
        srv_pkt::WriteLogin(pkt,clientID,srv_pkt::LOGIN_ERROR_C5);
    }
    else
    {
        switch(ret)
        {
        case DB::Account::LOGIN_ACCEPTED:
            {
                uint32_t queueID = (rand() >> 16) + (rand() >> 16);

                ///Obtain account access level, security level and current silk.

                DB::Account::Ext ext_query;
                boost::tuple<uint32_t,uint8_t,uint8_t,uint32_t> ext = ext_query(m_server->DBConnection(),usr);

                srv_pkt::WriteLoginQueue(pkt,queueID,usr,pw,IP,ext.get<0>(),ext.get<1>(),ext.get<2>(),ext.get<3>());
                m_server->Send(pkt,shardID);

                pkt.reset(new OPacket);

                srv_pkt::WriteLogin(pkt,clientID,queueID,i->second);
            }
            break;
        case DB::Account::LOGIN_REJECTED:
            {
                DB::Account::Attempt query;

                int attempts = query(m_server->DBConnection(),usr);

                if (attempts != -1)
                    srv_pkt::WriteLogin(pkt,clientID,srv_pkt::LOGIN_ERROR_WRONG_PW,attempts,3);
                else
                    srv_pkt::WriteLogin(pkt,clientID,srv_pkt::LOGIN_ERROR_C8);
            }
            break;
        case DB::Account::LOGIN_MAX_ATTEMPTS:
            srv_pkt::WriteLogin(pkt,clientID,srv_pkt::LOGIN_ERROR_WRONG_PW,3,3);
            break;
        case DB::Account::LOGIN_BANNED:
            {
                DB::Account::BanExpirationDate query;
                mysqlpp::DateTime dt = query(m_server->DBConnection(),usr);

                std::string msg = "Your account is suspended/blocked. Expiration date " + dt.str();

                srv_pkt::WriteLogin(pkt,clientID,srv_pkt::LOGIN_ERROR_BLOCKED_MSG,msg,dt.year(),
                    dt.month(),dt.day(),dt.hour(),dt.minute());
            }
            break;
        case DB::Account::LOGIN_ALREDY_LOGGED:
            srv_pkt::WriteLogin(pkt,clientID,srv_pkt::LOGIN_ERROR_USER_ALREDY_ON);
            break;
        default:
            srv_pkt::WriteLogin(pkt,clientID,srv_pkt::LOGIN_ERROR_C8);
            break;
        }
    }

    m_connection->Send(pkt);

    return MSG_SUCCESS;
}
