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

#ifndef __SLES_GAMESERVER_STALL_HPP__
#define __SLES_GAMESERVER_STALL_HPP__

#include "storage.hpp"
#include <string>
#include <boost/thread/mutex.hpp>

class Stall : public Storage
{
public:

    boost::signals2::signal<void (const std::string&)> signal_greeting;

    boost::signals2::signal<void (bool)> signal_state;

    boost::signals2::signal<void (uint8_t,uint16_t,uint32_t)> signal_price;

    boost::signals2::signal<void (const uint8_t pos,const std::string&)> signal_buy;

    boost::signals2::signal<void (uint32_t,const std::string&)> signal_enter;

    boost::signals2::signal<void (uint32_t)> signal_leave;

    boost::signals2::signal<void ()> signal_close;

    boost::signals2::signal<void (const std::string&,const std::string&)> signal_msg;

public:

	Stall (const std::string& name, const uint32_t owner);

	uint32_t get_owner () const;

	void set_name (const std::string& name);

	const std::string& get_name () const;

	void set_greeting (const std::string& greeting);

	const std::string& get_greeting () const;

	void change_state ();

	bool is_open () const;

	void enter (const uint32_t player_id, const std::string &name);

	void leave (const uint32_t player_id);

	void close ();

	void send_msg (const std::string &name, const std::string &msg);

	void buy (const std::string &name,const uint8_t item_position);

	void change_item (const uint8_t pos, const uint16_t quantity, const uint32_t price);

    std::vector<uint32_t> client_list () const;

private:

	std::string m_Name;	//! Stall Name.
	std::string m_Greeting;	//! Stall Greetings.
	uint32_t m_Owner;
	bool m_State;

    mutable boost::mutex m_mutex;
    std::vector<uint32_t> m_IDs;    //! Players inside the stall
};

#endif //__SLES_GAMESERVER_STALL_HPP__
