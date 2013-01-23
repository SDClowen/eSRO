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

#ifndef __SLES_GAME_SERVER_FRIEND_PACKETS_HPP__
#define __SLES_GAME_SERVER_FRIEND_PACKETS_HPP__

#include <friend.hpp>

#include <boost/shared_ptr.hpp>
#include <vector>

class OPacket;

namespace srv_pkt
{

enum FRIEND_STATE
{
	FRIEND_ONLINE,
	FRIEND_OFFLINE
};

enum FRIEND_NOTIFY
{
    FRIEND_NOTIFY_ACCEPT = 0x02,
    FRIEND_NOTIFY_DELETE,
    FRIEND_NOTIFY_STATE
};

enum FRIEND_ERROR
{
    FRIEND_ERROR_MAX = 0x08,
    FRIEND_ERROR_ALREDY_ADDED = 0x09,
    FRIEND_ERROR_INVALID = 0x0D,
    FRIEND_ERROR_GROUP_NAME_IN_USE = 0x19
};

enum FRIEND_ANSWER
{
    ANSWER_DENY = 0x020B,
    ANSWER_IGNORE = 0x000B
};

/**
 *
 *	@brief Friend list packet.
 *
 **/

void FriendList (const boost::shared_ptr<OPacket> &pkt, const FriendBook &book);

void FriendNotify (const boost::shared_ptr<OPacket> &pkt);

/**
 *
 *	@brief Friend state packet.
 *	@ID FriendID that changed its state.
 *
 **/

void FriendNotify (const boost::shared_ptr<OPacket> &pkt, const uint32_t ID, const FRIEND_STATE status);

void FriendNotify (const boost::shared_ptr<OPacket> &pkt, const Friend &new_friend);

/**
 *
 *	@brief Friend deleted notification packet.
 *	@ID FriendID to delete.
 *
 **/

void FriendNotify (const boost::shared_ptr<OPacket> &pkt, const uint32_t ID);

void FriendRequest (const boost::shared_ptr<OPacket> &pkt, const FRIEND_ERROR error);

void FriendRequest (const boost::shared_ptr<OPacket> &pkt, const FRIEND_ANSWER, const std::string &name);

void FriendRequest (const boost::shared_ptr<OPacket> &pkt, const uint32_t sessionID, const uint32_t playerID,
    const std::string &name);

void FriendDelete (const boost::shared_ptr<OPacket> &pkt, const uint32_t friendID);

void FriendGroupCreate (const boost::shared_ptr<OPacket> &pkt, const std::string &name, const uint32_t groupID);

void FriendGroupCreate (const boost::shared_ptr<OPacket> &pkt, const FRIEND_ERROR error);

void FriendGroupDelete (const boost::shared_ptr<OPacket> &pkt, const uint16_t groupID);

void FriendGroupEntryMove (const boost::shared_ptr<OPacket> &pkt, const uint32_t friendID, const uint16_t groupID);

}

#endif //__SLES_GAME_SERVER_FRIEND_PACKETS_HPP__
