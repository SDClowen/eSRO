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

#ifndef __SLES_STORAGE_HPP__
#define __SLES_STORAGE_HPP__

#include "item.hpp"
#include "storage_constants.hpp"

#include <map>
#include <boost/signals2.hpp>

namespace Exception
{
class Full
{
};

class OutRange
{
};

class Invalid
{
};

}

class Storage
{
public:

    boost::signals2::signal<void (const uint32_t)> signal_remove;

    boost::signals2::signal<uint32_t (const Item&, const STORAGE_TYPE)> signal_insert;

    boost::signals2::signal<void (const uint32_t, const uint8_t)> signal_position;

    boost::signals2::signal<void (uint32_t, uint16_t)> signal_quantity;

public:

	typedef std::map<uint8_t,Item>::iterator iterator;
	typedef std::map<uint8_t,Item>::const_iterator const_iterator;

	Storage (uint8_t capacity, const STORAGE_TYPE type);

	Storage (uint8_t capacity, const STORAGE_TYPE type, const std::map<uint8_t,Item>& list);

	void SetCapacity (uint8_t capacity);

	uint8_t GetCapacity () const;

	void MaxSlotsInc (const uint8_t slots);

	Item GetItem(uint8_t position) const;

	void MoveItem (const uint8_t source, const uint8_t destination, const uint16_t quantity);

	void RemoveItem (uint8_t position);

	void InsertItem (const Item& item);

	uint8_t get_empty_position (const uint8_t start_pos) const;

	iterator begin ();

	iterator end ();

	const_iterator begin () const;

	const_iterator end () const;

	iterator find (const uint8_t pos);

	const_iterator find (const uint8_t pos) const;

	bool empty() const;

	size_t size() const;

	void clear();

protected:

    uint8_t m_Capacity;

    STORAGE_TYPE m_Type;
	std::map<uint8_t,Item> m_Slots;
};

#endif //__SLES_STORAGE_HPP__
