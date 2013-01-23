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

#include "guild.hpp"
#include "guild_member.hpp"
#include "storage.hpp"

#include <boost/scoped_array.hpp>

Guild::Guild ()
    : m_GuildStorage(new Storage(0,STORAGE_GUILD))
{
    m_ID = 0;
    m_Level = 0;
    m_GP = 0;
    m_UnionID = 0;
    m_EmblemIndex = 0;

    m_WarFlag = false;

    m_StorageInUse = false;
    m_StoredGold = 0;
}

Guild::Guild (const uint32_t ID, const std::string &name, const uint8_t level, const uint8_t StorageSize)
    : m_ID(ID), m_Name(name), m_Level(level), m_GuildStorage(new Storage(StorageSize,STORAGE_GUILD))
{
    m_GP = 0;
    m_UnionID = 0;
    m_EmblemIndex = 0;

    m_WarFlag = false;

    m_StorageInUse = false;
    m_StoredGold = 0;
}

void Guild::set_id (const uint32_t ID)
{
	m_ID = ID;
}

void Guild::set_name (const std::string &name)
{
	m_Name = name;
}

void Guild::set_notice (const std::string &title, const std::string &msg)
{
	m_Title = title;
	m_Msg = msg;

    if (!signal_notice.empty())
        signal_notice(title,msg);
}

void Guild::set_level (const uint8_t lvl)
{
	m_Level = lvl;
}

void Guild::set_gp (const uint32_t GP)
{
	m_GP = GP;
}

void Guild::set_emblem_index (const uint32_t index)
{
	m_EmblemIndex = index;
}

void Guild::set_union_id (const uint32_t ID)
{
	m_UnionID = ID;

    if (!signal_alliance.empty())
        signal_alliance(ID);
}

void Guild::set_stored_gold (const uint64_t amount)
{
    m_StoredGold = amount;
}

uint32_t Guild::get_id () const
{
	return m_ID;
}

const std::string& Guild::get_name () const
{
	return m_Name;
}

const std::string& Guild::get_notice_title () const
{
	return m_Title;
}

const std::string& Guild::get_notice_msg () const
{
	return m_Msg;
}

uint32_t Guild::get_gp () const
{
	return m_GP;
}

uint8_t Guild::get_level () const
{
	return m_Level;
}

uint32_t Guild::get_emblem_index () const
{
	return m_EmblemIndex;
}

uint32_t Guild::get_union_id () const
{
	return m_UnionID;
}

uint64_t Guild::get_stored_gold () const
{
    return m_StoredGold;
}

boost::shared_ptr<Storage> Guild::get_guild_storage ()
{
    return m_GuildStorage;
}

void Guild::set_storage_usage (const bool use)
{
    m_StorageInUse = use;
}

bool Guild::is_storage_in_use () const
{
    return m_StorageInUse;
}

void Guild::promote ()
{
    ++m_Level;

    if (!signal_promote.empty())
        signal_promote(m_Level,m_GP);
}

void Guild::disband ()
{
    if (!signal_disband.empty())
        signal_disband();

    m_MemberList.clear();
    m_Master.reset();
}

bool Guild::spend_gp (const uint32_t GP)
{
	if (GP > m_GP)
		return false;

	m_GP -= GP;

    if (!signal_gp.empty())
        signal_gp(m_GP);

	return true;
}

bool Guild::donate_gp (const std::string &name, const uint32_t GP)
{
	iterator i = m_MemberList.find(name);

	if (i == m_MemberList.end())
		return false;

	m_GP += GP;
	i->second->GP += GP;

    if (!signal_gp.empty())
        signal_gp(m_GP);

    if (!signal_member_gp.empty())
        signal_member_gp(i->second->ID,i->second->GP);

	return true;
}

void Guild::member_state (const std::string &name, bool is_leaving)
{
	iterator i = m_MemberList.find(name);

	if (i != m_MemberList.end())
	{
        i->second->Status = is_leaving;

        if (!signal_member_state.empty())
            signal_member_state(i->second->ID,i->second->Status);
	}
}

void Guild::member_level (const std::string &name, uint8_t level)
{
    iterator i = m_MemberList.find(name);

    if (i != m_MemberList.end())
    {
        i->second->Level = level;

        if (!signal_member_level.empty())
            signal_member_level(i->second->ID,i->second->Level);
    }
}

void Guild::set_member_grantname (const uint32_t memberID, const std::string &grant)
{
    for (iterator i = m_MemberList.begin(); i != m_MemberList.end(); ++i)
    {
        if ( i->second->ID == memberID)
        {
            i->second->GrantName = grant;

            if (!signal_member_grant.empty())
                signal_member_grant(m_ID,i->second->Name,grant);

            break;
        }
    }
}

