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

#include "item_factory.hpp"

ItemFactory::ItemFactory (const boost::unordered_map<uint32_t,boost::shared_ptr<ItemProperty> > &property_table)
    : m_item_list(property_table)
{
}

Item ItemFactory::Create (const uint32_t ID) const
{
    boost::mutex::scoped_lock lock(m_mutex);

    const_iterator i = m_item_list.find(ID);

    if (i == m_item_list.end())
        throw Exception::InvalidItemID();

    return Item(i->second);
}

Item ItemFactory::Create (const uint32_t ID, const Item::extended_type &ext) const
{
    boost::mutex::scoped_lock lock(m_mutex);

    const_iterator i = m_item_list.find(ID);

    if (i == m_item_list.end())
        throw Exception::InvalidItemID();

    return Item(i->second,ext);
}

size_t ItemFactory::Size() const
{
    boost::mutex::scoped_lock lock(m_mutex);

    return m_item_list.size();
}
