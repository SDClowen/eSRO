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

#ifndef __SLES_MASTERY_HPP__
#define __SLES_MASTERY_HPP__

#include "types.hpp"

enum MASTERY_TYPES
{
    MASTERY_CH_SWORD = 257,
    MASTERY_CH_SPEAR,
    MASTERY_CH_BOW,
    MASTERY_CH_COLD = 273,
    MASTERY_CH_FIRE,
    MASTERY_CH_LIGHT,
    MASTERY_CH_WATER,

    MASTERY_EU_WARRIOR = 513,
    MASTERY_EU_WIZARD,
    MASTERY_EU_ROGUE,
    MASTERY_EU_WARLOCK,
    MASTERY_EU_BARD,
    MASTERY_EU_CLERIC
};

class Mastery
{
public:

    Mastery(uint32_t id = 0,uint8_t lv = 0);

    uint32_t ID () const;

	void Level (const uint8_t lvl);

	uint8_t Level () const;

	bool operator < (const Mastery& mastery) const;

	Mastery& operator++ ();

	Mastery operator++ (int);

	Mastery& operator-- ();

	Mastery operator-- (int);

private:

	uint32_t m_ID;
	uint8_t m_Level;
};

#endif //__SLES_MASTERY_HPP__
