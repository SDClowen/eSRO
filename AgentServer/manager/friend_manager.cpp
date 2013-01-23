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

#include "friend_manager.hpp"
#include "query/friend_query.hpp"

FriendManager::FriendManager (const std::string &usr, const std::string &pw, const std::string &host,
    const std::string &db)
    : m_db(db), m_db_host(host), m_db_usr(usr), m_db_pw(pw), m_db_conn(false)
{
}

bool FriendManager::initialize ()
{
    boost::mutex::scoped_lock lock(m_mutex);

    m_db_conn.connect(m_db.c_str(),m_db_host.c_str(),m_db_usr.c_str(),m_db_pw.c_str());

    if (!m_db_conn.connected())
        return false;

    return true;
}

void FriendManager::release ()
{
    boost::mutex::scoped_lock lock(m_mutex);

    m_db_conn.disconnect();
}

bool FriendManager::loadFriendbook (const uint32_t charID)
{
    boost::mutex::scoped_lock lock(m_mutex);

    const_iterator it = m_friendTable.find(charID);

    if (it != m_friendTable.end())
        return true;

    DB::FRIEND::Select friend_query;
    FriendBook book = friend_query(m_db_conn,charID);

    m_friendTable.insert(std::make_pair(charID,book));

    return true;
}

FriendBook FriendManager::findFriendbook (const uint32_t charID) const
{
    FriendBook book;

    boost::mutex::scoped_lock lock(m_mutex);

    const_iterator it = m_friendTable.find(charID);

    if (it != m_friendTable.end())
        book = it->second;

    return book;
}

bool FriendManager::isFriendMax (uint32_t charID) const
{
    boost::mutex::scoped_lock lock(m_mutex);

    const_iterator it = m_friendTable.find(charID);

    if (it == m_friendTable.end())
        return true;

    return it->second.friends.size() == MAX_FRIENDS;
}

bool FriendManager::isFriendAdded (const uint32_t charID, const std::string &name)
{
    boost::mutex::scoped_lock lock(m_mutex);

    const_iterator it = m_friendTable.find(charID);

    if (it == m_friendTable.end())
        return true;

    FriendBook::friend_const_iterator fit;

    for (fit = it->second.friends.begin(); fit != it->second.friends.end(); ++fit)
    {
        if (fit->Name == name)
            break;
    }

    return fit != it->second.friends.end();
}

bool FriendManager::addFriend (const Friend &friendA, const Friend &friendB)
{
    boost::mutex::scoped_lock lock(m_mutex);

    iterator ait = m_friendTable.find(friendA.ID);

    if (ait == m_friendTable.end())
        return false;

    iterator bit = m_friendTable.find(friendB.ID);

    if (bit == m_friendTable.end())
        return false;

    ait->second.friends.push_back(friendB);
    bit->second.friends.push_back(friendA);

    DB::FRIEND::Insert insert_query;
    insert_query(m_db_conn,friendA.ID,friendB.ID);

    return true;
}

std::pair<bool,std::string> FriendManager::deleteFriend (const uint32_t friendA, const uint32_t friendB)
{
    boost::mutex::scoped_lock lock(m_mutex);

    std::pair<bool,std::string> ret;
    ret.first = false;

    iterator ait = m_friendTable.find(friendA);

    if (ait == m_friendTable.end())
        return ret;

    FriendBook::friend_iterator i;
    for (i = ait->second.friends.begin(); i != ait->second.friends.end(); ++i)
    {
        if (i->ID == friendB)
            break;
    }

    if (i == ait->second.friends.end())
        return ret;

    if (i->groupID)
    {
        for (FriendBook::group_iterator git = ait->second.groups.begin(); git != ait->second.groups.end(); ++git)
        {
            if (git->groupID == i->groupID)
                --(git->friendCount);
        }
    }

    ret.second = i->Name;

    ait->second.friends.erase(i);

    iterator bit = m_friendTable.find(friendB);

    if (bit == m_friendTable.end())
        return ret;

    for (i = bit->second.friends.begin(); i != bit->second.friends.end(); ++i)
    {
        if (i->ID == friendA)
            break;
    }

    if (i == bit->second.friends.end())
        return ret;

    if (i->groupID)
    {
        for (FriendBook::group_iterator git = bit->second.groups.begin(); git != bit->second.groups.end(); ++git)
        {
            if (git->groupID == i->groupID)
                --(git->friendCount);
        }
    }

    bit->second.friends.erase(i);

    ret.first = true;

    DB::FRIEND::Delete delete_query;
    delete_query(m_db_conn,friendA,friendB);

    return ret;
}

