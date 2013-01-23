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

#ifndef __SLES_TYPES_HPP__
#define __SLES_TYPES_HPP__

#include <stdint.h>

enum GENDER
{
    GENDER_FEMALE,
    GENDER_MALE,
	GENDER_ANY
};

enum RACE
{
	RACE_CH= 0,
	RACE_EU = 1,
	RACE_ANY = 3,
	RACE_NONE
};

const int MAX_CAP = 100;

#endif //__SLES_TYPES_HPP__
