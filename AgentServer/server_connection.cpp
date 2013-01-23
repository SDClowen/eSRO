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

#include "server_connection.hpp"
#include "server.hpp"
#include "server_state_handshake.hpp"
#include "server_state_login.hpp"
#include "server_state_lobby.hpp"
#include "server_state_game.hpp"

#include "player.hpp"

namespace srv
{

Connection::Connection (const uint32_t ID, boost::asio::io_service &io_service, Server *srv)
    : srv::IConnection(ID,io_service,srv), m_account_id(0), m_access_lv(0), m_security_lv(0), m_silk(0)
{
    m_State.reset(new StateGame(srv,this));
    m_state_table.insert(std::make_pair(STATE_GAME,m_State));

    m_State.reset(new StateLobby(srv,this));
    m_state_table.insert(std::make_pair(STATE_LOBBY,m_State));

    m_State.reset(new StateLogin(srv,this));
    m_state_table.insert(std::make_pair(STATE_LOGIN,m_State));

    m_State.reset(new srv::StateHandshake("SR_Client",this));
}

Connection::~Connection ()
{
}

void Connection::AccountID(const uint32_t ID)
{
    m_account_id = ID;
}

uint32_t Connection::AccountID() const
{
    return m_account_id;
}

void Connection::AccessLevel (const uint8_t level)
{
    m_access_lv = level;
}

uint8_t Connection::AccessLevel () const
{
    return m_access_lv;
}

void Connection::SecurityLevel (const uint8_t level)
{
    m_security_lv = level;
}

uint8_t Connection::SecurityLevel () const
{
    return m_security_lv;
}

void Connection::Silk(const uint32_t amount)
{
    m_silk = amount;
}

uint32_t Connection::Silk () const
{
    return m_silk;
}

void Connection::SetPlayer(const boost::shared_ptr<Player> &player)
{
    boost::mutex::scoped_lock lock(m_player_mutex);

    m_player = player;
}

boost::shared_ptr<Player> Connection::GetPlayer() const
{
    boost::mutex::scoped_lock lock(m_player_mutex);

    return m_player;
}

int32_t Connection::AreaID() const
{
    return m_player->get_position().get_zone();
}

}
