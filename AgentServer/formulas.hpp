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

#ifndef FORMULAS_HPP
#define FORMULAS_HPP

#include <coord.hpp>

#include <stdint.h>
#include <boost/shared_ptr.hpp>

class NPC;
class Skill;

uint64_t ExpGained (uint32_t baseXP, uint8_t playerMaxLv, uint8_t gap, uint8_t mobLv);

float SkillRange (const boost::shared_ptr<NPC> &caster, const boost::shared_ptr<Skill> &skill, uint32_t targetID);

Coord RangePosition (const Coord &src, const Coord &dest, const float range, uint16_t *dir);

Coord TeleportPosition (const Coord &src, const Coord &dest, const float range, uint16_t *dir);

Coord KnockbackPosition (const Coord &src, const Coord &dest, uint16_t distance);

#endif // FORMULAS_HPP
