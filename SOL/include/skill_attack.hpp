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

#ifndef __SLES_SKILL_ATTACK_HPP__
#define __SLES_SKILL_ATTACK_HPP__

enum ATTACK_TYPE
{
	ATTACK_NONE,
	ATTACK_PHYSICAL_CLOSED = 5,
	ATTACK_PHYSICAL_RANGED,
	ATTACK_MAGICAL_CLOSED = 8,
    ATTACK_MAGICAL_RANGED = 9,
	ATTACK_MAGICAL = 10
};

struct AttackInfo
{
	AttackInfo ()
		: min_pwr(0), max_pwr(0), percent(0), type(ATTACK_NONE)
	{}

	int min_pwr;
	int max_pwr;
	int percent;
	ATTACK_TYPE type;
};

enum AREA_REFERENCE
{
	REF_PLAYER_POSITION = 1,
	REF_PLAYER_FRONT = 2,
	REF_TARGET_PIERCE = 3,
	REF_TARGET_DIRECTION = 4,
	REF_TARGET_TRANSFER = 6,
	REF_NONE
};

struct AttackArea
{
	AttackArea ()
		: reference(REF_NONE), distance(0),max_targets(0),damage_reduce(0)
	{}

	AREA_REFERENCE reference;
	int distance;
	int max_targets;
	int damage_reduce;
};

#endif
