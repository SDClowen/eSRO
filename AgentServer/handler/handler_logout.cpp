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

#include <player.hpp>

/// SRNL INCLUDES
#include <packet_misc.hpp>

#define LOGOUT_TIME 5

int StateGame::OnLogout (IPacket &packet)
{
    uint8_t action = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (player->get_attack_state() != ATTACK_ACTION_STOP)
    {
        srv_pkt::Logout(pkt,srv_pkt::LOGOUT_ERROR_ATTACK_STATE);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    if (player->GetCurrentTask() == PLAYER_TASK_TELEPORT)
    {
        srv_pkt::Logout(pkt,srv_pkt::LOGOUT_ERROR_TELEPORTING);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    srv_pkt::Logout(pkt,LOGOUT_TIME,action);
    m_connection->Send(pkt);

    m_logout_timer.expires_from_now(boost::posix_time::seconds(LOGOUT_TIME));
    m_logout_timer.async_wait(boost::bind(&StateGame::OnLogoutTimeout,this,boost::asio::placeholders::error));

    return MSG_SUCCESS;
}

int StateGame::OnLogoutCancel (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    m_logout_timer.cancel();

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::LogoutCancel(pkt);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

void StateGame::OnLogoutTimeout (const boost::system::error_code &error)
{
    if (!error)
    {
        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::LogoutNotify(pkt);
        m_connection->Send(pkt);

        m_connection->Stop();
    }
}
