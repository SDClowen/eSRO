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

#ifndef __SLES_GAME_SERVER_ATTACK_PACKETS_HPP__
#define __SLES_GAME_SERVER_ATTACK_PACKETS_HPP__

#include "packet_error_skill_cast.hpp"

#include <coord.hpp>
#include <skill_status.hpp>

#include <stdint.h>
#include <vector>
#include <boost/shared_ptr.hpp>

class OPacket;
class Player;
class NPC;

namespace srv_pkt
{

enum SKILL_ACTION_TYPE
{
    SKILL_ACTION_BUFF,
    SKILL_ACTION_ATTACK = 0x02
};

/**
 *
 *  For Knockback, the flags must be ATTACK_STATE_KNOCKBACK | ATTACK_STATE_POSITION
 *  For Knockdown, the flags must be ATTACK_STATE_POSITION
 *
 **/

enum ATTACK_STATE
{
    ATTACK_STATE_NORMAL,
    ATTACK_STATE_KNOCKBACK = 0x01,
    ATTACK_STATE_BLOCK = 0x02,
    ATTACK_STATE_POSITION = 0x04,
    ATTACK_STATE_DEAD = 0x80
};

enum DAMAGE_STATE
{
    DAMAGE_STATE_NORMAL = 0x01,
    DAMAGE_STATE_CRIT = 0x02,
    DAMAGE_STATE_STATUS = 0x10
};

struct DamageInfo
{
    uint32_t targetID;
    uint8_t AttackState;
    uint8_t DmgState;
    uint32_t damage;
    uint32_t MPAbs;     /// AMOUNT OF MP ABSORBED BY THE TARGET WHEN HITTED
    bool Wall;          /// FLAG TO KNOW IF THE DAMAGE WENT TO PLAYERS WALL
    Coord dest;

    std::vector<Status> StatusList;
};

struct CastInfo
{
    uint32_t casterID;
    uint32_t attackID;
    uint32_t skillID;
    uint32_t targetID;

    Coord dest;                         /// DESTINATION TELEPORT POSITION
    std::vector<DamageInfo> dmgInfo;
};

/**
 *
 *  @brief Display 1st animation skill sequence inflicting damage to the target.
 *
 **/

void SkillCast_1st (const boost::shared_ptr<OPacket> &pkt, const SKILL_ACTION_TYPE type, const CastInfo &info);


void SkillCast_1st (const boost::shared_ptr<OPacket> &pkt, const pkt::SKCAST_ERROR_CODE error);

/**
 *
 *  @brief Display 2nd animation skill sequence inflicting damage to the target.
 *
 **/

void SkillCast_2nd (const boost::shared_ptr<OPacket> &pkt, const CastInfo &info);

}

#endif //__SLES_GAME_SERVER_ATTACK_PACKETS_HPP__
