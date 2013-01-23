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

#include "server.hpp"
#include "server_connection.hpp"
#include "server_state_game.hpp"

/// SOL INCLUDES
#include <player.hpp>

/// EPL INCLUDES
#include <packet_block.hpp>

#include "query/block_query.hpp"

int StateGame::OnBlock (IPacket &packet)
{
    uint8_t action = packet.Read<uint8_t>();

    std::string name = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (name == player->get_name())
        return MSG_ERROR_ARG;

    boost::shared_ptr<OPacket> pkt(new OPacket);
    DB::BLOCK::Check check_query;

    switch(action)
    {
    case srv_pkt::BLOCK_INSERT:
        DB::BLOCK::Valid valid_query;
        if (!valid_query(m_server->DBConnection(),name))
        {
            srv_pkt::Block(pkt,srv_pkt::BLOCK_INSERT,srv_pkt::BLOCK_INVALID);
            m_connection->Send(pkt);
            break;
        }

        if (!check_query(m_server->DBConnection(),player->ID(),name))
        {
            srv_pkt::Block(pkt,srv_pkt::BLOCK_INSERT,srv_pkt::BLOCK_ADDED);
            m_connection->Send(pkt);
            break;
        }

        DB::BLOCK::Insert insert_query;
        insert_query(m_server->DBConnection(),player->ID(),name);

        srv_pkt::Block(pkt,srv_pkt::BLOCK_INSERT,srv_pkt::BLOCK_OK);
        m_connection->Send(pkt);

        break;
    case srv_pkt::BLOCK_REMOVE:
        if (check_query(m_server->DBConnection(),player->ID(),name))
        {
            DB::BLOCK::Remove remove_query;
            remove_query(m_server->DBConnection(),player->ID(),name);

            srv_pkt::Block(pkt,srv_pkt::BLOCK_REMOVE,srv_pkt::BLOCK_OK);
            m_connection->Send(pkt);
        }
        break;
    default:
        return MSG_ERROR_ARG;
    }

    return MSG_SUCCESS;
}
