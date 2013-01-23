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

#ifndef __SLES_SKILL_IMBUE_HPP__
#define __SLES_SKILL_IMBUE_HPP__

#include "skill_attack.hpp"
#include "skill_status.hpp"
#include <vector>

struct Imbue
{
	Imbue ()
        : skillID(0), castID(0),duration(0)
	{}

    uint32_t skillID;
    uint32_t castID;
    uint32_t taskID;
	int duration;
	AttackInfo attack;
	AttackArea area;
	std::vector<Status> effect;
};

#endif
