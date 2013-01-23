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

#ifndef __SLES_GAME_SERVER_NPC_PACKET_HPP__
#define __SLES_GAME_SERVER_NPC_PACKET_HPP__

#include <player.hpp>
#include <boost/shared_ptr.hpp>

class OPacket;
class TeleportBuilding;
class InteractiveNPC;

namespace srv_pkt
{

enum STAT_OP
{
    BASE_OP_DEC_HP = 0x01,
    BASE_OP_DEC_MP = 0x04,
    BASE_OP_INC_SELF = 0x10,
    BASE_OP_INC_POT = 0x20
};

enum STAT_BASE
{
    STAT_HP = 0x01,
    STAT_MP = 0x02,
    STAT_STATUS = 0x04
};

void SelectTarget (const boost::shared_ptr<OPacket> &pkt, const uint32_t entityID);

void SelectTarget (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<NPC> &target);

void SelectTarget (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<InteractiveNPC> &npc,
    const uint8_t lv);

void SelectTarget (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Player> &player);

void SelectTarget (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<TeleportBuilding> &blg);

void HPMP (const boost::shared_ptr<OPacket> &pkt, const uint32_t entityID, uint8_t op, bool hasStatus, uint8_t type,
    const uint32_t value, uint32_t EncStatus, const std::map<uint32_t,Status> &status);

void BadStatus (const boost::shared_ptr<OPacket> &pkt, const uint32_t entityID, uint32_t EncStatus,
    const std::map<uint32_t,Status> &status);

void StatusDmg (const boost::shared_ptr<OPacket> &pkt, const uint32_t entityID, uint32_t damage);

void Movement (const boost::shared_ptr<OPacket> &pkt, const uint32_t entityID, const Coord &dest);

void Rotate (const boost::shared_ptr<OPacket> &pkt, const uint32_t entityID, int16_t angle);

void State (const boost::shared_ptr<OPacket> &pkt, const uint32_t entityID, const uint8_t type, const uint8_t action);

void Speed (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<NPC> &npc);

void Position (const boost::shared_ptr<OPacket> &pkt, const uint32_t entityID, const Coord &pos, uint16_t angle);

}

#endif //__SLES_GAME_SERVER_NPC_PACKET_HPP__
