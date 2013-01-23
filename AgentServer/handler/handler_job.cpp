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
#include "trade_manager.hpp"
#include "npc_util.hpp"

#include <player.hpp>

/// SRDL INCLUDES
#include <job_query.hpp>

/// SRNL INCLUDES
#include <packet_job.hpp>

int StateGame::OnJobJoin (IPacket &packet)
{
    uint32_t npcID = packet.Read<uint32_t>();

    uint8_t type = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    World *world = m_server->GetWorld();

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<NPC> npc = world->FindNPC_CloseToPosition(npcID,player->get_position());

    ///TODO CHECK IF THE NPC IS A JOB MANAGER
    if (!npc)
        return MSG_ERROR_ARG;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    /// CHECK IF THE PLAYER HAS THE MINIMUM JOIN LEVEL
    if (player->level() < JOB_MINIMUM_LEVEL)
    {
        if (pkt)
        {
            srv_pkt::JobJoin(pkt,srv_pkt::JOB_ERROR_INSUFFICIENT_LEVEL);
            m_connection->Send(pkt);
        }

        return MSG_SUCCESS;
    }

    /// CHECK IF THE PLAYER HAS THE REQUIRED JOIN FEED
    if (player->get_gold() < JOB_LEAGUE_ENTRY_FEED)
    {
        if (pkt)
        {
            srv_pkt::JobJoin(pkt,srv_pkt::JOB_ERROR_INSUFFICIENT_GOLD);
            m_connection->Send(pkt);
        }

        return MSG_SUCCESS;
    }

    /// CHECK IF THE PLAYER HAS A JOB ALREDY
    Job job = player->get_job();

    if (job.get_type() != JOB_NONE)
    {
        return MSG_SUCCESS;;
    }

    /// CHECK IF HAVE A JOIN PENALTY

    switch(type)
    {
    case JOB_TRADER:
    case JOB_THIEF:
    case JOB_HUNTER:
        job.set_type(type);
        break;
    default:
        return MSG_ERROR_ARG;
    }

    job.set_level(1);

    player->set_job(job);

    player->spend_gold(JOB_LEAGUE_ENTRY_FEED);

    if (pkt)
    {
        srv_pkt::JobJoin(pkt,type);
        m_connection->Send(pkt);
    }

    DB::JOB::Create create_query;
    create_query(m_server->DBConnection(),player->ID(),job);

    return MSG_SUCCESS;
}

int StateGame::OnJobAlias (IPacket &packet)
{
    enum JOB_ALIAS_ACTION
    {
            ALIAS_CHECK,
            ALIAS_CREATE
    };

    uint32_t npcID = packet.Read<uint32_t>();

    uint8_t action = packet.Read<uint8_t>();

    std::string alias = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    World *world = m_server->GetWorld();

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<NPC> npc = world->FindNPC_CloseToPosition(npcID,player->get_position());

    ///TODO CHECK IF THE NPC IS A JOB MANAGER
    if (!npc)
        return MSG_ERROR_ARG;

    Job job = player->get_job();

    if (job.get_type() == Job::NONE)
        return MSG_ERROR_ORDER;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (player->IsJobbing())
    {
        return MSG_SUCCESS;
    }

    DB::JOB::Check check_query;

    switch(action)
    {
    case ALIAS_CHECK:
        {
            srv_pkt::JOB_MSG_TYPE type;

            if (!check_query(m_server->DBConnection(),alias))
                type = srv_pkt::JOB_ALIAS_VALID;
            else
                type = srv_pkt::JOB_ALIAS_EXISTS;

            if (pkt)
            {
                srv_pkt::JobAlias(pkt,type,alias);
                m_connection->Send(pkt);
            }
        }
        break;
    case ALIAS_CREATE:
        {
            if (!check_query(m_server->DBConnection(),alias))
            {
                job.set_alias(alias);

                player->set_job(job);

                if (pkt)
                {
                    srv_pkt::JobAlias(pkt,srv_pkt::JOB_ALIAS_CREATE,alias);
                    m_connection->Send(pkt);
                }

                DB::JOB::Alias alias_query;
                alias_query(m_server->DBConnection(),player->ID(),alias);
            }
        }
        break;
    default:
        return MSG_ERROR_ARG;
    }

    return MSG_SUCCESS;
}

