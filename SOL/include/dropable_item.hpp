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

#ifndef __SLES_DROPABLEDITEM_HPP__
#define __SLES_DROPABLEDITEM_HPP__

#include "item.hpp"
#include "spawnobject.hpp"

class DropableItem : public SpawnObject
{
public:

	DropableItem()
        : m_OwnerID(0), m_OwnerAuth(-1), m_GoldAmount(0)
	{}

    uint32_t m_OwnerID;
    std::string m_OwnerAlias;
    uint32_t m_OwnerAuth;
    uint64_t m_GoldAmount;

	Item m_Item;
};

#endif //__SLES_DROPABLEITEM_HPP__
