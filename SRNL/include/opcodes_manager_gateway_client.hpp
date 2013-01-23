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

#ifndef __SRNL_OPCODES_GATEWAY_MANAGER_HPP__
#define __SRNL_OPCODES_GATEWAY_MANAGER_HPP__

/**
 *
 *  GATEWAY CLIENT OPCODES
 *
 **/

enum
{
	GW_CL_CONFIG = 0xA001,
	GW_CL_LOGIN_QUEUE
};

/**
 *
 *  SR_GAMESERVER CLIENT OPCODES
 *
 **/

enum
{
    GM_CL_CONFIG = 0xA001,
    GM_CL_ACCOUNT_NOTIFY,
    GM_CL_SHARD_NOTIFY
};

/**
 *
 *  SHARD SERVER OPCODES
 *
 **/

enum
{
    GM_SR_BASE = 0xB001,
    GM_SR_CONFIG,
    GM_SR_LOGIN_QUEUE
};

#endif
