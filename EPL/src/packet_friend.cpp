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

#include "packet_friend.hpp"

#include <packet.hpp>
#include <opcodes_shard_server.hpp>

namespace srv_pkt
{

void FriendList (const boost::shared_ptr<OPacket> &pkt, const FriendBook &book)
{
    pkt->WriteOpcode(SERVER_FRIEND_LIST);
    pkt->Write<uint8_t>(book.groups.size());

    for (std::vector<FriendGroup>::const_iterator i = book.groups.begin(); i != book.groups.end(); ++i)
    {
        pkt->Write<uint16_t>(i->groupID);
        pkt->Write(i->Name);
        pkt->Write<uint8_t>(i->friendCount);
    }

    pkt->Write<uint8_t>(book.friends.size());

    for (std::vector<Friend>::const_iterator i = book.friends.begin(); i != book.friends.end(); ++i)
    {
        pkt->Write<uint32_t>(i->ID);
        pkt->Write(i->Name);
        pkt->Write<uint32_t>(i->Model);
        pkt->Write<uint16_t>(i->groupID);
        pkt->Write<uint8_t>(i->status);
    }
}

void FriendNotify (const boost::shared_ptr<OPacket> &pkt, const uint32_t id, const FRIEND_STATE state)
{
    pkt->WriteOpcode(SERVER_FRIEND_NOTIFY);
    pkt->Write<uint8_t>(FRIEND_NOTIFY_STATE);
    pkt->Write<uint32_t>(id);
    pkt->Write<uint8_t>(state);
}

void FriendNotify (const boost::shared_ptr<OPacket> &pkt, const Friend &new_friend)
{
    pkt->WriteOpcode(SERVER_FRIEND_NOTIFY);
    pkt->Write<uint8_t>(FRIEND_NOTIFY_ACCEPT);
    pkt->Write<uint32_t>(new_friend.ID);
    pkt->Write(new_friend.Name);
    pkt->Write<uint32_t>(new_friend.Model);
}

void FriendNotify (const boost::shared_ptr<OPacket> &pkt, const uint32_t id)
{
    pkt->WriteOpcode(SERVER_FRIEND_NOTIFY);
    pkt->Write<uint8_t>(FRIEND_NOTIFY_DELETE);
    pkt->Write<uint32_t>(id);
}

void FriendRequest (const boost::shared_ptr<OPacket> &pkt, const FRIEND_ERROR error)
{
    pkt->WriteOpcode(SERVER_FRIEND_ANSWER,ANSWER_ERROR,error);
    pkt->Write<uint8_t>(0x64);
}

void FriendRequest (const boost::shared_ptr<OPacket> &pkt, const FRIEND_ANSWER answer, const std::string &name)
{
    pkt->WriteOpcode(SERVER_FRIEND_ANSWER);
    pkt->Write<uint8_t>(ANSWER_ERROR);
    pkt->Write<uint16_t>(answer);
    pkt->Write<uint8_t>(0x64);
    pkt->Write(name);
}

void FriendRequest (const boost::shared_ptr<OPacket> &pkt, const uint32_t sessionID, const uint32_t playerID,
    const std::string &name)
{
    pkt->WriteOpcode(SERVER_FRIEND_REQUEST);
    pkt->Write<uint32_t>(sessionID);
    pkt->Write<uint32_t>(playerID);
    pkt->Write(name);
}

void FriendDelete (const boost::shared_ptr<OPacket> &pkt, const uint32_t friendID)
{
    pkt->WriteOpcode(SERVER_FRIEND_DELETE,ANSWER_ACCEPT);
    pkt->Write<uint32_t>(friendID);
}

void FriendGroupCreate (const boost::shared_ptr<OPacket> &pkt, const std::string &name, const uint32_t groupID)
{
    pkt->WriteOpcode(SERVER_FRIEND_GROUP_CREATE,ANSWER_ACCEPT);
    pkt->Write(name);
    pkt->Write(groupID);
}

void FriendGroupCreate (const boost::shared_ptr<OPacket> &pkt, const FRIEND_ERROR error)
{
    pkt->WriteOpcode(SERVER_FRIEND_GROUP_CREATE,ANSWER_ERROR,error);
    pkt->Write<uint8_t>(0x64);
}

void FriendGroupDelete (const boost::shared_ptr<OPacket> &pkt, const uint16_t groupID)
{
    pkt->WriteOpcode(SERVER_FRIEND_GROUP_DELETE,ANSWER_ACCEPT);
    pkt->Write<uint16_t>(groupID);
}

void FriendGroupEntryMove (const boost::shared_ptr<OPacket> &pkt, const uint32_t friendID, const uint16_t groupID)
{
    pkt->WriteOpcode(SERVER_FRIEND_GROUP_ENTRY_ADD,ANSWER_ACCEPT);
    pkt->Write<uint32_t>(friendID);
    pkt->Write<uint16_t>(groupID);
}

}
