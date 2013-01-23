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

#ifndef __SLES_GAME_SERVER_PARTY_MANAGER_HPP__
#define __SLES_GAME_SERVER_PARTY_MANAGER_HPP__

#include <party.hpp>

#include <vector>
#include <map>
#include <boost/thread/mutex.hpp>

class PartyManager
{
public:

	enum { MAX_PARTY_PER_PAGE = 15 };

	typedef std::map<uint32_t,boost::shared_ptr<Party> >::iterator iterator;
	typedef std::map<uint32_t,boost::shared_ptr<Party> >::const_iterator const_iterator;

	PartyManager();

	void insert_party (const boost::shared_ptr<Party> party);

	void remove_party (const uint32_t party_id);

	boost::shared_ptr<Party> find (const uint32_t party_id) const;

	uint32_t get_next_number ();

	uint8_t max_pages () const;

	std::vector<boost::shared_ptr<Party> > get_party_list (const uint8_t page, const uint32_t party_id) const;

private:

	uint32_t m_count;
    mutable boost::mutex m_mutex;
	std::map<uint32_t,boost::shared_ptr<Party> > m_party_list;
};

#endif //__SLES_GAME_SERVER_PARTY_MANAGER_HPP__
