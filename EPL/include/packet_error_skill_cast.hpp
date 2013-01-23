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

#ifndef PACKET_ERROR_SKILL_CAST_HPP
#define PACKET_ERROR_SKILL_CAST_HPP

namespace pkt
{

enum SKCAST_ERROR_CODE
{
    ERROR_SKCAST_INVALID_SKILL = 0x03,
    ERROR_SKCAST_INVALID_MP = 0x04,
    ERROR_SKCAST_INVALID_TARGET = 0x06,
    ERROR_SKCAST_INVALID_EQUIPMENT = 0x0D,
    ERROR_SKCAST_INVALID_AMMO = 0x0E,
    ERROR_SKCAST_WEAPON_BROKEN = 0x0F,
    ERROR_SKCAST_OBSTACLE = 0x10,
    ERROR_SKCAST_INVALID_HP = 0x13,
    ERROR_SKCAST_PK_INVALID_LEVEL = 0x16,
    ERROR_SKCAST_SAFE_ZONE = 0x18,
    ERROR_SKCAST_TRANSPORT = 0x21,
    ERROR_SKCAST_PARTY_MEMBER = 0x22,
    ERROR_SKCAST_PARTY_TRANSPORT = 0x23,
    ERROR_SKCAST_ITEM_COOLDOWN = 0x30
};

/*
02 07 - Cannot use the skill on this target
02 08 - Cannot use because your level is low
02 11 - The target is not selected
02 12 - Cannot resurrect the other player because his level is higher than you
02 14 - Cannot attack first because you have exceeded the daily cumulative PK(15Point)limit
02 15 - Cannot attack the opponement due to high murderer reputation
02 17 - Cannot attack because your level is too low
02 19 - Cannot attack for 10 seconds after wearing a job equipment
02 20 - Cannot attack or be attacked for 5 seconds after untouchable status
02 24 - In the case that the trade level is just one star, you are not allowed to attack traders nor transport
02 25 - PK is disallowed on the current server
02 28 - This skill cannot be used in battle
02 29 - The available number of people for this skill is maxed
02 30 - Selected skill cannot be used under the transformation state
02 31 - The selected skill cannot be used under berserk state
02 32 - Can only be used within music play area
02 33 - The skill can be used on general monsters
02 34 - Can only be used under stealth state
02 35 - Skill cannot be used because the targets level is to high
02 36 - Can only be used when HP is below 30%
02 37 - Cannot set a trap because a set trap already exists
02 38 - Cannot set a trap because a monster exists nearby
02 39 - Cannot be used when you or the opponement is under job or free battle state
02 40 - It is not allowed to attack the heart of the fortress since all of the guard towers forming the seal has not been destroyed
02 41 - It is not allowed to attack the fortress structures in any period other than the fortress war
02 42 - Invalid target
02 43 - It is not allowed to attack the fortress structures in any period other than the fortress war
02 44 - The behavior is not allowed
02 45 - The behavior is not allowed
02 46 - It is not allowed to attack the heart of the fortress, since the seal of the heart is now being released
02 47 - The behavior is not allowed
02 0C - The selected item cannot be used together or the skill is already in use
02 3B - It is not allowed to attack other member of the same guild during the fortress war
02 3C - It is not allowed to attack the allied guild during the fortress war
02 3D - It is allowed to attack with the seige weapon only
02 3E - It is allowed to attack the defending guild only
02 3F - It is allowed to attack the seige guild only
*/

}

#endif // PACKET_ERROR_SKILL_CAST_HPP
