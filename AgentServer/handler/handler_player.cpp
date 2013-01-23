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
#include "map_manager.hpp"
#include "skill_manager.hpp"
#include "emote_actions.hpp"
#include "near_objects.hpp"
#include "formulas.hpp"

/// SRLF INCLUDES
#include <skill.hpp>
#include <party.hpp>
#include <storage.hpp>
#include <transport.hpp>
#include <interactive_npc.hpp>

/// SRDL INCLUDES
#include <player_query.hpp>
#include <hotkeys_query.hpp>
#include <block_query.hpp>

/// SRNL INCLUDES
#include <packet_item.hpp>
#include <packet_spawn.hpp>
#include <packet_player.hpp>
#include <packet_attack.hpp>
#include <packet_skill.hpp>
#include <packet_npc.hpp>
#include <packet_teleport.hpp>

#if defined DEBUG
#include <packet_util_functions.hpp>
#endif

void StateGame::connect_player_signals ()
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (player)
    {
        player->signal_dead.connect(boost::bind(&StateGame::OnPlayerDead,this));
        player->signal_state.connect(boost::bind(&StateGame::OnPlayerState,this,_1,_2));
        player->m_gold_changed.connect(boost::bind(&StateGame::OnPlayerGold,this));
        player->m_sp_changed.connect(boost::bind(&StateGame::OnPlayerSP,this));
        player->signal_stats.connect(boost::bind(&StateGame::OnPlayerStats,this));
        player->signal_speed.connect(boost::bind(&StateGame::OnPlayerSpeed,this));
        player->signal_connect_party.connect(boost::bind(&StateGame::connect_party_signals,this));

        connect_storage_signals(player->get_storage(STORAGE_INVENTORY));
        connect_storage_signals(player->get_storage(STORAGE_AVATAR));
        connect_storage_signals(player->get_storage(STORAGE_MAIN));
    }
}

void StateGame::disconnect_player_signals ()
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (player)
    {
        player->signal_dead.disconnect(boost::bind(&StateGame::OnPlayerDead,this));
        player->signal_state.disconnect(boost::bind(&StateGame::OnPlayerState,this,_1,_2));
        player->m_gold_changed.disconnect(boost::bind(&StateGame::OnPlayerGold,this));
        player->m_sp_changed.disconnect(boost::bind(&StateGame::OnPlayerSP,this));
        player->signal_stats.disconnect(boost::bind(&StateGame::OnPlayerStats,this));
        player->signal_speed.disconnect(boost::bind(&StateGame::OnPlayerSpeed,this));
        player->signal_connect_party.disconnect(boost::bind(&StateGame::connect_party_signals,this));

        disconnect_storage_signals(player->get_storage(STORAGE_INVENTORY));
        disconnect_storage_signals(player->get_storage(STORAGE_AVATAR));
        disconnect_storage_signals(player->get_storage(STORAGE_MAIN));
    }
}