int StateGame::OnJobPastInfo (IPacket &packet)
{
    uint32_t npcID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    World *world = m_server->GetWorld();

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<NPC> npc = world->FindNPC_CloseToPosition(npcID,player->get_position());

    ///TODO CHECK IF THE NPC IS A JOB MANAGER
    if (!npc)
        return MSG_ERROR_ARG;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::JobPastInfo(pkt,srv_pkt::JOB_ERROR_NO_PAST_JOB_INFORMATION);
        m_connection->Send(pkt);
    }

    return MSG_SUCCESS;
}

int StateGame::OnJobPayment (IPacket &packet)
{
    uint32_t npcID = packet.Read<uint32_t>();

    uint8_t reserved = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    World *world = m_server->GetWorld();

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<NPC> npc = world->FindNPC_CloseToPosition(npcID,player->get_position());

    ///TODO CHECK IF THE NPC IS A JOB MANAGER
    if (!npc)
        return MSG_ERROR_ARG;

    DB::JOB::Payment query;
    uint32_t amount = query(m_server->DBConnection(),player->ID());

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::JobPayment(pkt,amount);
        m_connection->Send(pkt);
    }

    return MSG_SUCCESS;
}

int StateGame::OnJobRanking (IPacket &packet)
{
    enum
    {
        JOB_RANKING,
        JOB_WEEKLY
    };

    uint32_t npcID = packet.Read<uint32_t>();

    uint8_t job = packet.Read<uint8_t>();

    uint8_t type = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    World *world = m_server->GetWorld();

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<NPC> npc = world->FindNPC_CloseToPosition(npcID,player->get_position());

    ///TODO CHECK IF THE NPC IS A JOB MANAGER
    if (!npc)
        return MSG_ERROR_ARG;

    std::vector<Job> list;

    switch(type)
    {
    case JOB_RANKING:
        DB::JOB::Ranking rnk_query;
        list = rnk_query(m_server->DBConnection(),job);
        break;
    case JOB_WEEKLY:
        DB::JOB::Weekly wkl_query;
        list = wkl_query(m_server->DBConnection(),job);
        break;
    default:
        return MSG_ERROR_ARG;
    }

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::JobRanking(pkt,job,type,list);
        m_connection->Send(pkt);
    }

    return MSG_SUCCESS;
}

int StateGame::OnJobLeave (IPacket &packet)
{
    uint32_t npcID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    World *world = m_server->GetWorld();

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<NPC> npc = world->FindNPC_CloseToPosition(npcID,player->get_position());

    ///TODO CHECK IF THE NPC IS A JOB MANAGER
    if (!npc)
        return MSG_ERROR_ARG;

    Job job;

    player->set_job(job);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (pkt)
    {
        srv_pkt::JobLeave(pkt);
        m_connection->Send(pkt);
    }

    DB::JOB::Delete query;
    query(m_server->DBConnection(),player->ID());

    return MSG_SUCCESS;
}

int StateGame::OnJobProductInformation (IPacket &packet)
{
    uint32_t refCharID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    World *world = m_server->GetWorld();

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (!IsSpecialShop(refCharID))
        return MSG_ERROR_ARG;

    Market market = world->GetTradeManager()->FindMarket(refCharID);

    if (market.refCharID == refCharID)
    {
        boost::shared_ptr<OPacket> pkt(new OPacket);

        if (pkt)
        {
            srv_pkt::JobProductInformation(pkt,market);
            m_connection->Send(pkt);
        }
    }

    return MSG_SUCCESS;
}
