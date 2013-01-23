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

#include "party_manager.hpp"

PartyManager::PartyManager ()
	: m_count(0)
{
}

uint32_t PartyManager::get_next_number ()
{
    boost::mutex::scoped_lock lock(m_mutex);
    return ++m_count;
}

void PartyManager::insert_party (const boost::shared_ptr<Party> party)
{
    if (party && party->get_number())
    {
        boost::mutex::scoped_lock lock(m_mutex);
        m_party_list.insert(std::make_pair(party->get_number(),party));
    }
}

void PartyManager::remove_party (const uint32_t party_id)
{
    boost::mutex::scoped_lock lock(m_mutex);
	m_party_list.erase(party_id);
}

boost::shared_ptr<Party> PartyManager::find (const uint32_t partyID) const
{
    boost::mutex::scoped_lock lock(m_mutex);

    boost::shared_ptr<Party> party;

    const_iterator iter = m_party_list.find(partyID);

    if (iter != m_party_list.end())
        party = iter->second;

    return party;
}

uint8_t PartyManager::max_pages () const
{
    boost::mutex::scoped_lock lock(m_mutex);
	return (m_party_list.size() / MAX_PARTY_PER_PAGE)+1;
}

std::vector<boost::shared_ptr<Party> >
PartyManager::get_party_list (const uint8_t page, const uint32_t party_id) const
{
	std::vector<boost::shared_ptr<Party> > list;

    if (page <= max_pages())
    {
        boost::mutex::scoped_lock lock(m_mutex);
        const_iterator iter;

        if (party_id)
        {
            iter = m_party_list.find(party_id);

            if (iter != m_party_list.end())
                list.push_back(iter->second);
        }

        iter = m_party_list.begin();

        std::advance(iter,page*MAX_PARTY_PER_PAGE);

        for (; list.size() < MAX_PARTY_PER_PAGE && iter != m_party_list.end(); ++iter)
        {
            if (iter->second->get_number() != party_id)
                list.push_back(iter->second);
        }
    }

	return list;
}