int StateGame::OnSelectTarget (IPacket &packet)
{
    uint32_t ID =  packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (ID == player->GetUniqueID())
    {
        srv_pkt::SelectTarget(pkt,ID);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    boost::shared_ptr<Transport> transport = player->m_transport;

    if (transport && transport->GetUniqueID() == ID)
    {
        srv_pkt::SelectTarget(pkt,transport);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    player->select_object(ID);

    boost::shared_ptr<NPC> npc = m_server->GetWorld()->FindNPC_CloseToPosition(ID,player->get_position());

    if (npc)
    {
        if (npc->IsDead())
            return MSG_SUCCESS;

        if (npc->type() == ET_INTERACTIVE)
        {
            boost::shared_ptr<InteractiveNPC> itv = boost::shared_dynamic_cast<InteractiveNPC,NPC>(npc);

            srv_pkt::SelectTarget(pkt,itv,player->level());
        }
        else
        {
            srv_pkt::SelectTarget(pkt,npc);
        }

        m_connection->Send(pkt);

        return MSG_SUCCESS;
    }

    boost::shared_ptr<TeleportBuilding> blg;
    blg = m_server->GetWorld()->FindBuilding_CloseToPosition(ID,player->get_position());

    if (blg)
    {
        srv_pkt::SelectTarget(pkt,blg);
        m_connection->Send(pkt);

        return MSG_SUCCESS;
    }

    boost::shared_ptr<Player> target = m_server->GetWorld()->FindPlayer_CloseToPosition(ID,player->get_position());

    if (target)
    {
        srv_pkt::SelectTarget(pkt,target);
        m_connection->Send(pkt);

        return MSG_SUCCESS;
    }

    player->select_object(0);

    return MSG_SUCCESS;
}

int StateGame::OnMovement (IPacket &packet)
{
    uint8_t type = packet.Read<uint8_t>();

    if (type)
    {
        Coord new_pos;

        if ( packet.size() == 9 )
        {
            int16_t zone = packet.Read<int16_t>();

            int16_t xpos = packet.Read<int16_t>();

            int16_t zpos = packet.Read<int16_t>();

            int16_t ypos = packet.Read<int16_t>();

            if (!packet.EndOfStream())
                return MSG_ERROR_SIZE;

            new_pos = Coord(xpos,ypos,zpos,zone);
        }
        else if (packet.size() == 15)
        {
            int16_t zone = packet.Read<int16_t>();

            int32_t xpos = packet.Read<int32_t>();

            int32_t zpos = packet.Read<int32_t>();

            int32_t ypos = packet.Read<int32_t>();

            if (!packet.EndOfStream())
                return MSG_ERROR_SIZE;

            new_pos = Coord(xpos,ypos,zpos,zone);
        }
        else
        {
#if defined DEBUG
            syslog(LOG_INFO,"Unknown packet size (%i)",packet.size());
#endif
            return MSG_ERROR_SIZE;
        }

        boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

        if (!player)
            return MSG_ERROR;

#if defined DEBUG
        Coord current = player->get_position();

        syslog (LOG_INFO,"Player %s position %i, %f %f %f",player->get_name().c_str(),current.get_zone(),
            current.get_x(),current.get_z(),current.get_y());

        syslog (LOG_INFO,"Player %s moving to %i, %f %f %f",player->get_name().c_str(),new_pos.get_zone(),
            new_pos.get_x(),new_pos.get_z(),new_pos.get_y());
#endif

        Coord mpos;
        if (!m_server->GetWorld()->resolveMotion(player->get_position(),new_pos,&mpos))
            return MSG_SUCCESS;

        new_pos = mpos;

#if defined DEBUG
            syslog (LOG_INFO,"Player %s actual destination %i, %f %f %f",player->get_name().c_str(),
                new_pos.get_zone(),new_pos.get_x(),new_pos.get_z(),new_pos.get_y());
#endif

        if (distance(player->get_position(),new_pos) > 1000)
        {
#if defined DEBUG
            syslog(LOG_INFO,"StateGame::OnMovement - Position to far from the player.");
#endif
            return MSG_SUCCESS;
        }

        if (player->IsDead() || !player->IsTaskInterruptable() || player->get_stance_state() == MOVEMENT_ACTION_SIT)
            return MSG_SUCCESS;

        if (player->hasStatus(STATUS_GROUP_RESTRAIN) || player->hasWall())
            return MSG_SUCCESS;

        boost::shared_ptr<Transport> transport = player->m_transport;

        if (transport && transport->IsMounted())
            return MSG_SUCCESS;

        if (player->GetCurrentTask() != PLAYER_TASK_MOVE)
        {
            m_server->GetWorld()->TerminateTask(player->GetUniqueID());

            player->set_destination(new_pos);

            player->SetCurrentTask(PLAYER_TASK_MOVE);

            m_server->GetWorld()->InitTaskMove(player->GetUniqueID());
        }
        else
            player->set_destination(new_pos);

        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::Movement(pkt,player->GetUniqueID(),new_pos);
        m_server->SendChannel(pkt,player->get_position().get_zone());

        if (transport && transport->is_moving())
        {
            Coord pos = RangePosition(transport->get_position(),player->get_position(),40,NULL);

            transport->set_destination(pos);

            pkt.reset(new OPacket);

            srv_pkt::Movement(pkt,transport->GetUniqueID(),pos);
            m_server->SendChannel(pkt,transport->get_position().get_zone());
        }
    }
    else if (type == 0)
    {
        int8_t unk = packet.Read<int8_t>();

        uint16_t angle = packet.Read<uint16_t>();

        if (!packet.EndOfStream())
            return MSG_ERROR_SIZE;

        boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

        if (!player)
            return MSG_ERROR;

        if (player->IsDead() || !player->IsTaskInterruptable() || player->get_stance_state() == MOVEMENT_ACTION_SIT)
            return MSG_SUCCESS;

        if (player->hasStatus(STATUS_GROUP_RESTRAIN) || player->hasWall())
            return MSG_SUCCESS;

    }
    else
        return MSG_ERROR_ARG;

    return MSG_SUCCESS;
}

int StateGame::OnRotate (IPacket &packet)
{
    uint16_t angle = packet.Read<uint16_t>(); /// TO CONVERT ANGLE TO DEGREES 360/0x10000

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (player->IsDead() || !player->IsTaskInterruptable() || player->get_stance_state() == MOVEMENT_ACTION_SIT)
        return MSG_SUCCESS;

    if (player->hasStatus(STATUS_GROUP_RESTRAIN))
        return MSG_SUCCESS;

    player->set_angle(angle);

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::Rotate(pkt,player->GetUniqueID(),angle);
    m_server->SendChannel(pkt,player->get_position().get_zone());

    DB::PLAYER::Angle query;
    query(m_server->DBConnection(),player->ID(),angle);

    return MSG_SUCCESS;
}

int StateGame::OnTranslate (IPacket &packet)
{
    int16_t pos = packet.Read<int16_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

#if defined DEBUG
    syslog(LOG_INFO,"Translate %i",pos);
#endif

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (player->IsDead() || !player->IsTaskInterruptable() || player->get_stance_state() == MOVEMENT_ACTION_SIT)
        return MSG_SUCCESS;

    if (player->hasStatus(STATUS_GROUP_RESTRAIN))
        return MSG_SUCCESS;

    return MSG_SUCCESS;
}

int StateGame::OnCastBerserk (IPacket &packet)
{
	uint8_t type = packet.Read<uint8_t>();

	if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

#if defined DEBUG
    syslog(LOG_INFO,"OnCastBerserk - Type = %i",type);
#endif

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (player->IsDead())
        return MSG_SUCCESS;

    if (player->get_berserk_count() < MAX_PLAYER_BERSERK_COUNT)
        return MSG_SUCCESS;

    STask task;
    task.task = STATE_TASK_BERSERK;
    task.seconds = 60;
    task.playerID = player->GetUniqueID();

    player->berserk(true);

    m_server->GetWorld()->InitStateTask(task);

    DB::PLAYER::Bersek query;
    query(m_server->DBConnection(),player->ID(),player->get_berserk_count());

    return MSG_SUCCESS;
}

int StateGame::OnEmote (IPacket &packet)
{
    uint8_t emote_action = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    if (emote_action > UIIT_STT_COSNEWUI_CHARMS)
        return MSG_ERROR_ARG;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (player->IsDead() || !player->IsTaskInterruptable() || player->get_stance_state() == MOVEMENT_ACTION_SIT)
        return MSG_SUCCESS;

    if (player->hasStatus(STATUS_GROUP_RESTRAIN))
        return MSG_SUCCESS;

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::PlayerEmote(pkt,player,emote_action);
    m_server->SendChannel(pkt,player->get_position().get_zone());

    return MSG_SUCCESS;
}

int StateGame::OnHelpIcon (IPacket &packet)
{
    enum
    {
        HELP_DEACTIVE = 1,
        HELP_ACTIVATE = 3
    };

    uint8_t action = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (!player->help_mark() && action == HELP_DEACTIVE)
        return MSG_ERROR_ARG;

    if (player->help_mark() && action == HELP_ACTIVATE)
        return MSG_ERROR_ARG;

    if (action == HELP_ACTIVATE)
        player->help_mark(true);
    else
        player->help_mark(false);

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::PlayerHelp(pkt,player,action);
    m_server->SendChannel(pkt,player->get_position().get_zone());

    return MSG_SUCCESS;
}

int StateGame::OnStance(IPacket &packet)
{
    enum
    {
        STANCE_ACTION_WALK = 2,
        STANCE_ACTION_RUN = 3,
        STANCE_ACTION_POSITION = 4
    };

    uint8_t action = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (!player->IsTaskInterruptable() || player->hasStatus(STATUS_GROUP_RESTRAIN))
        return MSG_SUCCESS;

    switch(action)
    {
    case STANCE_ACTION_WALK:
    case STANCE_ACTION_RUN:
        player->set_movement_state(static_cast<STATE_MOVEMENT_ACTION>(action));
        break;
    case STANCE_ACTION_POSITION:
        player->change_stance();
        m_server->GetWorld()->TerminateTask(player->GetUniqueID());

        if (player->get_stance_state() == MOVEMENT_ACTION_SIT)
            m_server->GetWorld()->InitTaskIdle(player->GetUniqueID());
        break;
    default:
        return MSG_ERROR_ARG;
    }

    return MSG_SUCCESS;
}

int StateGame::OnResurrect (IPacket &packet)
{
    enum
    {
        RESURRECT_TOWN = 1,
        RESURRECT_PLACE
    };

    uint8_t action = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player || !player->IsDead())
        return MSG_ERROR;

    switch(action)
    {
    case RESURRECT_TOWN:
        {
            Teleport tlp = m_server->GetWorld()->FindTeleport(player->get_return_point());

            if (!tlp.ID)
            {
#if defined DEBUG
                syslog(LOG_INFO,"Invalid Return Point - %i",player->get_return_point());
#endif
                return MSG_ERROR;
            }

            Coord pos = Coord(tlp.x,tlp.y,tlp.z,tlp.Zone);

            player->SetCurrentTask(PLAYER_TASK_TELEPORT);

            m_server->GetWorld()->InitTaskTeleport(player->GetUniqueID(),pos,0);
        }
        break;
    case RESURRECT_PLACE:
        {
            player->resurrect(true,false,0,false);
            player->SetCurrentTask(PLAYER_TASK_IDLE);

            boost::shared_ptr<OPacket> pkt(new OPacket);

            std::map<uint32_t,Status> status = player->GetStatus();

            uint8_t op = srv_pkt::BASE_OP_INC_SELF;
            uint8_t type = status.empty() ? srv_pkt::STAT_HP : srv_pkt::STAT_HP | srv_pkt::STAT_STATUS;

            srv_pkt::HPMP(pkt,player->GetUniqueID(),op,status.size(),type,player->hp(),player->GetEncodedStatus(),status);
            m_server->SendChannel(pkt,player->get_position().get_zone());

            type = status.empty() ? srv_pkt::STAT_MP : srv_pkt::STAT_MP | srv_pkt::STAT_STATUS;

            pkt.reset(new OPacket);

            srv_pkt::HPMP(pkt,player->GetUniqueID(),op,status.size(),type,player->mp(),player->GetEncodedStatus(),status);
            m_server->SendChannel(pkt,player->get_position().get_zone());

            STask task;
            task.task = STATE_TASK_INVUNERABLE;
            task.seconds = PLAYER_INVUNERABLE_STATE_TIME;
            task.playerID = player->GetUniqueID();

            m_server->GetWorld()->InitStateTask(task);

            boost::shared_ptr<Party> party = player->get_party();

            if (party)
            {
                party->SendHPMPNotification(player->GetUniqueID(),player->hp(),player->max_hp(),player->mp(),
                    player->max_mp());
            }
        }
        break;
    default:
        return MSG_ERROR_ARG;
    }

    return MSG_SUCCESS;
}

int StateGame::OnPlayerAction (IPacket &packet)
{
    enum
    {
        PLAYER_ACTION_ATTACK = 1,
        PLAYER_ACTION_PICKUP_ITEM ,
        PLAYER_ACTION_TRACE,
        PLAYER_ACTION_CAST_SKILL,
        PLAYER_ACTION_CANCEL_SKILL
    };

    enum
    {
        PLAYER_ACTION_EXECUTE = 1,
        PLAYER_ACTION_CANCEL
    };

    uint8_t op = packet.Read<uint8_t>();

    if ( op < PLAYER_ACTION_EXECUTE || op > PLAYER_ACTION_CANCEL)
        return MSG_ERROR_ARG;

    uint8_t ret = MSG_SUCCESS;

    if (op == PLAYER_ACTION_EXECUTE)
    {
        uint8_t action = packet.Read<uint8_t>();

        switch (action)
        {
        case PLAYER_ACTION_ATTACK:
            ret = OnPlayerAction_Attack(packet);
            break;
        case PLAYER_ACTION_PICKUP_ITEM:
            ret = OnPlayerAction_Pickup(packet);
            break;
        case PLAYER_ACTION_TRACE:
            ret = OnPlayerAction_Trace(packet);
            break;
        case PLAYER_ACTION_CAST_SKILL:
            ret = OnPlayerAction_SkillCast(packet);
            break;
        case PLAYER_ACTION_CANCEL_SKILL:
            ret = OnPlayerAction_SkillCancel(packet);
            break;
        default:
    #if defined DEBUG
            syslog(LOG_INFO,"Unknown action - OnPlayerAction() %i",action);
    #endif
            break;
        }
    }
    else
    {
        boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

        if (!player)
            return MSG_ERROR;

        if (player->GetCurrentTask() == PLAYER_TASK_CAST)
            player->SetQueueSkill(-1,0);
    }

    return ret;
}

int StateGame::OnPlayerAction_Attack (IPacket &packet)
{
    uint8_t reserved = packet.Read<uint8_t>();

    if (reserved != 1)
    {
#if defined DEBUG
        syslog(LOG_INFO,"OnPlayerAction_Attack() - reserved = %i",reserved);
#endif
        return MSG_ERROR_ARG;
    }

    uint32_t targetID = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (player->IsDead() || targetID == player->GetUniqueID())
        return MSG_SUCCESS;

    if (player->hasStatus(STATUS_GROUP_RESTRAIN | STATUS_FEAR) || player->get_stance_state() == MOVEMENT_ACTION_SIT)
        return MSG_SUCCESS;

    boost::shared_ptr<Transport> transport = player->GetTransport();

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (transport && transport->IsMounted())
    {
        srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_TRANSPORT);
        m_connection->Send(pkt);

        return MSG_SUCCESS;
    }

    if (targetID != player->get_selected_object())
    {
        srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_INVALID_TARGET);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    uint32_t skillID = player->get_base_attack();

    if (player->GetCurrentTask() == PLAYER_TASK_CAST_WAIT || !player->IsTaskInterruptable())
    {
        player->SetQueueSkill(skillID,targetID);
        return MSG_SUCCESS;
    }

    m_server->GetWorld()->InitTaskCastSkill(player->GetUniqueID(),targetID,skillID,Coord());

    return MSG_SUCCESS;
}

