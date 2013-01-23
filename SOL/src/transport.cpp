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

#include "transport.hpp"
#include "storage.hpp"
#include "entity_property.hpp"

Transport::Transport (const boost::shared_ptr<EntityProperty> &property)
    : NPC(property), m_Mounted(true), m_Storage(new Storage(0,STORAGE_TRANSPORT)), m_TradeScale(0), m_SpawnTaskID(0)
{
    m_HP = m_property->HP;
	m_MP = 0;

	m_MaxHP = m_HP;
	m_MaxMP = 0;
}

void Transport::SetOwner (const boost::shared_ptr<Player> &owner)
{
    m_Owner = owner;

    if (m_Owner)
        m_Storage->SetCapacity(0x41);
    else
        m_Storage->SetCapacity(0);
}

const boost::shared_ptr<Player>& Transport::GetOwner () const
{
    return m_Owner;
}

const boost::shared_ptr<Storage>& Transport::GetStorage () const
{
    return m_Storage;
}

bool Transport::has_goods () const
{
    return m_Storage->size();
}

bool Transport::IsMounted () const
{
    return m_Mounted;
}

void Transport::Mount ()
{
    m_Mounted = true;
}

void Transport::Dismount ()
{
    m_Mounted = false;
}

uint32_t Transport::SpawnTaskID () const
{
    return m_SpawnTaskID;
}

void Transport::SpawnTaskID (const uint32_t taskID)
{
    m_SpawnTaskID = taskID;
}

uint8_t Transport::TradeScale() const
{
    return m_TradeScale;
}

void Transport::CalculateTradeScale()
{
    uint16_t max_goods = 0;

    for (Storage::const_iterator it = m_Storage->begin(); it != m_Storage->end(); ++it)
        max_goods += it->second.get_quantity();

    m_TradeScale = 0;

    if (max_goods)
    {
        if (max_goods > 0 && max_goods <= 719)
        {
            m_TradeScale = 1;
        }
        else if (max_goods > 719 && max_goods <= 1293)
        {
            m_TradeScale = 2;
        }
        else if (max_goods > 1293 && max_goods <= 2012)
        {
            m_TradeScale = 3;
        }
        else if (max_goods > 2012 && max_goods <= 2600)
        {
            m_TradeScale = 4;
        }
        else
        {
            m_TradeScale = 5;
        }
    }
}

uint8_t Transport::level () const
{
    return m_property->Level;
}

void Transport::calculate_stats ()
{
}
