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

#include "stall.hpp"

Stall::Stall (const std::string& name, const uint32_t owner)
	: Storage(10,STORAGE_STALL), m_Name(name),m_Owner(owner),m_State(false)
{
}

uint32_t Stall::get_owner () const
{
	return m_Owner;
}

void Stall::set_name (const std::string& name)
{
	m_Name = name;
}

const std::string& Stall::get_name () const
{
	return m_Name;
}

void Stall::set_greeting (const std::string& greeting)
{
	m_Greeting = greeting;

    if (!signal_greeting.empty())
        signal_greeting(m_Greeting);
}

const std::string& Stall::get_greeting () const
{
	return m_Greeting;
}

void Stall::change_state ()
{
	m_State = !m_State;

    if (!signal_state.empty())
        signal_state(m_State);
}

bool Stall::is_open () const
{
	return m_State;
}

void Stall::enter (const uint32_t player_id, const std::string &name)
{
    if (!signal_enter.empty())
        signal_enter(player_id,name);

    boost::mutex::scoped_lock lock(m_mutex);
    m_IDs.push_back(player_id);
}

void Stall::leave (const uint32_t player_id)
{
    if (!signal_leave.empty())
        signal_leave(player_id);

    boost::mutex::scoped_lock lock(m_mutex);
    m_IDs.erase(std::remove(m_IDs.begin(),m_IDs.end(),player_id));
}

void Stall::close ()
{
    if (!signal_close.empty())
        signal_close();
}

void Stall::send_msg (const std::string &name, const std::string &msg)
{
    if (!signal_msg.empty())
        signal_msg(name,msg);
}

void Stall::buy (const std::string &name,const uint8_t item_position)
{
	RemoveItem(item_position);

    if (!signal_buy.empty())
        signal_buy(item_position,name);

	if (empty())
		close();
}

void Stall::change_item (const uint8_t pos, const uint16_t quantity, const uint32_t price)
{
	iterator i = m_Slots.find(pos);

	if (i != m_Slots.end())
	{
		i->second.set_stall_quantity(quantity);

		i->second.set_stall_price(price);

        if (!signal_price.empty())
            signal_price(pos,quantity,price);
	}
}

std::vector<uint32_t> Stall::client_list () const
{
    boost::mutex::scoped_lock lock(m_mutex);
    return m_IDs;
}
