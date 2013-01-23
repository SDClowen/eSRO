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

#ifndef __SR_GAME_SERVER_UNION_MANAGER_HPP__
#define __SR_GAME_SERVER_UNION_MANAGER_HPP__

#include <map>
#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <mysql++/mysql++.h>

class Union;
class Guild;
class GuildManager;

class UnionManager
{
public:

    typedef std::map<uint32_t,boost::shared_ptr<Union> >::iterator union_iterator;
    typedef std::map<uint32_t,boost::shared_ptr<Union> >::const_iterator union_const_iterator;

    UnionManager (const std::string &usr, const std::string &pw, const std::string &host, const std::string &db,
        const boost::shared_ptr<GuildManager> &mgr);

    bool Initialize ();

    boost::shared_ptr<Union> Create (const boost::shared_ptr<Guild> &head,
        const boost::shared_ptr<Guild> &slave);

    boost::shared_ptr<Union> Load (const uint32_t unionID);

    boost::shared_ptr<Union> Find (const uint32_t unionID);

    void Remove (const uint32_t unionID);

private:

    boost::mutex m_mutex;

    std::string m_db;
    std::string m_db_host;
    std::string m_db_usr;
    std::string m_db_pw;

    mysqlpp::Connection m_db_conn;

    boost::shared_ptr<GuildManager> m_GuildMgr;
    std::map<uint32_t,boost::shared_ptr<Union> > m_UnionTable;
};

#endif