bool Guild::member_withdraw (const std::string &name, const GUILD_MEMBER_LEAVE_REASON reason)
{
    iterator i = m_MemberList.find(name);

    if ( i == m_MemberList.end())
        return false;

    if (!signal_member_remove.empty())
        signal_member_remove(i->second->Name,i->second->ID,reason);

    m_MemberList.erase(i);

    return true;
}

bool Guild::member_fortress_position (const uint32_t memberID, const uint8_t position)
{
    iterator iter = find(memberID);

    if (iter == end())
        return false;

    iter->second->Position = position;

    if (!signal_member_position.empty())
        signal_member_position(memberID,position);

    return true;
}

bool Guild::member_zone (const std::string &name, const int16_t zoneID)
{
    iterator iter = m_MemberList.find(name);

    if (iter == m_MemberList.end())
        return false;

    if (iter->second->Zone == zoneID)
        return true;

    iter->second->Zone = zoneID;

    if (!signal_member_zone.empty())
        signal_member_zone(iter->second->ID,zoneID);

    return true;
}

bool Guild::member_transfer_leadership (const uint32_t currentMasterID, const uint32_t nextMasterID)
{
    iterator iter = find(currentMasterID);

    iterator niter = find(nextMasterID);

    if (niter == end())
        return false;

    /// UPDATE THE MASTER STATUS AND AUTHORITY FLAGS
    iter->second->Master = false;
    niter->second->Master = true;

    for (int i = 0; i < 5; ++i)
    {
        iter->second->Rights[i] = false;
        niter->second->Rights[i] = true;
    }

    m_Master = niter->second;

    if (!signal_member_leader.empty())
        signal_member_leader(currentMasterID,nextMasterID);

    return true;
}

bool Guild::member_authority (const uint8_t count, const boost::shared_array<uint32_t> &memberIDs,
        const boost::shared_array<uint32_t> &authority)
{
    boost::scoped_array<Guild::iterator> miter_list(new Guild::iterator[count]);

    ///CHECK THAT ALL MEMBERS EXISTS!!
    for (int i = 0; i < count; ++i)
    {
        miter_list[i] = find(memberIDs[i]);

        if (miter_list[i] == end())
            return false;
    }

    ///UPDATE AUTHORITY FOR EACH MEMEBER
    for (int i = 0; i < count; ++i)
    {
        for ( int j = 0; j < 5; ++j)
            miter_list[i]->second->Rights[j] = ((authority[i]) & (1<<j));
    }

    if (!signal_member_authority.empty())
        signal_member_authority(count,memberIDs,authority);

    return true;
}

bool Guild::full () const
{
	bool is_full = false;
	size_t count = size();

	switch(m_Level)
	{
	case 1:
		is_full = (count == 15);
		break;
	case 2:
		is_full = (count == 20);
		break;
	case 3:
		is_full = (count == 25);
		break;
	case 4:
		is_full = (count == 35);
		break;
	case 5:
		is_full = (count == 50);
		break;
	}

	return is_full;
}

bool Guild::is_at_war () const
{
    return m_WarFlag;
}

bool Guild::is_guild_master (const std::string &name) const
{
    return m_Master->Name == name;
}

size_t Guild::size () const
{
	return m_MemberList.size();
}

Guild::iterator Guild::begin ()
{
	return m_MemberList.begin();
}

Guild::const_iterator Guild::begin () const
{
	return m_MemberList.begin();
}

Guild::iterator Guild::end ()
{
	return m_MemberList.end();
}

Guild::const_iterator Guild::end () const
{
	return m_MemberList.end();
}

void Guild::insert (const boost::shared_ptr<GuildMember> &member)
{
	m_MemberList.insert(std::make_pair(member->Name,member));

	if (member->Master)
        m_Master = member;

    if (!signal_member_insert.empty())
        signal_member_insert(m_ID,member->Name);
}

Guild::iterator Guild::find (const uint32_t memberID)
{
    iterator i;
    for ( i = begin(); i != end(); ++i)
        if ( i->second->ID == memberID)
            break;

    return i;
}

Guild::const_iterator Guild::find (const std::string &name) const
{
	return m_MemberList.find(name);
}

boost::shared_ptr<GuildMember> Guild::get_master () const
{
    return m_Master;
}

bool Guild::send_msg (const std::string &sender, const std::string &msg)
{
    const_iterator iter = m_MemberList.find(sender);

    if (iter == m_MemberList.end())
        return false;

    if (!signal_msg.empty())
        signal_msg(sender,msg);

    return true;
}

Guild::member_container_type Guild::get_member_list () const
{
    return m_MemberList;
}
