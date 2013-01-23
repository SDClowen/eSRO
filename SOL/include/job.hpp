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

#ifndef __SLES_GAMESERVER_JOB_HPP__
#define __SLES_GAMESERVER_JOB_HPP__

#include "types.hpp"
#include <string>

#define JOB_LEAGUE_ENTRY_FEED 10000
#define JOB_MINIMUM_LEVEL 20

enum JOB_TYPE
{
    JOB_NONE,
    JOB_TRADER,
    JOB_THIEF,
    JOB_HUNTER
};

class Job
{
public:

	enum
	{
		NONE,
		TRADER,
		THIEF,
		HUNTER
	};

	Job ();

	~Job ();

	void set_alias (const std::string& alias);

	void set_experience (uint64_t exp);

	void set_type (uint8_t job);

	void set_level (uint8_t lvl);

	void set_contribution (uint32_t contribution);

	std::string get_alias () const;

	uint64_t get_experience () const;

	uint8_t get_type () const;

	uint8_t get_level () const;

	uint32_t get_contribution () const;

private:

	uint8_t m_Level;
	uint8_t m_Type;
	uint64_t m_Experience;
	uint32_t m_Contribution;
	std::string m_Alias;
};

#endif //__SLES_GAMESERVER_JOB_HPP__
