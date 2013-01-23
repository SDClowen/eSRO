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

#include "union.hpp"
#include "guild.hpp"

#include <boost/bind.hpp>

Union::Union (const uint32_t ID, const boost::shared_ptr<Guild> &head,
        const boost::shared_ptr<Guild> &slave)
    : m_ID(ID), m_EmblemIndex(0),m_HeadGuildID(head->get_id())
{
    m_GuildTable.push_back(head);
    m_GuildTable.push_back(slave);

    for (iterator i = m_GuildTable.begin(); i != m_GuildTable.end(); ++i)
        connect_guild_signals(*i);
}

Union::Union (const uint32_t unionID, const uint32_t idx, const uint32_t headGuildID,
        const std::vector<boost::shared_ptr<Guild> > &list)
    : m_ID(unionID), m_EmblemIndex(idx), m_HeadGuildID(headGuildID), m_GuildTable(list)
{
    for (iterator i = m_GuildTable.begin(); i != m_GuildTable.end(); ++i)
        connect_guild_signals(*i);
}

uint32_t Union::id () const
{
    return m_ID;
}

uint32_t Union::head_guild_id () const
{
    return m_HeadGuildID;
}

uint32_t Union::emblem_index () const
{
    return m_EmblemIndex;
}

size_t Union::size () const
{
    return m_GuildTable.size();
}

bool Union::empty () const
{
    return m_GuildTable.empty();
}

bool Union::full() const
{
    return m_GuildTable.size() == UNION_MAX_GUILD_COUNT;
}

bool Union::is_leading_guild (const uint32_t guildID) const
{
    bool ret = false;

    for (const_iterator i = m_GuildTable.begin(); i != m_GuildTable.end(); ++i)
    {
        if ((*i)->get_id() == guildID)
        {
            ret = true;
            break;
        }
    }

    return ret;
}

void Union::insert (const boost::shared_ptr<Guild> &guild)
{
    m_GuildTable.push_back(guild);

    if(!signal_insert.empty())
        signal_insert(guild->get_id());

    connect_guild_signals(guild);
}

bool Union::remove (const uint32_t guildID, const UNION_GUILD_REASON reason)
{
    for (iterator i = m_GuildTable.begin(); i != m_GuildTable.end(); ++i)
    {
        if ((*i)->get_id() == guildID)
        {
            if (!signal_remove.empty())
                signal_remove(guildID,reason);

            if (guildID == m_HeadGuildID && !signal_disband.empty())
                signal_disband(guildID);

            disconnect_guild_signals(*i);

            (*i)->set_union_id(0);

            m_GuildTable.erase(i);
            return true;
        }
    }

    return false;
}

void Union::send_msg (const std::string &sender, const std::string &msg)
{
    if (!signal_msg.empty())
        signal_msg(sender,msg);
}

std::vector<boost::shared_ptr<Guild> > Union::guild_list () const
{
    return m_GuildTable;
}

void Union::connect_guild_signals (const boost::shared_ptr<Guild> &guild)
{
    if (guild)
    {
        guild->signal_promote.connect(boost::bind(&Union::OnGuildLevel,this,guild->get_id(),_1));
        guild->signal_member_insert.connect(boost::bind(&Union::OnGuildMember,this,guild->get_id()));
        guild->signal_member_remove.connect(boost::bind(&Union::OnGuildMember,this,guild->get_id()));
        guild->signal_member_leader.connect(boost::bind(&Union::OnGuildLeader,this,guild->get_id()));
    }
}

void Union::disconnect_guild_signals (const boost::shared_ptr<Guild> &guild)
{
    if (guild)
    {
        guild->signal_promote.disconnect(boost::bind(&Union::OnGuildLevel,this,guild->get_id(),_1));
        guild->signal_member_insert.disconnect(boost::bind(&Union::OnGuildMember,this,guild->get_id()));
        guild->signal_member_remove.disconnect(boost::bind(&Union::OnGuildMember,this,guild->get_id()));
        guild->signal_member_leader.disconnect(boost::bind(&Union::OnGuildLeader,this,guild->get_id()));
    }
}

void Union::OnGuildLevel (const uint32_t guildID,const uint8_t lvl)
{
    if (!signal_level.empty())
        signal_level(guildID,lvl);
}

void Union::OnGuildMember (const uint32_t guildID)
{
    for (iterator iter = m_GuildTable.begin(); iter != m_GuildTable.end(); ++iter)
    {
        if ((*iter)->get_id() == guildID)
        {
            if (!signal_member.empty())
                signal_member(guildID,(*iter)->size());
        }
    }
}

void Union::OnGuildLeader (const uint32_t guildID)
{
    if (!signal_leader.empty())
        signal_leader(guildID);
}
