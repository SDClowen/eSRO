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

#include "party.hpp"
#include "player.hpp"
#include <boost/bind.hpp>

Party::Party ()
{
	m_UniqueID = ++m_count;
}

uint32_t Party::get_id () const
{
	return m_UniqueID;
}

uint32_t Party::get_number () const
{
	return m_Entry.number();
}

void Party::set_entry (const PartyEntry &entry)
{
	m_Entry = entry;
}

PartyEntry Party::get_entry () const
{
	return m_Entry;
}

void Party::join (boost::shared_ptr<Player> player)
{
    if (player)
    {
        if (m_Members.empty())
            m_Master = player;

        if (!signal_join.empty())
            signal_join(player);

        m_Members.push_back(player);
    }
}

void Party::leave (boost::shared_ptr<Player> player)
{
    if (!player)
        return;

	for (iterator i = m_Members.begin(); i != m_Members.end(); ++i)
	{
		if ((*i)->GetUniqueID() == player->GetUniqueID())
		{
            if (!signal_leave.empty())
                signal_leave(player->GetUniqueID());

			m_Members.erase(i);
			break;
		}
	}
}

void Party::kick (const uint32_t player_id)
{
	for (iterator i = m_Members.begin(); i != m_Members.end(); ++i)
	{
		if ((*i)->GetUniqueID() == player_id)
		{
            if (!signal_kick.empty())
                signal_kick(player_id);

			m_Members.erase(i);
			break;
		}
	}

	if (m_Members.size() == 1)
	{
        if (!signal_dismiss.empty())
            signal_dismiss();

		m_Members.clear();
	}

}

void Party::transferLeadership (const uint32_t memberID)
{
    for (const_iterator i = m_Members.begin(); i != m_Members.end(); ++i)
    {
        if ((*i)->GetUniqueID() == memberID)
        {
            m_Master = *i;

            if (!signal_leader.empty())
                signal_leader(m_Master->GetUniqueID());

            break;
        }
    }
}

void Party::dismiss ()
{
	if (m_Members.size() <= 2)
	{
        if (!signal_dismiss.empty())
            signal_dismiss();

		m_Members.clear();
	}
	else
	{
        if (!signal_leave.empty())
            signal_leave(m_Master->GetUniqueID());

		m_Members.erase(m_Members.begin());

		m_Master = m_Members[0];

        if (!signal_leader.empty())
            signal_leader(m_Master->GetUniqueID());
	}
}

const boost::shared_ptr<Player> Party::get_master () const
{
	return m_Master;
}

uint8_t Party::get_member_count () const
{
	return m_Members.size();
}

std::vector<boost::shared_ptr<Player> > Party::get_member_list () const
{
    return m_Members;
}

bool Party::valid_level (const uint8_t level) const
{
	if (!m_Entry.min_level() || !m_Entry.max_level())
		return false;

	if (m_Entry.min_level() > level || m_Entry.max_level() < level)
		return false;

	return true;
}

Party::iterator Party::begin()
{
	return m_Members.begin();
}

Party::const_iterator Party::begin () const
{
	return m_Members.begin();
}

Party::iterator Party::end()
{
	return m_Members.end();
}

Party::const_iterator Party::end() const
{
	return m_Members.end();
}

uint8_t Party::IsMemberOrTransport (const uint32_t entityID) const
{
    for (const_iterator it = m_Members.begin(); it != m_Members.end(); ++it)
    {
        if ((*it)->GetUniqueID() == entityID)
            return 1;

        if ((*it)->TransportID() == entityID)
            return 2;
    }

    return 0;
}

void Party::send_msg (const std::string &name, const std::string &msg)
{
    if (!signal_msg.empty())
        signal_msg(name,msg);
}

void Party::SendMoveNotification (const uint32_t playerID, const Coord &position)
{
    if (!signal_move.empty())
        signal_move(playerID,position);
}

void Party::SendMasteryNotification (const uint32_t playerID, const uint32_t fmastID, const uint32_t smastID)
{
    if (!signal_mastery.empty())
        signal_mastery(playerID,fmastID,smastID);
}

void Party::SendGuildNotification (const uint32_t playerID, const std::string &name)
{
    if (!signal_guild.empty())
        signal_guild(playerID,name);
}

void Party::SendLevelNotification (const uint32_t playerID, const uint8_t level)
{
    if (!signal_level.empty())
        signal_level(playerID,level);
}

void Party::SendHPMPNotification (const uint32_t playerID, const uint32_t HP, const uint32_t maxHP, const uint32_t MP,
    const uint32_t maxMP)
{
    if (!signal_hp_mp.empty())
        signal_hp_mp(playerID,HP,maxHP,MP,maxMP);
}

uint32_t Party::m_count = 0;
