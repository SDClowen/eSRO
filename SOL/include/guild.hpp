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

#ifndef __SLES_GUILD_HPP__
#define __SLES_GUILD_HPP__

#include "types.hpp"
#include "guild_constants.hpp"

/// GLOBAL INCLUDES
#include <map>
#include <string>
#include <boost/shared_array.hpp>
#include <boost/signals2.hpp>

class Storage;
class GuildMember;

class Guild
{
public:

	typedef boost::shared_ptr<GuildMember> member_pointer_type;

	typedef std::map<std::string,member_pointer_type> member_container_type;

	typedef member_container_type::iterator iterator;
	typedef member_container_type::const_iterator const_iterator;

	Guild ();

	Guild (const uint32_t ID, const std::string &name, const uint8_t level,const uint8_t StorageSize);

	void set_id (const uint32_t id);

	void set_name (const std::string &name);

	void set_notice (const std::string &title, const std::string &msg);

	void set_level (const uint8_t level);

	void set_gp (const uint32_t gp);

	void set_emblem_index (const uint32_t index);

	void set_union_id (const uint32_t ID);

	void set_stored_gold (const uint64_t amount);

	uint32_t get_id () const;

	const std::string& get_name () const;

	const std::string& get_notice_title () const;

	const std::string& get_notice_msg () const;

	uint32_t get_gp () const;

	uint8_t get_level () const;

	uint32_t get_emblem_index () const;

	uint32_t get_union_id () const;

	uint64_t get_stored_gold () const;

	boost::shared_ptr<Storage> get_guild_storage ();

    void set_storage_usage (const bool use);

	bool is_storage_in_use () const;

	void promote ();

	void disband ();

	bool spend_gp (const uint32_t amount);

	bool donate_gp (const std::string &name, const uint32_t GP);

	void member_state (const std::string &name, bool is_leaving);

    void member_level (const std::string &name, uint8_t level);

	void set_member_grantname (const uint32_t memberID, const std::string &grant);

	bool member_withdraw (const std::string &name, const GUILD_MEMBER_LEAVE_REASON reason);

	bool member_fortress_position (const uint32_t memberID, const uint8_t position);

    bool member_zone (const std::string &name, const int16_t zone);

	bool member_transfer_leadership (const uint32_t currentMasterID, const uint32_t nextMasterID);

	bool member_authority (const uint8_t count, const boost::shared_array<uint32_t> &memberIDs,
        const boost::shared_array<uint32_t> &authority);

	/**
	 *
	 *	@brief: Indicates if theres available spots to join a new member.
	 *
	 **/

	bool full () const;

	/**
	 *
	 *  @brief: Indicates if the guild is at war or not.
	 *  @return: guild war status.
	 *
	 **/

	bool is_at_war () const;

	bool is_guild_master (const std::string &name) const;

	/**
	 *
	 *	@brief Guild member count.
	 *
	 **/

	size_t size () const;

	iterator begin ();

	const_iterator begin () const;

	iterator end ();

	const_iterator end () const;

	void insert (const boost::shared_ptr<GuildMember> &member);

    iterator find (const uint32_t memberID);

	const_iterator find (const std::string &name) const;

	boost::shared_ptr<GuildMember> get_master() const;

	bool send_msg (const std::string &sender, const std::string &msg);

    member_container_type get_member_list () const;

	/**
	 *
	 *	GUILD SIGNALS
	 *
	 **/

    boost::signals2::signal<void (const std::string&, const std::string&)> signal_notice;

    boost::signals2::signal<void (const uint8_t, const uint32_t)> signal_promote;

    boost::signals2::signal<void (const uint32_t)> signal_gp;

    boost::signals2::signal<void ()> signal_disband;

    boost::signals2::signal<void (const uint32_t, const uint32_t)> signal_member_gp;

    boost::signals2::signal<void (const uint32_t, const uint8_t)> signal_member_state;

    boost::signals2::signal<void (const uint32_t, const uint8_t)> signal_member_level;

    boost::signals2::signal<void (const uint32_t, const std::string&)> signal_member_insert;

    boost::signals2::signal<void (const std::string&, const uint32_t, const uint8_t)> signal_member_remove;

    boost::signals2::signal<void (const uint32_t, const std::string&, const std::string&)> signal_member_grant;

    boost::signals2::signal<void (const uint32_t, const uint8_t)> signal_member_position;

    boost::signals2::signal<void (const uint32_t, const int16_t)> signal_member_zone;

    boost::signals2::signal<void (const uint32_t, const uint32_t)> signal_member_leader;

    boost::signals2::signal<void (const uint8_t count, const boost::shared_array<uint32_t> &,
        const boost::shared_array<uint32_t> &)> signal_member_authority;

    boost::signals2::signal<void (const uint32_t)> signal_alliance;

    boost::signals2::signal<void (const std::string&, const std::string&)> signal_msg;

private:

	uint32_t m_ID;
	uint32_t m_EmblemIndex;
	std::string m_Name;
	uint8_t m_Level;
	uint32_t m_GP;
	std::string m_Title;
	std::string m_Msg;
	uint32_t m_UnionID;

	bool m_WarFlag;

	member_container_type m_MemberList;

    bool m_StorageInUse;
    uint64_t m_StoredGold;
    boost::shared_ptr<GuildMember> m_Master;
	boost::shared_ptr<Storage> m_GuildStorage;
};

#endif //__SLES_GUILD_HPP__
