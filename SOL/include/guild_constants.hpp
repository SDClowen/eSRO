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

#ifndef __SRLF_GUILD_CONSTANTS_HPP__
#define __SRLF_GUILD_CONSTANTS_HPP__

#define GUILD_MAX_LEVEL 5
#define GUILD_STORAGE_MIN_ACCESS_LEVEL 2
#define GUILD_SHOP_MIN_ACCESS_LEVEL 3

enum GUILD_MEMBER_LEAVE_REASON
{
	GUILD_MEMBER_LEFT = 1,
	GUILD_MEMBER_BANNED = 2
};

enum GUILD_MEMBER_RIGHTS
{
    JOIN_RIGHTS,
    WITHDRAW_RIGHTS,
    UNION_RIGHTS,
    STORAGE_RIGHTS,
    NOTICE_RIGHTS
};

#endif
