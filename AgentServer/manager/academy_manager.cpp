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

#include "academy_manager.hpp"
#include <cassert>

AcademyManager::AcademyManager ()
	: m_count(0)
{
}

bool AcademyManager::Initialize ()
{
    return true;
}

void AcademyManager::Release ()
{
}

uint32_t AcademyManager::get_next_number ()
{
    return ++m_count;
}

void AcademyManager::insert (const Academy &academy)
{
	assert(academy.get_number());

	m_list.insert(std::make_pair(academy.get_number(),academy));
}

void AcademyManager::remove (const uint32_t id)
{
	m_list.erase(id);
}


uint8_t AcademyManager::max_pages () const
{
	return (m_list.size() / MAX_ENTRY_PER_PAGE)+1;
}

std::vector<Academy> AcademyManager::get_list (const uint8_t page) const
{
	assert(page <= max_pages());

	std::vector<Academy> list;

	std::map<uint32_t,Academy>::const_iterator iter;

	iter = m_list.begin();

	std::advance(iter,page*MAX_ENTRY_PER_PAGE);

	for (; list.size() < MAX_ENTRY_PER_PAGE && iter != m_list.end(); ++iter)
			list.push_back(iter->second);

	return list;
}
