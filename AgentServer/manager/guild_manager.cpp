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

#include "guild_manager.hpp"

/// SRLF INCLUDES
#include <guild.hpp>
#include <guild_member.hpp>

/// SRDL INCLUDES
#include <guild_query.hpp>
#include <guild_member_query.hpp>

GuildManager::GuildManager (const std::string &usr, const std::string &pw, const std::string &host,
    const std::string &db)
    : m_db(db), m_db_host(host), m_db_usr(usr), m_db_pw(pw), m_db_conn(false)
{
}

bool GuildManager::Initialize ()
{
    m_db_conn.connect(m_db.c_str(),m_db_host.c_str(),m_db_usr.c_str(),m_db_pw.c_str());

    if (!m_db_conn.connected())
		return false;

    DB::GUILD::PromotePolicy promote_query;
    m_GuildPromoteChart = promote_query(m_db_conn);

    if (m_GuildPromoteChart.empty())
        return false;

	return true;
}

bool GuildManager::IsValidName (const std::string &name)
{
    boost::mutex::scoped_lock lock(m_mutex);

    if (!m_db_conn.connected())
        m_db_conn.connect(m_db.c_str(),m_db_host.c_str(),m_db_usr.c_str(),m_db_pw.c_str());

    DB::GUILD::Valid query;
    return query(m_db_conn,name);
}

boost::shared_ptr<Guild> GuildManager::CreateGuild (const std::string &name)
{
    boost::mutex::scoped_lock lock(m_mutex);

    if (!m_db_conn.connected())
        m_db_conn.connect(m_db.c_str(),m_db_host.c_str(),m_db_usr.c_str(),m_db_pw.c_str());

	DB::GUILD::Create create_query;
	uint32_t guild_id = create_query(m_db_conn,name);

	boost::shared_ptr<Guild> guild(new Guild(guild_id,name,1,0));

    if (guild)
        m_GuildTable.insert(std::make_pair(guild_id,guild));

	return guild;
}

void GuildManager::DeleteGuild (const uint32_t guildID)
{
    boost::mutex::scoped_lock lock(m_mutex);

    if (!m_db_conn.connected())
        m_db_conn.connect(m_db.c_str(),m_db_host.c_str(),m_db_usr.c_str(),m_db_pw.c_str());

    DB::GUILD::Delete query;
	query(m_db_conn,guildID);

	m_GuildTable.erase(guildID);
}

boost::shared_ptr<Guild> GuildManager::LoadGuild (const uint32_t guildID)
{
    boost::mutex::scoped_lock lock(m_mutex);

    if (!m_db_conn.connected())
        m_db_conn.connect(m_db.c_str(),m_db_host.c_str(),m_db_usr.c_str(),m_db_pw.c_str());

    DB::GUILD::Select query;
    boost::shared_ptr<Guild> guild = query(m_db_conn,guildID);

    if (guild)
        m_GuildTable.insert(std::make_pair(guildID,guild));

    return guild;
}

boost::shared_ptr<Guild> GuildManager::Find (const uint32_t ID)
{
    boost::mutex::scoped_lock lock(m_mutex);

    boost::shared_ptr<Guild> guild;

    guild_iterator iter = m_GuildTable.find(ID);

    if (iter != m_GuildTable.end())
        guild = iter->second;

    return guild;
}

void GuildManager::UnloadGuild (const uint32_t ID)
{
    boost::mutex::scoped_lock lock(m_mutex);

    m_GuildTable.erase(ID);
}

boost::shared_ptr<GuildMember> GuildManager::CreateMember (const uint32_t guildID, const uint8_t lvl,
    const uint32_t model, const bool master, const std::string &name, const int16_t zoneID)
{
    boost::mutex::scoped_lock lock(m_mutex);

    if (!m_db_conn.connected())
        m_db_conn.connect(m_db.c_str(),m_db_host.c_str(),m_db_usr.c_str(),m_db_pw.c_str());

	boost::shared_ptr<GuildMember> member(new GuildMember());

    if (member)
    {
        member->GuildID = guildID;
        member->GP = 0;
        member->CombatPoints = 0;
        member->Level = lvl;
        member->refCharID = model;
        member->Master = master;
        member->Status = false;
        member->Name = name;
        member->Position = GUILD_MEMBER_POSITION_NO_POSITION;
        member->Zone = zoneID;

        for (int i = 0; i < 5; ++i)
            member->Rights[i] = master;

        DB::GUILD_MEMBER::Insert insert_query;
        member->ID = insert_query(m_db_conn,member);
    }

	return member;
}

std::pair<uint16_t,uint16_t> GuildManager::GetPromotePolicy (const uint8_t level) const
{
    boost::mutex::scoped_lock lock(m_mutex);

    return m_GuildPromoteChart.find(level)->second;
}
