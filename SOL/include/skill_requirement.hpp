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

#ifndef __SLES_SKILL_REQUIREMENT_HPP__
#define __SLES_SKILL_REQUIREMENT_HPP__

enum REQ_WEAPON_TYPE
{
	REQ_WEAPON_NONE = 1,
	REQ_WEAPON_CH_SWORD,
	REQ_WEAPON_CH_BLADE,
	REQ_WEAPON_CH_SPEAR,
	REQ_WEAPON_CH_GLAIVE,
	REQ_WEAPON_CH_BOW,
	REQ_WEAPON_EU_SWORD,
	REQ_WEAPON_EU_TSWORD,
	REQ_WEAPON_EU_AXE,
	REQ_WEAPON_EU_WARLOCK,
	REQ_WEAPON_EU_TSTAFF,
	REQ_WEAPON_EU_CROSSBOW,
	REQ_WEAPON_EU_DAGGER,
	REQ_WEAPON_EU_HARP,
	REQ_WEAPON_EU_CLERIC,
	REQ_WEAPON_CH_EU_FORTRESS,
	REQ_WEAPON_ANY = 255
};

enum REQ_ARMOR_TYPE
{
	REQ_ARMOR_LA,
	REQ_ARMOR_NONE
};

#endif //__SLES_SKILL_REQUIREMENT_HPP__
