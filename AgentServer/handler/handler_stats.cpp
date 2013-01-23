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
#include <party.hpp>

/// SRDL INCLUDES
#include <player_query.hpp>

/// SRNL INCLUDES
#include <packet_player.hpp>

int StateGame::OnStatsIntInc (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (player->get_stat())
    {
        player->inc_int();

        DB::PLAYER::Stats query;
        query(m_server->DBConnection(),player);

        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::IncInt(pkt);
        m_connection->Send(pkt);

        boost::shared_ptr<Party> party = player->get_party();

        if (party)
        {
            party->SendHPMPNotification(player->GetUniqueID(),player->hp(),player->max_hp(),player->mp(),
                player->max_mp());
        }
    }

    return MSG_SUCCESS;
}

int StateGame::OnStatsStrInc (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if  (player->get_stat())
    {
        player->inc_str();

        DB::PLAYER::Stats query;
        query(m_server->DBConnection(),player);

        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::IncStr(pkt);
        m_connection->Send(pkt);

        boost::shared_ptr<Party> party = player->get_party();

        if (party)
        {
            party->SendHPMPNotification(player->GetUniqueID(),player->hp(),player->max_hp(),player->mp(),
                player->max_mp());
        }
    }

    return MSG_SUCCESS;
}
