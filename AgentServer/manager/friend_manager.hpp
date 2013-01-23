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

#ifndef FRIEND_MANAGER_HPP
#define FRIEND_MANAGER_HPP

#include <friend.hpp>

#include <boost/unordered_map.hpp>
#include <boost/thread/mutex.hpp>
#include <mysql++/mysql++.h>

class FriendManager
{
public:

    typedef boost::unordered_map<uint32_t,FriendBook>::iterator iterator;
    typedef boost::unordered_map<uint32_t,FriendBook>::const_iterator const_iterator;

    FriendManager (const std::string &usr, const std::string &pw, const std::string &host, const std::string &db);

    bool initialize ();

    void release ();

    bool loadFriendbook (const uint32_t charID);

    FriendBook findFriendbook (const uint32_t charID) const;

    bool isFriendMax (const uint32_t charID) const;

    bool isFriendAdded (const uint32_t charID, const std::string &name);

    bool addFriend (const Friend &friendA, const Friend &friendB);

    std::pair<bool,std::string> deleteFriend (const uint32_t charID, const uint32_t friendID);

    uint16_t createGroup (const uint32_t charID, const std::string &name);

    bool moveFriendIntoGroup (const uint32_t charID, const uint32_t friendID, const uint32_t groupID);

    bool deleteGroup (const uint32_t charID, const uint32_t groupID);

    void unloadFriendbook (const uint32_t charID);

private:

    mutable boost::mutex m_mutex;

    std::string m_db;
    std::string m_db_host;
    std::string m_db_usr;
    std::string m_db_pw;

    mysqlpp::Connection m_db_conn;

    boost::unordered_map<uint32_t,FriendBook> m_friendTable;
};

#endif // FRIEND_MANAGER_HPP
