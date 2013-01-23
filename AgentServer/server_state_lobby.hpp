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

#ifndef SERVER_STATE_LOBBY_HPP
#define SERVER_STATE_LOBBY_HPP

#include <server_connection_state.hpp>

class Server;

namespace srv
{
    class Connection;
}

class Player;

class StateLobby : public srv::ConnectionState
{
public:

    typedef boost::shared_ptr<Player> player_ptr;
    typedef std::vector<player_ptr>::iterator player_iterator;
    typedef std::vector<player_ptr>::const_iterator player_const_iterator;

    StateLobby (Server *server, srv::Connection *conn);

    virtual ~StateLobby ();

    virtual void Close ();

private:

    int OnHandleCharacter (IPacket &packet);
    
    int OnCreateCharacter (IPacket &packet);

    int OnListCharacter (IPacket &packet);

    int OnEraseCharacter (IPacket &packet);

    int OnRestoreCharacter (IPacket &packet);

    int OnValidateCharacterName (IPacket &packet);

    int OnSelectCharacter (IPacket &packet);

    /**
     *
     *  HELPER FUNCTIONS
     *
     **/

    void LoadCharacters ();

    bool IsNameValid (const std::string &name);

    player_iterator FindCharacterIndex(const std::string& name);

private:

    bool m_loaded;
    std::string m_ch_filter;
    std::vector<std::string> m_substr_filter_list;
    std::vector<player_ptr> m_player_list;

    Server *m_server;
};

#endif // SERVER_STATE_LOBBY_HPP
