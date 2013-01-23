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

#include "packet.hpp"
#include "packet_npc.hpp"
#include "opcodes_shard_server.hpp"

#include <entity_state.hpp>
#include <interactive_npc.hpp>
#include <teleport_building.hpp>
#include <npc_list.hpp>

namespace srv_pkt
{

void HPMP (const boost::shared_ptr<OPacket> &pkt, const uint32_t entityID, uint8_t op, bool hasStatus, uint8_t type,
    const uint32_t value, uint32_t EncStatus, const std::map<uint32_t,Status> &status)
{
    pkt->WriteOpcode(SERVER_HPMP);
    pkt->Write<uint32_t>(entityID);
    pkt->Write<uint8_t>(op);
    pkt->Write<uint8_t>(hasStatus);
    pkt->Write<uint8_t>(type);

    if ((type & STAT_HP) || (type & STAT_MP))
        pkt->Write<uint32_t>(value);

    if (hasStatus)
    {
        pkt->Write(EncStatus);

        if (EncStatus)
        {
            std::map<uint32_t,Status>::const_iterator si;
            for ( si = status.begin(); si != status.end(); ++si)
            {
                if (si->second.Level)
                    pkt->Write<uint8_t>(si->second.Level);
            }
        }
    }
}

void BadStatus (const boost::shared_ptr<OPacket> &pkt, const uint32_t entityID, uint32_t EncStatus,
    const std::map<uint32_t,Status> &status)
{
    pkt->WriteOpcode(SERVER_HPMP);
    pkt->Write<uint32_t>(entityID);
    pkt->Write<uint8_t>(1);
    pkt->Write<uint8_t>(true);
    pkt->Write<uint8_t>(STAT_STATUS);
    pkt->Write(EncStatus);

    std::map<uint32_t,Status>::const_iterator si;
    for ( si = status.begin(); si != status.end(); ++si)
    {
        if (si->second.Level)
            pkt->Write<uint8_t>(si->second.Level);
    }
}

void StatusDmg (const boost::shared_ptr<OPacket> &pkt, const uint32_t entityID, uint32_t damage)
{
    pkt->WriteOpcode(SERVER_STATUS_DAMAGE);
    pkt->Write<uint32_t>(entityID);
    pkt->Write<uint32_t>(damage);
}

void Movement (const boost::shared_ptr<OPacket> &pkt, const uint32_t entityID, const Coord &dest)
{
    pkt->WriteOpcode(SERVER_NPC_MOVEMENT);
    pkt->Write<uint32_t>(entityID);
    pkt->Write<uint8_t>(1);
    pkt->Write<uint16_t>(dest.get_zone());
    pkt->Write<uint16_t>(dest.get_x());
    pkt->Write<uint16_t>(dest.get_z());
    pkt->Write<uint16_t>(dest.get_y());
    pkt->Write<uint8_t>(0);
}

void Rotate (const boost::shared_ptr<OPacket> &pkt, const uint32_t entityID, int16_t angle)
{
    pkt->WriteOpcode(SERVER_PLAYER_ROTATE);
    pkt->Write<uint32_t>(entityID);
    pkt->Write<int16_t>(angle);
}

void State (const boost::shared_ptr<OPacket> &pkt, const uint32_t entityID, const uint8_t type, const uint8_t action)
{
    pkt->WriteOpcode(SERVER_UPDATE_STATE);
    pkt->Write<uint32_t>(entityID);
    pkt->Write<uint8_t>(type);
    pkt->Write<uint8_t>(action);

    if (type == STATE_ATTRIBUTE && action == ATTRIBUTE_ACTION_BERSERK)
        pkt->Write<uint8_t>(0);
}

void Speed (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<NPC> &npc)
{
    pkt->WriteOpcode(SERVER_UPDATE_SPEED);
    pkt->Write<uint32_t>(npc->GetUniqueID());
    pkt->Write<float>(npc->walking_speed());
    pkt->Write<float>(npc->running_speed());
}

void Position (OPacket *pkt, const uint32_t entityID, const Coord &pos, uint16_t angle)
{
    pkt->WriteOpcode(SERVER_ENTITY_UPDATE_POSITION);
    pkt->Write<int16_t>(pos.get_zone());
    pkt->Write<float>(pos.get_x());
    pkt->Write<float>(pos.get_z());
    pkt->Write<float>(pos.get_y());
    pkt->Write<uint16_t>(angle);
    pkt->Write<uint32_t>(entityID);
}

void SelectTarget (const boost::shared_ptr<OPacket> &pkt, const uint32_t entityID)
{
    pkt->WriteOpcode(SERVER_PLAYER_SELECT_TARGET,ANSWER_ACCEPT);
    pkt->Write<uint32_t>(entityID);
}

void SelectTarget (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<NPC> &target)
{
    pkt->WriteOpcode(SERVER_PLAYER_SELECT_TARGET,ANSWER_ACCEPT);
    pkt->Write<uint32_t>(target->GetUniqueID());
    pkt->Write<uint8_t>(1);
    pkt->Write<uint32_t>(target->hp());
    pkt->Write<uint8_t>(0x01);       ///<--- TYPE OF MOB I THINK
    pkt->Write<uint8_t>(0x05);
}

void SelectTarget (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<InteractiveNPC> &npc,
    const uint8_t lv)
{
    pkt->WriteOpcode(SERVER_PLAYER_SELECT_TARGET,ANSWER_ACCEPT);
    pkt->Write<uint32_t>(npc->GetUniqueID());
    pkt->Write<uint8_t>(0);

    std::vector<uint8_t> tasks = npc->GetAvailableTasks(lv);

    pkt->Write<uint8_t>(tasks.size());
    for (std::vector<uint8_t>::const_iterator i = tasks.begin(); i != tasks.end(); ++i)
        pkt->Write<uint8_t>(*i);

    pkt->Write<uint8_t>(0);

    if (npc->isFortressActive())
        pkt->Write<int16_t>(npc->GetFortressTax());
}

void SelectTarget (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Player> &player)
{
    pkt->WriteOpcode(SERVER_PLAYER_SELECT_TARGET,ANSWER_ACCEPT);
    pkt->Write<uint32_t>(player->GetUniqueID());
    pkt->Write<uint8_t>(0x01);
    pkt->Write<uint8_t>(0x05);

    if (player->IsJobbing())
    {
        pkt->Write<uint8_t>(player->get_job().get_type());
        pkt->Write<uint8_t>(player->get_job().get_level());
    }
    else
        pkt->Write<uint8_t>(4);
}

void SelectTarget (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<TeleportBuilding> &blg)
{
    pkt->WriteOpcode(SERVER_PLAYER_SELECT_TARGET,ANSWER_ACCEPT);
    pkt->Write<uint32_t>(blg->GetUniqueID());

    pkt->Write<uint8_t>(blg->Tasks.size());

    for (size_t i = 0; i < blg->Tasks.size(); ++i)
        pkt->Write<uint8_t>(blg->Tasks[i]);

    if (blg->TaxActive)
        pkt->Write<uint16_t>(blg->TaxRate);
}

}