uint16_t FriendManager::createGroup (const uint32_t charID, const std::string &name)
{
    boost::mutex::scoped_lock lock(m_mutex);

    iterator it = m_friendTable.find(charID);

    if (it == m_friendTable.end())
        return 0;

    uint16_t groupID = 1;

    for (size_t i = 0; i < it->second.groups.size(); ++i,++groupID)
    {
        if (groupID != it->second.groups[i].groupID)
            break;
    }

    FriendGroup group;
    DB::FRIEND::GroupCreate query;

    group.refID = query(m_db_conn,charID,groupID,name);
    group.Name = name;
    group.friendCount = 0;

    if (group.refID)
    {
        it->second.groups.push_back(group);
        std::sort(it->second.groups.begin(),it->second.groups.end());
    }
    else
        groupID = 0;

    return groupID;
}

bool FriendManager::moveFriendIntoGroup (const uint32_t charID, const uint32_t friendID, const uint32_t groupID)
{
    boost::mutex::scoped_lock lock(m_mutex);

    iterator it = m_friendTable.find(charID);

    if (it == m_friendTable.end())
        return false;

    FriendBook::friend_iterator fit;

    for (fit = it->second.friends.begin(); fit != it->second.friends.end(); ++fit)
    {
        if (fit->ID == friendID)
            break;
    }

    if (fit == it->second.friends.end())
        return false;

    FriendBook::group_iterator git;

    if (fit->groupID)
    {
        /// DECREASE FRIEND COUNT FROM CURRENT GROUP
        for (git = it->second.groups.begin(); git != it->second.groups.end(); ++git)
        {
            if (git->groupID == fit->groupID && git->friendCount)
            {
                --(git->friendCount);

                DB::FRIEND::GroupEntryRemove remove_query;
                remove_query(m_db_conn,fit->ID,git->refID);

                break;
            }
        }
    }

    fit->groupID = 0;

    /// ADD FRIEND TO DESIRED GROUP
    for (git = it->second.groups.begin(); git != it->second.groups.end(); ++git)
    {
        if (git->groupID == groupID)
            break;
    }

    if (git == it->second.groups.end())
        return false;

    fit->groupID = groupID;

    DB::FRIEND::GroupEntryInsert insert_query;
    insert_query(m_db_conn,fit->ID,git->refID);

    return true;
}

bool FriendManager::deleteGroup (const uint32_t charID, const uint32_t groupID)
{
    boost::mutex::scoped_lock lock(m_mutex);

    iterator it = m_friendTable.find(charID);

    if (it == m_friendTable.end())
        return false;

    FriendBook::group_iterator git;

    for (git = it->second.groups.begin(); git != it->second.groups.end(); ++git)
    {
        if (git->groupID == groupID)
            break;
    }

    if (git == it->second.groups.end())
        return false;

    FriendBook::friend_iterator fit;

    for (fit = it->second.friends.begin(); fit != it->second.friends.end(); ++fit)
    {
        if (fit->groupID == groupID)
            fit->groupID = 0;
    }

    DB::FRIEND::GroupDelete delete_query;
    delete_query(m_db_conn,git->refID);

    it->second.groups.erase(git);

    return true;
}

void FriendManager::unloadFriendbook (const uint32_t charID)
{
    boost::mutex::scoped_lock lock(m_mutex);

    m_friendTable.erase(charID);
}
