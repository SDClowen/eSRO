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

#ifndef __SRLF_ENTITY_STATE_HPP__
#define __SRLF_ENTITY_STATE_HPP__

/**
 *
 *  DEFINE THE AVAILABLE STATES AND ACTIONS FOR ANY ENTITY OBJECT.
 *
 **/

enum STATE_TYPE
{
    STATE_STATUS = 0,
    STATE_MOVEMENT = 1,
    STATE_ATTRIBUTE = 4,
    STATE_ATTACK = 8,
    STATE_TELEPORT = 11
};

enum STATE_STATUS_ACTION
{
	STATUS_ACTION_ALIVE = 1,
	STATUS_ACTION_DEAD = 2
};

enum STATE_MOVEMENT_ACTION
{
    MOVEMENT_ACTION_STAND = 0,
    MOVEMENT_ACTION_WALK = 2,
    MOVEMENT_ACTION_RUN = 3,
    MOVEMENT_ACTION_SIT = 4
};

enum STATE_ATTRIBUTE_ACTION
{
	ATTRIBUTE_ACTION_NORMAL,
	ATTRIBUTE_ACTION_BERSERK,
	ATTRIBUTE_ACTION_INVUNERABLE,
	ATTRIBUTE_ACTION_INVISIBLE = 7  ///NOT SURE!!
};

enum STATE_ATTACK_ACTION
{
    ATTACK_ACTION_STOP,
    ATTACK_ACTION_START,
    ATTACK_ACTION_MIDDLE
};

enum STATE_TELEPORT_ACTION
{
    TELEPORT_ACTION_STOP,
	TELEPORT_ACTION_START
};

#endif
