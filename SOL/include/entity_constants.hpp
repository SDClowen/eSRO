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

#ifndef __SRLF_ENTITY_CONSTANTS_HPP__
#define __SRLF_ENTITY_CONSTANTS_HPP__

enum ENTITY_TYPE
{
    ET_UNKNOWN,
    ET_PLAYER,
    ET_MOB,
    ET_MOB_QUEST,
    ET_MOB_HUNTER,
    ET_MOB_THIEF,
    ET_MOB_EVENT,
    ET_INTERACTIVE,
    ET_COS_VEHICLE,
    ET_COS_TRANSPORT,
    ET_COS_GROWTH,
    ET_COS_ABILITY,
    ET_COS_MERC,
    ET_COS_MOB
};

#endif
