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

#ifndef __AGENT_SERVER_CONNECTION_HPP__
#define __AGENT_SERVER_CONNECTION_HPP__

#include <server_connection_interface.hpp>

class Server;
class Player;

namespace srv
{

enum CONNECTION_STATES
{
    STATE_HANDSHAKE,
    STATE_LOGIN,
    STATE_LOBBY,
    STATE_GAME
};

class Connection : public srv::IConnection
{
public:

    Connection (const uint32_t ID, boost::asio::io_service &io_service, Server *srv);

    virtual ~Connection ();

    void AccountID (const uint32_t ID);

    uint32_t AccountID () const;

    void AccessLevel (const uint8_t level);

    uint8_t AccessLevel () const;

    void SecurityLevel (const uint8_t level);

    uint8_t SecurityLevel () const;

    void Silk (const uint32_t amount);

    uint32_t Silk () const;

    void SetPlayer (const boost::shared_ptr<Player> &player);

    boost::shared_ptr<Player> GetPlayer () const;

    int32_t AreaID () const;

private:

    uint32_t m_account_id;
    uint8_t m_access_lv;
    uint8_t m_security_lv;
    uint32_t m_silk;

    mutable boost::mutex m_player_mutex;
    boost::shared_ptr<Player> m_player;
};

}

#endif //__AGENT_SERVER_CONNECTION_HPP__
