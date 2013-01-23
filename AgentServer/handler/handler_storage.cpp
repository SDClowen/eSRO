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

/// SRNL INCLUDES
#include <packet_storage.hpp>

int StateGame::OnUseStorage (IPacket &packet)
{
    uint32_t npcID = packet.Read<uint32_t>();

    uint8_t reserv = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (player->get_selected_object() != npcID || reserv)
        return MSG_ERROR_ARG;

    boost::shared_ptr<Storage> storage = player->get_storage(STORAGE_MAIN);

    if (!storage)
        return MSG_ERROR;

    boost::shared_ptr<OPacket> p1(new OPacket),p2(new OPacket),p3(new OPacket);

    srv_pkt::StorageGold(p1,player->get_stored_gold());
    srv_pkt::StorageItems(p2,storage);
    srv_pkt::StorageEnter(p3);
    m_connection->Send(p1,p2,p3);

    return MSG_SUCCESS;
}
