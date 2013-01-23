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

#ifndef __SLES_GAME_SERVER_PARTY_HPP__
#define __SLES_GAME_SERVER_PARTY_HPP__

#include <types.hpp>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

#include "party_entry.hpp"
#include "coord.hpp"

class Player;

enum PARTY_PURPOSE
{
    PARTY_PURPOSE_NORMAL,
    PARTY_PURPOSE_QUEST,
    PARTY_PURPOSE_TRADE,
    PARTY_PURPOSE_THIEF
};

#define PARTY_TYPE_AUTOSHARE_EXP 0x01
#define PARTY_TYPE_AUTOSHARE_ITEM 0x02
#define PARTY_TYPE_JOIN_RIGHT 0x04

class Party
{
public:

    boost::signals2::signal<void (boost::shared_ptr<Player>)> signal_join;

    boost::signals2::signal<void (uint32_t)> signal_leave;

    boost::signals2::signal<void (uint32_t)> signal_kick;

    boost::signals2::signal<void ()> signal_dismiss;

    boost::signals2::signal<void (uint32_t)> signal_leader;

    boost::signals2::signal<void (const std::string&, const std::string&)> signal_msg;

    boost::signals2::signal<void (uint32_t,Coord)> signal_move;

    boost::signals2::signal<void (uint32_t,uint32_t,uint32_t)> signal_mastery;

    boost::signals2::signal<void (uint32_t,const std::string&)> signal_guild;

    boost::signals2::signal<void (uint32_t,uint8_t)> signal_level;

    boost::signals2::signal<void (uint32_t,uint32_t,uint32_t,uint32_t,uint32_t)> signal_hp_mp;

public:

	typedef std::vector<boost::shared_ptr<Player> >::iterator iterator;
	typedef std::vector<boost::shared_ptr<Player> >::const_iterator const_iterator;

	Party ();

	uint32_t get_id () const;

	uint32_t get_number () const;

	void set_entry (const PartyEntry &entry);

	PartyEntry get_entry () const;

	void join (boost::shared_ptr<Player> new_player);

	void leave (boost::shared_ptr<Player> player);

	void kick (const uint32_t player_id);

    void transferLeadership (const uint32_t memberID);

	void dismiss ();

	const boost::shared_ptr<Player> get_master () const;

	/**
	 *
	 *	brief Obtain the current number of members in the party.
	 *
	 **/

	uint8_t get_member_count () const;

    std::vector<boost::shared_ptr<Player> > get_member_list () const;

	bool valid_level (const uint8_t level) const;

	iterator begin();

	const_iterator begin() const;

	iterator end();

	const_iterator end() const;

	void send_msg (const std::string &name, const std::string &msg);

    void SendMoveNotification (const uint32_t playerID, const Coord &position);

    void SendMasteryNotification (const uint32_t playerID, const uint32_t fmastID, const uint32_t smastID);

    void SendGuildNotification (const uint32_t playerID, const std::string &name);

    void SendLevelNotification (const uint32_t playerID, const uint8_t level);

    void SendHPMPNotification (const uint32_t playerID, const uint32_t HP, const uint32_t maxHP, const uint32_t MP,
        const uint32_t maxMP);

    uint8_t IsMemberOrTransport (const uint32_t entityID) const;

private:

	uint8_t m_Type;
	uint32_t m_UniqueID;
	PartyEntry m_Entry;

	boost::shared_ptr<Player> m_Master;

	std::vector<boost::shared_ptr<Player> > m_Members;

    static uint32_t m_count;
};

#endif //__SLES_GAME_SERVER_PARTY_HPP__
