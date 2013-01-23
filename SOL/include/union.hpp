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

#ifndef __SRLF_UNION_HPP__
#define __SRLF_UNION_HPP__

#include <vector>
#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

#define UNION_MAX_GUILD_COUNT 8
#define UNION_MIMINUM_GUILD_COUNT 2

enum UNION_GUILD_REASON
{
    UG_REASON_SECEDE = 1,
    UG_REASON_EXPEL
};

class Guild;

class Union
{
public:

    typedef std::vector<boost::shared_ptr<Guild> >::iterator iterator;
    typedef std::vector<boost::shared_ptr<Guild> >::const_iterator const_iterator;

    /**
     *
     *  ALLIANCE SIGNALS
     *
     **/

    boost::signals2::signal<void (uint32_t)> signal_insert;
    boost::signals2::signal<void (uint32_t,uint8_t)> signal_level;
    boost::signals2::signal<void (uint32_t,uint8_t)> signal_member;
    boost::signals2::signal<void (const std::string&, const std::string&)> signal_msg;
    boost::signals2::signal<void (uint32_t, uint8_t)> signal_remove;
    boost::signals2::signal<void (uint32_t)> signal_disband;
    boost::signals2::signal<void (uint32_t)> signal_leader;

    Union (const uint32_t unionID, const boost::shared_ptr<Guild> &head,
        const boost::shared_ptr<Guild> &slave);

    Union (const uint32_t unionID, const uint32_t idx, const uint32_t headGuildID,
        const std::vector<boost::shared_ptr<Guild> > &list);

    uint32_t id () const;

    uint32_t head_guild_id () const;

    uint32_t emblem_index () const;

    size_t size () const;

    bool empty() const;

    bool full () const;

    bool is_leading_guild (const uint32_t guildID) const;

    void insert (const boost::shared_ptr<Guild> &guild);

    bool remove (const uint32_t guildID, const UNION_GUILD_REASON remove_reason);

    void send_msg (const std::string &sender, const std::string &msg);

    std::vector<boost::shared_ptr<Guild> > guild_list () const;

private:

    void connect_guild_signals (const boost::shared_ptr<Guild> &guild);

    void disconnect_guild_signals (const boost::shared_ptr<Guild> &guild);

    void OnGuildLevel (const uint32_t guildID,const uint8_t lvl);

    void OnGuildMember (const uint32_t guilID);

    void OnGuildLeader (const uint32_t guildID);

private:

    uint32_t m_ID;
    uint32_t m_EmblemIndex;
    uint32_t m_HeadGuildID;

    std::vector<boost::shared_ptr<Guild> > m_GuildTable;
};

#endif