int StateGame::OnPlayerAction_Pickup (IPacket &packet)
{
    uint8_t reserved = packet.Read<uint8_t>();

    if (reserved != 1)
    {
#if defined DEBUG
        syslog(LOG_INFO,"OnPlayerAction_Pickup - reserved = %i",reserved);
#endif
        return MSG_ERROR_ARG;
    }

    uint32_t objectID = packet.Read<uint32_t>();

    if (!objectID)
        return MSG_SUCCESS;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (player->IsDead() || !player->IsTaskInterruptable())
        return MSG_SUCCESS;

    if (player->hasStatus(STATUS_GROUP_RESTRAIN) || player->get_stance_state() == MOVEMENT_ACTION_SIT)
        return MSG_SUCCESS;

    if(!m_server->GetWorld()->InitTaskPickup(player->GetUniqueID(),objectID))
        return MSG_ERROR;

    return MSG_SUCCESS;
}

int StateGame::OnPlayerAction_Trace (IPacket &packet)
{
#if defined DEBUG
    PrintPacketToSyslog(packet);
#endif

    return MSG_SUCCESS;
}

int StateGame::OnPlayerAction_SkillCast (IPacket &packet)
{
    enum
    {
        SKILL_CAST_SELF,
        SKILL_CAST_TARGET,
        SKILL_CAST_POSITION
    };

    uint32_t skillID = packet.Read<uint32_t>();

    uint8_t type = packet.Read<uint8_t>();

    Coord pos;
	uint32_t targetID = 0;

    switch(type)
    {
    case SKILL_CAST_SELF:
        break;
    case SKILL_CAST_TARGET:
        targetID = packet.Read<uint32_t>();
        break;
    case SKILL_CAST_POSITION:
        pos.set_zone(packet.Read<int16_t>());
        pos.set_x(packet.Read<int32_t>());
        pos.set_z(packet.Read<int32_t>());
        pos.set_y(packet.Read<int32_t>());
        break;
    default:
        break;
    }

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (player->IsDead())
        return MSG_SUCCESS;

    if (player->hasStatus(STATUS_GROUP_RESTRAIN) || player->get_stance_state() == MOVEMENT_ACTION_SIT)
        return MSG_SUCCESS;

    if (type == SKILL_CAST_POSITION && player->hasWall())
        return MSG_SUCCESS;

    boost::shared_ptr<Transport> transport = player->GetTransport();

    if (transport && transport->IsMounted())
    {
        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_TRANSPORT);
        m_connection->Send(pkt);

        return MSG_SUCCESS;
    }

    if (!player->skill_exists(skillID) || !player->IsSkillAvailable(skillID))
    {
        if (!player->is_gm())
            return MSG_SUCCESS;
    }

    if (player->GetCurrentTask() == PLAYER_TASK_CAST_WAIT || !player->IsTaskInterruptable())
    {
        if (type != SKILL_CAST_POSITION)
            player->SetQueueSkill(skillID,targetID);

        return MSG_SUCCESS;
    }

    m_server->GetWorld()->InitTaskCastSkill(player->GetUniqueID(),targetID,skillID,pos);

    return MSG_SUCCESS;
}

