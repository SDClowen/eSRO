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

/// SRLF INCLUDES
#include <player.hpp>
#include <transport.hpp>
#include <interactive_npc.hpp>
#include <teleport_building.hpp>

/// SRDL INCLUDES
#include <player_query.hpp>

/// SRNL INCLUDES
#include <packet_player.hpp>
#include <packet_teleport.hpp>

int StateGame::OnTeleport (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    disconnect_player_signals();

    if (player->IsBerserk())
        player->berserk(false);

    player->set_attack_state(ATTACK_ACTION_STOP);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::InitScreen(pkt);
        m_connection->Send(pkt);
    }

    if (player->IsDead())
    {
        player->resurrect(false,false,0,false);

        Teleport tlp = m_server->GetWorld()->FindTeleport(player->get_return_point());

        if (!tlp.ID)
        {
#if defined DEBUG
            syslog(LOG_INFO,"Invalid Return Point - %i",player->get_return_point());
#endif
            return MSG_ERROR;
        }

        player->set_position(Coord(tlp.x,tlp.y,tlp.z,tlp.Zone));
    }

    player->SetCurrentTask(PLAYER_TASK_IDLE);

    pkt.reset(new OPacket);

    if (pkt)
    {
        srv_pkt::PlayerInfo(pkt,player,static_cast<srv::Connection*>(m_connection)->AccountID());
        m_connection->Send(pkt);
    }

    pkt.reset(new OPacket);

    if (pkt)
    {
        srv_pkt::EndScreen(pkt);
        m_connection->Send(pkt);
    }

    pkt.reset(new OPacket);

    if (pkt)
    {
        srv_pkt::PlayerID(pkt,player);
        m_connection->Send(pkt);
    }

    return MSG_SUCCESS;
}

int StateGame::OnTeleportCancel (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player || player->GetCurrentTask() != PLAYER_TASK_TELEPORT)
        return MSG_ERROR;

    m_server->GetWorld()->TerminateTask(player->GetUniqueID());

    return MSG_SUCCESS;
}


int StateGame::OnTeleportDesignate (IPacket &packet)
{
    uint32_t ID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (player->get_selected_object() != ID)
        return MSG_ERROR_ARG;

    boost::shared_ptr<TeleportBuilding> blg;
    blg = m_server->GetWorld()->FindBuilding_CloseToPosition(ID,player->get_position());

    if (!blg)
        return MSG_ERROR_ARG;

    bool valid = false;

    for (size_t n = 0; n < blg->Tasks.size(); ++n)
    {
        if (blg->Tasks[n] == TELEPORT_TASK_DESIGNATE)
            valid = true;
    }

    if (!valid)
        return MSG_SUCCESS;

    player->set_return_point(blg->TeleportID);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::TeleportDesignate(pkt);
        m_connection->Send(pkt);
    }

    DB::PLAYER::ReturnPoint query;
    query(m_server->DBConnection(),player->ID(),blg->TeleportID);

    return MSG_SUCCESS;
}

int StateGame::OnTeleportToDestination (IPacket &packet)
{
    uint32_t ID =  packet.Read<uint32_t>();

    uint8_t resv = packet.Read<uint8_t>();

    uint32_t dest = 0;

    if (resv == 2)
        dest = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (player->get_selected_object() != ID)
        return MSG_ERROR_ARG;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (player->GetCurrentTask() == PLAYER_TASK_TELEPORT)
    {
        if (pkt)
        {
            srv_pkt::TeleportDestination(pkt,srv_pkt::TELEPORT_DST_ERROR_BEING_TELEPORTED);
            m_connection->Send(pkt);
        }

        return MSG_SUCCESS;
    }

    if (!player->IsTaskInterruptable())
        return MSG_SUCCESS;

    boost::shared_ptr<Transport> transport = player->GetTransport();

    if (transport && transport->has_goods())
    {
#if defined DEBUG
        syslog(LOG_INFO,"Cant teleport while carrying goods in a transport.");
#endif
        return MSG_SUCCESS;
    }

    /// CHECK IF WE ARE SELECTING A TELEPORT BUILDING OR A NPC ELSE IS AN ERROR.

    uint32_t TeleportID = 0;
    boost::shared_ptr<TeleportBuilding> blg;
    blg = m_server->GetWorld()->FindBuilding_CloseToPosition(ID,player->get_position());

    if (blg)
    {
        for (size_t n = 0; n < blg->Tasks.size(); ++n)
        {
            switch(blg->Tasks[n])
            {
            case TELEPORT_TASK_TELEPORT_RETURN_POINT:
                dest = player->get_return_point();
                break;
            case TELEPORT_TASK_TELEPORT:
                TeleportID = blg->TeleportID;
                break;
            default:
                break;
            }
        }
    }
    else
    {
        boost::shared_ptr<NPC> npc;
        npc = m_server->GetWorld()->FindNPC_CloseToPosition(ID,player->get_position());

        if (!npc)
            return MSG_ERROR_ARG;

        boost::shared_ptr<InteractiveNPC> interactive = boost::shared_dynamic_cast<InteractiveNPC,NPC>(npc);

        if (!interactive)
            return MSG_ERROR_ARG;

        TeleportID = interactive->GetTeleportID();
    }

    if (!dest)
        return MSG_SUCCESS;

    /// CHECK THAT THE PLAYER MEETS ALL THE REQUIREMENTS.

    Teleport tlp = m_server->GetWorld()->FindTeleport(dest);

    if (!tlp.ID)
        return MSG_SUCCESS;

    if (TeleportID)
    {
        TeleportLink link = m_server->GetWorld()->FindTeleportLink(TeleportID,dest);

        if (player->get_maxlv() < link.minlv && player->get_maxlv() < link.maxlv)
        {
            if (pkt)
            {
                srv_pkt::TeleportDestination(pkt,srv_pkt::TELEPORT_DST_ERROR_INVALID_LEVEL);
                m_connection->Send(pkt);
            }

            return MSG_SUCCESS;
        }

        /// TODO: CHECK FORTRESS REQUIREMENTS!!

        if (link.tax && !player->spend_gold(link.tax))
        {
            if (pkt)
            {
                srv_pkt::TeleportDestination(pkt,srv_pkt::TELEPORT_DST_ERROR_INSUFFICIENT_GOLD);
                m_connection->Send(pkt);
            }

            return MSG_SUCCESS;
        }
    }

    player->SetCurrentTask(PLAYER_TASK_TELEPORT);

    if (pkt)
    {
        srv_pkt::TeleportDestination(pkt);
        m_connection->Send(pkt);
    }

    Coord pos = Coord(tlp.x,tlp.y,tlp.z,tlp.Zone);

    if (!m_server->GetWorld()->InitTaskTeleport(player->GetUniqueID(),pos,0))
    {
#if defined DEBUG
        syslog(LOG_INFO,"Failed to initiate teleport task - Zone %i.",pos.get_zone());
#endif
    }

    return MSG_SUCCESS;
}
