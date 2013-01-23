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

#ifndef __SRNL_OPCODES_MANAGER_GATEWAY_SERVER_HPP__
#define __SRNL_OPCODES_MANAGER_GATEWAY_SERVER_HPP__

enum
{
	SERVER_BASE = 0xB001,
	SERVER_CONFIG,
	SERVER_ACCOUNT_PROCESS,
	SERVER_NEWS_INSERT,
	SERVER_NEWS_REMOVE,
	SERVER_SHARD_INSERT,
	SERVER_SHARD_REMOVE,
	SERVER_SHARD_NOTIFY,
	SERVER_PATCH_INSERT,
	SERVER_PATCH_REMOVE,
	SERVER_PATCH_NOTIFY
};

#endif
