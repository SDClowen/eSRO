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

#ifndef __SR_GAME_SERVER_ITEM_FACTORY_HPP__
#define __SR_GAME_SERVER_ITEM_FACTORY_HPP__

#include <stdint.h>
#include <item.hpp>
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

namespace Exception
{
	class InvalidItemID {};
}

class ItemProperty;

class ItemFactory
{
public:

    typedef boost::unordered_map<uint32_t,boost::shared_ptr<ItemProperty> >::const_iterator const_iterator;

    ItemFactory (const boost::unordered_map<uint32_t,boost::shared_ptr<ItemProperty> > &property_table);

    Item Create (const uint32_t ID) const;

    Item Create (const uint32_t ID, const Item::extended_type &ext) const;

    size_t Size () const;

private:

    mutable boost::mutex m_mutex;
    boost::unordered_map<uint32_t,boost::shared_ptr<ItemProperty> > m_item_list;
};

#endif
