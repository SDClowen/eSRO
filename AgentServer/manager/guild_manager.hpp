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

#ifndef __GAME_SERVER_GUILD_MANAGER_HPP__
#define __GAME_SERVER_GUILD_MANAGER_HPP__

#include <mysql++/mysql++.h>

#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

class Guild;
class GuildMember;

class GuildManager
{
public:

    typedef boost::unordered_map<uint32_t,boost::shared_ptr<Guild> >::iterator guild_iterator;
    typedef boost::unordered_map<uint32_t,boost::shared_ptr<Guild> >::const_iterator guild_const_iterator;

    GuildManager (const std::string &usr, const std::string &pw, const std::string &host, const std::string &db);

	bool Initialize ();

	void Release ();

	bool IsValidName (const std::string &name);

	boost::shared_ptr<Guild> CreateGuild (const std::string &name);

	void DeleteGuild (const uint32_t guildID);

	boost::shared_ptr<Guild> LoadGuild (const uint32_t ID);

	boost::shared_ptr<Guild> Find (const uint32_t ID);

    /**
     *
     *  @brief Unload a guild object from memory.
     *  @ID: Guild ID to unload.
     *
     **/

	void UnloadGuild (const uint32_t ID);

	boost::shared_ptr<GuildMember> CreateMember (const uint32_t guildID, const uint8_t lvl,
        const uint32_t model, const bool master, const std::string &name, const int16_t zoneID);

    /**
     *
     *  @brief Obtain requirements to level up the guild to the desired level from the one below it.
     *  @level Guild next level.
     *  @return a pair of values the first is the required gold amount and the second is the required guild points
     *  to level up the guild.
     *
     **/

    std::pair<uint16_t,uint16_t> GetPromotePolicy (const uint8_t level) const;

private:

    mutable boost::mutex m_mutex;

    std::string m_db;
    std::string m_db_host;
    std::string m_db_usr;
    std::string m_db_pw;

    mysqlpp::Connection m_db_conn;

	std::map<uint8_t,std::pair<uint16_t,uint16_t> > m_GuildPromoteChart;
    boost::unordered_map<uint32_t,boost::shared_ptr<Guild> > m_GuildTable;
};

#endif //__GAME_SERVER_GUILD_MANAGER_HPP__
