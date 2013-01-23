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

#ifndef __SLES_GAME_SERVER_HANDLE_PLAYER_PACKET_HPP__
#define __SLES_GAME_SERVER_HANDLE_PLAYER_PACKET_HPP__

#include <vector>
#include <boost/shared_ptr.hpp>

class OPacket;
class Player;

namespace srv_pkt
{

enum LOBBY_ACTION
{
    LOBBY_ACTION_CREATE = 1,
    LOBBY_ACTION_LIST,
    LOBBY_ACTION_ERASE,
    LOBBY_ACTION_VALIDATE_NAME,
    LOBBY_ACTION_RESTORE
};

enum LOBBY_ERROR
{
    LOBBY_ERROR_MAX_CHARACTERS = 0x05,
    LOBBY_ERROR_NAME_EXIST = 0x10
};

void Lobby (const boost::shared_ptr<OPacket> &pkt, const LOBBY_ACTION action);

void Lobby (const boost::shared_ptr<OPacket> &pkt, const std::vector<boost::shared_ptr<Player> > &player_list);

void Lobby (const boost::shared_ptr<OPacket> &pkt, const LOBBY_ACTION action, const LOBBY_ERROR error);

void RequestIngame (const boost::shared_ptr<OPacket> &pkt);

}

#endif //__SLES_GAME_SERVER_HANDLE_PLAYER_PACKET_HPP__
