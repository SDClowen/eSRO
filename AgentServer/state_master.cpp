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

#include "state_master.hpp"
#include "server.hpp"
#include "client_connection.hpp"

/// SRNL INCLUDES
#include <opcodes_manager_gateway_client.hpp>

namespace clt
{

StateMaster::StateMaster (Server *srv, Connection *conn)
    : clt::ConnectionState(conn), m_server(srv)
{
    using boost::bind;

    m_opcode_table[GM_SR_CONFIG] = bind(&StateMaster::OnConfig,this,_1);
    m_opcode_table[GM_SR_LOGIN_QUEUE] = bind(&StateMaster::OnQueue,this,_1);
}

StateMaster::~StateMaster ()
{
}

void StateMaster::Close ()
{
}

int StateMaster::OnConfig (IPacket &packet)
{
    uint16_t ID = packet.Read<uint16_t>();

    uint8_t locale = packet.Read<uint8_t>();

    uint8_t allowed_race = packet.Read<uint8_t>();

    std::string ch_filter = packet.Read();

    size_t size = packet.Read<size_t>();

    std::string tmp;
    std::vector<std::string> strfilter_list;
    for (size_t i=0; i < size; ++i)
    {
        tmp = packet.Read();
        strfilter_list.push_back(tmp);
    }

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    m_server->SetConfig(ID,locale,allowed_race,ch_filter,strfilter_list);

    m_server->Execute(false);

    return MSG_SUCCESS;
}

int StateMaster::OnQueue (IPacket &packet)
{
    LoginRequest request;

    request.ID = packet.Read<uint32_t>();

    request.usr = packet.Read();

    request.pw = packet.Read();

    request.IP = packet.Read();

    request.AccountID = packet.Read<uint32_t>();

    request.access_lv  = packet.Read<uint8_t>();

    request.sec_lv = packet.Read<uint8_t>();

    request.silk = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    m_server->QueueRequest(request);

    return MSG_SUCCESS;
}

}

