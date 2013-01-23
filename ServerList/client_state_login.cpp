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

#include "client_state_login.hpp"
#include "srvlistclient.h"

StateLogin::StateLogin (SrvListClient *conn)
    : clt::ConnectionState(conn)
{
    m_opcode_table[0xA101] = boost::bind(&StateLogin::OnServerList,this,_1);
}

StateLogin::~StateLogin ()
{
}

void StateLogin::Close()
{
}

int StateLogin::OnServerList(IPacket &packet)
{
    uint8_t ret = packet.Read<uint8_t>();

    if (ret == ANSWER_ACCEPT)
    {
        uint8_t locale = packet.Read<uint8_t>();

        std::string ID = packet.Read();

        uint8_t begin = packet.Read<uint8_t>();

        if (begin)
            return MSG_ERROR_ARG_VALUE;

        int size = packet.size() - 5 - ID.length();

        ///READ THE SERVER LIST INFORMATION
        Shard shard;
        std::vector<Shard> shard_list;

        while(size > 1)
        {
            shard.is_new = packet.Read<uint8_t>();
            shard.id = packet.Read<uint16_t>();
            shard.name = packet.Read();
            shard.usr_cnt = packet.Read<uint16_t>();
            shard.usr_max = packet.Read<uint16_t>();
            shard.state = packet.Read<uint8_t>();

            size = size - 10 - shard.name.length();

            if (size < 1)
                return MSG_ERROR_SIZE_MISMATCH;

            shard_list.push_back(shard);
        }

        uint8_t end = packet.Read<uint8_t>();

        if (end)
            return MSG_ERROR_ARG_VALUE;

        if (!packet.EndOfStream())
            return MSG_ERROR_SIZE_MISMATCH;

        static_cast<SrvListClient*>(m_connection)->SetShardList(shard_list);
    }
    else
    {
    }

    return MSG_SUCCESS;
}
