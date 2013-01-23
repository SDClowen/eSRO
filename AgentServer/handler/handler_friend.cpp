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

#include "world.hpp"
#include "manager/friend_manager.hpp"

/// SOL INCLUDES
#include <player.hpp>

/// EPL INCLUDES
#include <packet_friend.hpp>

int StateGame::OnFriendRequest (IPacket &packet)
{
    std::string name = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    World *world = m_server->GetWorld();
    boost::shared_ptr<FriendManager> mgr = world->GetFriendManager();

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Player> friend_player = world->FindPlayer_ByName(name);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (!pkt)
        return MSG_ERROR_MEMORY;

    if (!friend_player)
    {
        srv_pkt::FriendRequest(pkt,srv_pkt::FRIEND_ERROR_INVALID);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    if (mgr->isFriendMax(player->ID()) || mgr->isFriendMax(friend_player->ID()))
    {
        srv_pkt::FriendRequest(pkt,srv_pkt::FRIEND_ERROR_MAX);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    if (mgr->isFriendAdded(player->ID(),name))
    {
        srv_pkt::FriendRequest(pkt,srv_pkt::FRIEND_ERROR_ALREDY_ADDED);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    srv_pkt::FriendRequest(pkt,2,player->GetUniqueID(),player->get_name());
    m_server->Send(pkt,friend_player->ConnectionID());

    return MSG_SUCCESS;
}

int StateGame::OnFriendAnswer (IPacket &packet)
{
    enum
    {
        ANSWER_DENY,
        ANSWER_ACCEPT
    };

    uint32_t sessionID = packet.Read<uint32_t>();

    uint32_t playerID = packet.Read<uint32_t>();

    uint8_t answer = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    if (answer > 1)
        return MSG_ERROR_ARG;

    World *world = m_server->GetWorld();

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<Player> friend_player = world->FindPlayer_ByID(playerID);

    if (!friend_player)
        return MSG_SUCCESS;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if ( answer == ANSWER_DENY)
    {
        srv_pkt::FriendRequest(pkt,srv_pkt::ANSWER_DENY,friend_player->get_name());
        m_server->Send(pkt,friend_player->ConnectionID());
    }
    else
    {

        Friend friendA;
        friendA.ID = player->ID();
        friendA.Name = friend_player->get_name();
        friendA.Model = friend_player->get_model();
        friendA.status = true;

        Friend friendB;
        friendB.ID = friend_player->ID();
        friendB.Name = player->get_name();
        friendB.Model = player->get_model();
        friendB.status = true;

        if (!world->GetFriendManager()->addFriend(friendA,friendB))
            return MSG_ERROR_ARG;

        srv_pkt::FriendNotify(pkt,friendA);
        m_connection->Send(pkt);

        pkt.reset(new OPacket);

        srv_pkt::FriendNotify(pkt,friendA.ID,srv_pkt::FRIEND_ONLINE);
        m_connection->Send(pkt);

        pkt.reset(new OPacket);

        srv_pkt::FriendNotify(pkt,friendB);
        m_server->Send(pkt,friend_player->ConnectionID());

        pkt.reset(new OPacket);

        srv_pkt::FriendNotify(pkt,friendB.ID,srv_pkt::FRIEND_ONLINE);
        m_server->Send(pkt,friend_player->ConnectionID());
    }

    return MSG_SUCCESS;
}

int StateGame::OnFriendDelete (IPacket &packet)
{
    uint32_t friendID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    std::pair<bool,std::string> ret;
    ret = m_server->GetWorld()->GetFriendManager()->deleteFriend(player->ID(),friendID);

    if (!ret.first)
        return MSG_ERROR_ARG;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (!pkt)
        return MSG_ERROR_MEMORY;

    srv_pkt::FriendNotify(pkt,friendID);
    m_connection->Send(pkt);

    pkt.reset(new OPacket);

    if (!pkt)
        return MSG_ERROR_MEMORY;

    srv_pkt::FriendDelete(pkt,friendID);
    m_connection->Send(pkt);

    boost::shared_ptr<Player> fplayer = m_server->GetWorld()->FindPlayer_ByName(ret.second);

    if (fplayer)
    {
        pkt.reset(new OPacket);

        if (!pkt)
            return MSG_ERROR_MEMORY;

        srv_pkt::FriendNotify(pkt,player->ID());
        m_server->Send(pkt,fplayer->ConnectionID());
    }

    return MSG_SUCCESS;
}

int StateGame::OnFriendGroupCreate (IPacket &packet)
{
    std::string name = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    if (name.empty())
        return MSG_ERROR_ARG;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    uint16_t groupID = m_server->GetWorld()->GetFriendManager()->createGroup(player->ID(),name);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (!pkt)
        return MSG_ERROR_MEMORY;

    if (groupID)
    {
        FriendGroup group;
        group.refID = 1;
        group.Name = name;
        group.friendCount = 0;

        srv_pkt::FriendGroupCreate(pkt,name,groupID);
        m_connection->Send(pkt);
    }
    else
    {
        srv_pkt::FriendGroupCreate(pkt,srv_pkt::FRIEND_ERROR_GROUP_NAME_IN_USE);
        m_connection->Send(pkt);
    }

    return MSG_SUCCESS;
}

int StateGame::OnFriendGroupDelete (IPacket &packet)
{
    uint16_t groupID = packet.Read<uint16_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (!m_server->GetWorld()->GetFriendManager()->deleteGroup(player->ID(),groupID))
        return MSG_ERROR_ARG;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (!pkt)
        return MSG_ERROR_MEMORY;

    srv_pkt::FriendGroupDelete(pkt,groupID);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnFriendGroupEntryMove (IPacket &packet)
{
    uint32_t friendID = packet.Read<uint32_t>();

    uint16_t groupID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (!m_server->GetWorld()->GetFriendManager()->moveFriendIntoGroup(player->ID(),friendID,groupID))
        return MSG_ERROR_ARG;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (!pkt)
        return MSG_ERROR_MEMORY;

    srv_pkt::FriendGroupEntryMove(pkt,friendID,groupID);
    m_connection->Send(pkt);

    return MSG_ERROR_ARG;
}
