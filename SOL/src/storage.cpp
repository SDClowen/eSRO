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

#include "storage.hpp"

Storage::Storage(uint8_t capacity, const STORAGE_TYPE type)
	: m_Capacity(capacity),m_Type(type)
{
}

Storage::Storage(uint8_t capacity, const STORAGE_TYPE type, const std::map<uint8_t,Item>& list)
	: m_Capacity(capacity), m_Type(type), m_Slots(list)
{
}

void Storage::SetCapacity (uint8_t capacity)
{
	m_Capacity = capacity;
}

uint8_t Storage::GetCapacity () const
{
	return m_Capacity;
}

void Storage::MaxSlotsInc (const uint8_t slots)
{
    m_Capacity += slots;
}

Item Storage::GetItem (uint8_t position) const
{
	const_iterator iter = m_Slots.find(position);

	if (iter == m_Slots.end())
		return Item();

	return iter->second;
}

void Storage::MoveItem (const uint8_t src, const uint8_t dst, const uint16_t quantity)
{
    Item isrc = m_Slots[src];
    Item idest = m_Slots[dst];

    if (quantity && isrc.get_max_stack() > 1)
    {
        if (isrc.get_quantity() < quantity)
            return;

        /// UNSTACK PART OF ITEM AMOUNT TO AN EMPTY POSITION
        if (!idest.get_id() && isrc.get_quantity() > quantity)
        {
            idest = isrc;
            idest.set_position(dst);
            idest.set_quantity(quantity);

            isrc.set_quantity(isrc.get_quantity() - quantity);

            if (!signal_insert.empty())
                idest.ID(*(signal_insert(idest,m_Type)));

            if (!signal_quantity.empty())
                signal_quantity(isrc.ID(),isrc.get_quantity());

            m_Slots[dst] = idest;
            m_Slots[src] = isrc;

            return;
        }

        /// STACK OR UNSTACK ONLY IF THEY ARE THE SAME ITEM
        if (idest.get_id() == isrc.get_id())
        {
            /// CHECK IF THE QUANTITY TO MOVE ISNT GREATER THAN MAX STACKABLE
            if (idest.get_max_stack() - idest.get_quantity() < quantity)
                return;

            uint16_t amount = isrc.get_quantity() - quantity;

            isrc.set_quantity(idest.get_quantity()+quantity);
            idest.set_quantity(amount);

            if (!signal_quantity.empty())
            {
                signal_quantity(isrc.ID(),isrc.get_quantity());
                signal_quantity(idest.ID(),amount);
            }

            return;
        }
    }

    m_Slots.erase(src);

    if (idest.get_id() && idest.get_quantity())
    {
        idest.set_position(src);
        idest.set_place(m_Type);

        m_Slots[src] = idest;

        if (!signal_position.empty())
            signal_position(idest.ID(),src);
    }

    if (isrc.get_quantity())
    {
        isrc.set_position(dst);

        m_Slots[dst] = isrc;

        if (!signal_position.empty())
            signal_position(isrc.ID(),dst);
    }
}

void Storage::RemoveItem (uint8_t position)
{
    iterator iter = m_Slots.find(position);

    if (iter != m_Slots.end())
    {
        if (!signal_remove.empty())
            signal_remove(iter->second.ID());

        m_Slots.erase(iter);
    }
}

void Storage::InsertItem (const Item& item)
{
    if (!item.get_id())
		throw Exception::Invalid();

	if (item.get_position() > m_Capacity)
		throw Exception::OutRange();

	std::pair<iterator,bool> ret = m_Slots.insert(std::make_pair(item.get_position(),item));

    if (!signal_insert.empty())
        ret.first->second.ID(*(signal_insert(item,m_Type)));
}

uint8_t Storage::get_empty_position (const uint8_t start_pos) const
{
    uint8_t pos = start_pos;
	int empty_pos = -1;

	for ( const_iterator i = m_Slots.begin(); i != m_Slots.end(); ++i )
	{
		if (i->first < start_pos)
			continue;

		if ( i->first == pos )
			++pos;
		else if ( empty_pos == -1)
			empty_pos = pos;
	}

	if ( empty_pos == -1 && pos < m_Capacity)
		empty_pos = pos;

	return empty_pos;
}

Storage::iterator Storage::begin ()
{
	return m_Slots.begin();
}

Storage::iterator Storage::end ()
{
	return m_Slots.end();
}

Storage::const_iterator Storage::begin () const
{
	return m_Slots.begin();
}

Storage::const_iterator Storage::end () const
{
	return m_Slots.end();
}

Storage::iterator Storage::find (const uint8_t pos)
{
    return m_Slots.find(pos);
}

Storage::const_iterator Storage::find (const uint8_t pos) const
{
    return m_Slots.find(pos);
}

bool Storage::empty () const
{
	return m_Slots.empty();
}

size_t Storage::size () const
{
	return m_Slots.size();
}

void Storage::clear ()
{
	m_Slots.clear();
}