int StateGame::OnPlayerAction_SkillCancel (IPacket &packet)
{
    uint32_t skillID = packet.Read<uint32_t>();

    uint8_t reserv = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    if (reserv)
        return MSG_ERROR_ARG;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    std::pair<uint32_t,uint32_t> buffIDs;

    buffIDs = player->remove_buff(skillID);

    if (!buffIDs.first)
        buffIDs = player->remove_imbue(skillID);

    if (!buffIDs.first)
        return MSG_SUCCESS;

    m_server->GetWorld()->TerminateBackgroundTask(buffIDs.first);

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::TermBuff(pkt,buffIDs.second);
    m_server->SendChannel(pkt,player->get_position().get_zone());

    return MSG_SUCCESS;
}

void StateGame::OnPlayerState (uint8_t type, uint8_t action)
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (player)
    {
        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::State(pkt,player->GetUniqueID(),type,action);
        m_server->SendChannel(pkt,player->get_position().get_zone());
    }
}

void StateGame::OnPlayerGold ()
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (player)
    {
        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::PlayerGold(pkt,player);
        m_connection->Send(pkt);

        DB::PLAYER::Gold query;
        query(m_server->DBConnection(),player);
    }
}

void StateGame::OnPlayerSP ()
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (player)
    {
        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::PlayerSP(pkt,player);
        m_connection->Send(pkt);

        DB::PLAYER::SP query;
        query(m_server->DBConnection(),player->ID(),player->get_sp());
    }
}

void StateGame::OnPlayerSpeed ()
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (player)
    {
        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::Speed(pkt,player);
        m_server->SendChannel(pkt,player->get_position().get_zone());
    }
}

void StateGame::OnPlayerStats ()
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (player)
    {
        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::PlayerStats(pkt,player);
        m_connection->Send(pkt);
    }
}

void StateGame::OnPlayerDead ()
{
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (player)
    {
        player->EstablishDeathPoint();

        m_server->GetWorld()->TerminateStateTask(player->GetUniqueID());
        m_server->GetWorld()->TerminateTask(player->GetUniqueID());

        /// TODO: CLEAR BUFF AND STATUS
    }
}
