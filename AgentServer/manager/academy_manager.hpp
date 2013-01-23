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

#ifndef __SLES_GAME_SERVER_ACADEMY_MANAGER_HPP__
#define __SLES_GAME_SERVER_ACADEMY_MANAGER_HPP__

#include <academy.hpp>
#include <vector>
#include <map>

class AcademyManager
{
public:

	enum { MAX_ENTRY_PER_PAGE = 15 };

	AcademyManager ();

	bool Initialize ();

	void Release ();

	uint8_t max_pages () const;

	uint32_t get_next_number ();

	std::vector<Academy> get_list (const uint8_t page) const;

	void remove (const uint32_t ID);

	void insert (const Academy &academy);

private:

	uint32_t m_count;
	std::map<uint32_t,Academy> m_list;
};

#endif //__SLES_GAME_SERVER_ACADEMY_MANAGER_HPP__
