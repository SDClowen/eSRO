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

#include "state_shard.hpp"
#include "connection.hpp"
#include "server.hpp"
#include "query/account_query.hpp"
#include "query/manager_query.hpp"

///SRNL INCLUDES
#include <opcodes_manager_gateway_client.hpp>
#include <iostream>

StateShard::StateShard (Connection *conn, Server *srv)
    : srv::ConnectionState(conn), m_server(srv)
{
    using boost::bind;

	m_opcode_table[GM_CL_CONFIG] = bind(&StateShard::OnConfig,this,_1);
	m_opcode_table[GM_CL_ACCOUNT_NOTIFY] = bind(&StateShard::OnAccountNotify,this,_1);
	m_opcode_table[GM_CL_SHARD_NOTIFY] = bind(&StateShard::OnShardNotify,this,_1);
}

StateShard::~StateShard ()
{
}

void StateShard::Close ()
{
}

int StateShard::OnConfig (IPacket &packet)
{
    Shard serv;

    serv.is_new = packet.Read<uint8_t>();
    serv.name = packet.Read();
    serv.usr_cnt = packet.Read<uint16_t>();
    serv.usr_max = packet.Read<uint16_t>();
    serv.state = packet.Read<uint8_t>();
    serv.ip = packet.Read();
    serv.port = packet.Read<uint16_t>();

    if (!packet.EndOfStream())
        throw Exception::InvalidPacketSize();

    serv.id = m_connection->ID();

    m_server->InsertShard(serv);

    ///READ SERVER RESPECTIVE CONFIGURATION FILE

    DB::MANAGER::Config cfg_query;
    ServConfig cfg = cfg_query(m_server->DBConnection(),serv.name);

    DB::MANAGER::NameFilter query;
    boost::tuple<std::string,std::vector<std::string> > ret = query(m_server->DBConnection());

    uint8_t allowed_race = cfg.ch_char | (cfg.eu_char << 1);

    ///SEND THE SERVER CURRENT ID
    ///SEND ANY OTHER NECESARY CONFIGURATION INFORMATION NEEDED
    boost::shared_ptr<OPacket> p(new OPacket(0xB002));
    p->Write<uint16_t>(serv.id);
    p->Write<uint8_t>(m_server->Locale());
    p->Write<uint8_t>(allowed_race);
    p->Write(ret.get<0>());
    p->Write(ret.get<1>());

    m_connection->Send(p);

    return MSG_SUCCESS;
}

int StateShard::OnAccountNotify (IPacket &packet)
{
    enum ACCOUNT_NOTIFY
    {
        ACCOUNT_NOTIFY_STATE,
        ACCOUNT_NOTIFY_SILK,
        ACCOUNT_NOTIFY_BAN,
    };

    uint8_t type = packet.Read<uint8_t>();

    uint32_t accountID = packet.Read<uint32_t>();

    switch(type)
    {
    case ACCOUNT_NOTIFY_STATE:
        {
            uint8_t arg = packet.Read<uint8_t>();

            DB::Account::SetLogged query;
            query(m_server->DBConnection(),accountID,(bool)arg);

            if (!packet.EndOfStream())
                throw Exception::InvalidPacketSize();
        }
        break;
    case ACCOUNT_NOTIFY_BAN:
        {
            uint8_t arg = packet.Read<uint8_t>();

            ///STILL NEED TO DEFINE BAN POLICY!!

            if (!packet.EndOfStream())
                throw Exception::InvalidPacketSize();
        }
        break;
    case ACCOUNT_NOTIFY_SILK:
        {
            uint32_t arg = packet.Read<uint32_t>();

            DB::Account::Silk query;
            query(m_server->DBConnection(),accountID,arg);

            if (!packet.EndOfStream())
                throw Exception::InvalidPacketSize();
        }
        break;
    default:
        break;
    }

    return MSG_SUCCESS;
}

int StateShard::OnShardNotify (IPacket &packet)
{
    enum SHARD_NOTIFY
    {
        SHARD_NOTIFY_USR_CNT,
        SHARD_NOTIFY_USR_MAX,
        SHARD_NOTIFY_STATE
    };

    uint8_t type = packet.Read<uint8_t>();

    uint16_t ID = packet.Read<uint16_t>();

    uint16_t arg = 0;

    switch(type)
    {
    case SHARD_NOTIFY_USR_CNT:
    case SHARD_NOTIFY_USR_MAX:
        arg = packet.Read<uint16_t>();
        break;
    case SHARD_NOTIFY_STATE:
        arg = packet.Read<uint8_t>();
        break;
    default:
        break;
    }

    if (!packet.EndOfStream())
        throw Exception::InvalidPacketSize();

    m_server->UpdateShard(type,ID,arg);

    return MSG_SUCCESS;
}
