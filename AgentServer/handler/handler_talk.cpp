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

#include "player.hpp"
#include "interactive_npc.hpp"

#include <packet_talk.hpp>

int StateGame::OnTalkInitiate (IPacket &packet)
{
    uint32_t npcID = packet.Read<uint32_t>();

    uint8_t task = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<NPC> npc = m_server->GetWorld()->FindNPC_CloseToPosition(npcID,player->get_position());

    if (!npc)
    {
#if defined DEBUG
        syslog(LOG_INFO,"Talking to a non-npc object.");
#endif
        return MSG_ERROR;
    }

    boost::shared_ptr<InteractiveNPC> itv = boost::shared_dynamic_cast<InteractiveNPC,NPC>(npc);

    if (!itv)
        return MSG_ERROR;

    if (task && !itv->IsTaskAvailable(task,player->level()))
    {
#if defined DEBUG
        syslog(LOG_INFO,"This NPC dosent contain this task or the player dont meet the level requirements.");
#endif
        return MSG_ERROR;
    }

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::TalkInitiate(pkt,task);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateGame::OnTalkTerminate (IPacket &packet)
{
    uint32_t npcID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    player->select_object(0);

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::TalkTerminate(pkt);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}
