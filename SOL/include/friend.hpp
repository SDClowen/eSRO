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

#ifndef __SLES_FRIEND_HPP__
#define __SLES_FRIEND_HPP__

#include <stdint.h>
#include <string>
#include <vector>

#define MAX_FRIENDS 50

struct FriendGroup
{
    bool operator < (const FriendGroup &group) const
    {
        return groupID < group.groupID;
    }

    uint32_t refID;         /// GROUP ID FOR INDEXING IN THE DATABASE!!
    uint16_t groupID;       /// GROUP ID FOR PLAYER!!
    std::string Name;
    uint8_t friendCount;
};

struct Friend
{
    Friend ()
        : ID(0), Model(0), groupID(0), status(false)
    {}

    uint32_t ID;
	std::string Name;
    uint32_t Model;
    uint16_t groupID;        /// REFERS TO (FriendGroup.groupID)!! NOTE: 0 is default group.
	bool status;
};

struct FriendBook
{
    typedef std::vector<FriendGroup>::const_iterator group_const_iterator;
    typedef std::vector<FriendGroup>::iterator group_iterator;
    typedef std::vector<Friend>::const_iterator friend_const_iterator;
    typedef std::vector<Friend>::iterator friend_iterator;

    std::vector<FriendGroup> groups;
    std::vector<Friend> friends;
};

#endif //__SLES_FRIEND_HPP__
