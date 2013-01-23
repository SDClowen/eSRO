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

#include "union_manager.hpp"
#include "guild_manager.hpp"

/// SRLF INCLUDES
#include <union.hpp>
#include <guild.hpp>

/// SRDL INCLUDES
#include <alliance_query.hpp>

UnionManager::UnionManager (const std::string &usr, const std::string &pw, const std::string &host,
    const std::string &db, const boost::shared_ptr<GuildManager> &mgr)
    : m_db(db), m_db_host(host), m_db_usr(usr), m_db_pw(pw), m_db_conn(false), m_GuildMgr(mgr)
{
    m_db_conn.connect(m_db.c_str(),m_db_host.c_str(),m_db_usr.c_str(),m_db_pw.c_str());
}

bool UnionManager::Initialize ()
{
    if (!m_GuildMgr)
        return false;

    if (!m_db_conn.connected())
		return false;

    return true;
}

boost::shared_ptr<Union> UnionManager::Create (const boost::shared_ptr<Guild> &head,
    const boost::shared_ptr<Guild> &slave)
{
    boost::mutex::scoped_lock lock(m_mutex);

    if (!m_db_conn.connected())
    {
        m_db_conn.connect(m_db.c_str(),m_db_host.c_str(),m_db_usr.c_str(),m_db_pw.c_str());
    }

    DB::ALLIANCE::Create query;
    uint32_t allyID = query(m_db_conn,head->get_id());

    boost::shared_ptr<Union> ally(new Union(allyID,head,slave));

    if (ally)
    {
        m_UnionTable.insert(std::make_pair(allyID,ally));
        lock.unlock();

        head->set_union_id(allyID);
        slave->set_union_id(allyID);
    }

    return ally;
}

boost::shared_ptr<Union> UnionManager::Load (const uint32_t unionID)
{
    boost::mutex::scoped_lock lock(m_mutex);

    if (!m_db_conn.connected())
    {
        m_db_conn.connect(m_db.c_str(),m_db_host.c_str(),m_db_usr.c_str(),m_db_pw.c_str());
    }

    DB::ALLIANCE::Select query;
    std::pair<uint32_t,uint32_t> data = query(m_db_conn,unionID);

    DB::ALLIANCE::List list_query;
    std::vector<uint32_t> id_list = list_query(m_db_conn,unionID);

    boost::shared_ptr<Union> ally;
    boost::shared_ptr<Guild> guild;
    std::vector<boost::shared_ptr<Guild> > guild_list;

    for (size_t i = 0; i < id_list.size(); ++i)
    {
        guild = m_GuildMgr->Find(id_list[i]);

        if (!guild)
        {
            guild = m_GuildMgr->LoadGuild(id_list[i]);

            if (!guild)
                return ally;
        }

        guild_list.push_back(guild);
    }

    ally.reset(new Union(unionID,data.first,data.second,guild_list));

    m_UnionTable.insert(std::make_pair(unionID,ally));

    return ally;
}

boost::shared_ptr<Union> UnionManager::Find (const uint32_t unionID)
{
    boost::mutex::scoped_lock lock(m_mutex);

    boost::shared_ptr<Union> ptr;

    union_iterator iter = m_UnionTable.find(unionID);

    if (iter != m_UnionTable.end())
        ptr = iter->second;

    return ptr;
}

void UnionManager::Remove (const uint32_t unionID)
{
    boost::mutex::scoped_lock lock(m_mutex);

    m_UnionTable.erase(unionID);

    if (!m_db_conn.connected())
    {
        m_db_conn.connect(m_db.c_str(),m_db_host.c_str(),m_db_usr.c_str(),m_db_pw.c_str());
    }

    DB::ALLIANCE::Delete query;
    query(m_db_conn,unionID);
}
