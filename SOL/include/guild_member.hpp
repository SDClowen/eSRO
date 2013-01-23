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

#ifndef __SRLF_GUILD_MEMBER_HPP__
#define __SRLF_GUILD_MEMBER_HPP__

#include "types.hpp"

/// GLOBAL INCLUDES
#include <string>
#include <boost/array.hpp>

///WAR,PRODUCTION AND TRAINING CAN ONLY BE ALLOCATED WHEN UR FORTRESS OWNER

#define MAX_POS_WAR_ADMIN 1
#define MAX_POS_PRODUCTION_ADMIN 1
#define MAX_POS_TRAINING_ADMIN 1
#define MAX_POS_DEPUTY_COMMANDER 4
#define MAX_POS_MILITAR_ENGINEER 10

enum GM_AUTHORITY
{
    GM_AUTHORITY_JOIN,
    GM_AUTHORITY_WITHDRAW,
    GM_AUTHORITY_NOTICE,
    GM_AUTHORITY_STORE,
    GM_AUTHORITY_UNION
};

enum GUILD_MEMBER_POSITION
{
    GUILD_MEMBER_POSITION_NO_POSITION,
    GUILD_MEMBER_POSITION_COMMANDER,
    GUILD_MEMBER_POSITION_DEPUTY_COMMANDER,
    GUILD_MEMBER_POSITION_WAR_ADMINISTRATOR = 4,
    GUILD_MEMBER_POSITION_PRODUCTION_ADMINISTRATOR = 8,
    GUILD_MEMBER_POSITION_TRAINING_ADMINISTRATOR = 16,
    GUILD_MEMBER_POSITION_MILITARY_ENGINEER = 32
};

struct GuildMember
{
    bool CanJoin () const;

    bool CanWithdraw () const;

    bool CanNotice () const;

    bool CanStore () const;

    bool CanUnion () const;

	uint32_t ID;
	uint32_t GuildID;
	std::string Name;
	std::string GrantName;
	uint8_t Level;
	uint32_t GP;
	uint32_t CombatPoints;
	uint32_t refCharID;
	bool Master;
    int16_t Zone;
	bool Status;
	boost::array<bool,5> Rights;
	uint8_t Position;
};

#endif //__SRLF_GUILD_MEMBER_HPP__
