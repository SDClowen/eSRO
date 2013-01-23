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

#ifndef GUILD_NOTIFY_HPP
#define GUILD_NOTIFY_HPP

enum GUILD_NOTIFY_TYPE
{
    GUILD_NOTIFY_DISBAND = 0x01,
    GUILD_NOTIFY_MEMBER_JOIN = 0x02,
    GUILD_NOTIFY_MEMBER_LEAVE = 0x03,
    GUILD_NOTIFY_GUILD = 0x05,
    GUILD_NOTIFY_MEMBER = 0x06,
    GUILD_NOTIFY_UNION_MEMBER = 0x0E,
    GUILD_NOTIFY_UNION_MEMBER_LEAVE = 0x12,
    GUILD_NOTIFY_MEMBER_AUTHORITY = 0x16,
    GUILD_NOTIFY_WAR_END_TIME_A = 0x1A,
    GUILD_NOTIFY_WAR_END_TIME_B = 0x1B,
    GUILD_NOTIFY_WAR_OUTCOME = 0x1C,
    GUILD_NOTIFY_WAR_CASUALTY = 0x1D,
    GUILD_NOTIFY_WAR_REMAINING_TIME = 0x1F
};

enum GUILD_NOTIFY_GUILD_TYPE
{
    GUILD_NOTIFY_GUILD_GP = 0x08,
    GUILD_NOTIFY_GUILD_PROMOTE = 0x0C,
    GUILD_NOTIFY_GUILD_NOTICE = 0x10
};

enum GUILD_NOTIFY_MEMBER_TYPE
{
    GUILD_NOTIFY_MEMBER_LEVEL = 0x01,
    GUILD_NOTIFY_MEMBER_STATE = 0x02,
    GUILD_NOTIFY_MEMBER_GP = 0x08,
    GUILD_NOTIFY_MEMBER_POSITION = 0x40,
    GUILD_NOTIFY_MEMBER_ZONE = 0x80
};

enum GUILD_NOTIFY_UNION_TYPE
{
    GUILD_NOTIFY_UNION_JOIN = 0x01,
    GUILD_NOTIFY_UNION_MEMBER_MAX = 0x02,
    GUILD_NOTIFY_UNION_MEMBER_LEADER = 0x04,
    GUILD_NOTIFY_UNION_MEMBER_COUNT = 0x08
};

enum GUILD_NOTIFY_UNION_MEMBER_LEAVE_TYPE
{
    GUILD_NOTIFY_UNION_MEMBER_REASON_LEAVE = 1,
    GUILD_NOTIFY_UNION_MEMBER_REASON_KICK,
    GUILD_NOTIFY_UNION_MEMBER_REASON_DISBAND
};

enum GUILD_NOTIFY_MEMBER_AUTHORITY_TYPE
{
    GUILD_NOTIFY_MEMBER_AUTHORITY_SETTING = 0x10,
    GUILD_NOTIFY_MEMBER_AUTHORITY_LEADER_TRANSFER = 0x54
};

enum GUILD_MEMBER_STATE
{
    GUILD_MEMBER_LOGIN,
    GUILD_MEMBER_LOGOFF
};

enum GUILD_WAR_CASUALTY_TYPE
{
    GUILD_WAR_CASUALTY_DEATH = 0x01,
    GUILD_WAR_CASUALTY_KILL = 0x02
};

#endif // GUILD_NOTIFY_HPP
