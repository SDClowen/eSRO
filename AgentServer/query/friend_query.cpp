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

#include "friend_query.hpp"

namespace DB {
namespace FRIEND {

FriendBook Select::operator() (mysqlpp::Connection &conn, const uint32_t charID)
{
    mysqlpp::Query query = conn.query();

    FriendBook book;

    query << "select characters.State,characters.Name,characters.refCharID,friends.charID_A,friends.charID_B "
        << "from characters,friends where (friends.charID_A = " << charID
        << " and characters.ID = friends.charID_B)"
        << " or (friends.charID_B = " << charID
        << " and characters.ID = friends.charID_A)";

    mysqlpp::StoreQueryResult res = query.store();

    Friend nfriend;
    for (size_t i = 0; i < res.num_rows(); ++i)
    {
        if (static_cast<uint32_t>(res[i]["charID_A"]) == charID)
            nfriend.ID = res[i]["charID_B"];
        else
            nfriend.ID = res[i]["charID_A"];

        nfriend.Name = static_cast<std::string>(res[i]["Name"]);
        nfriend.Model = res[i]["refCharID"];
        nfriend.status = res[i]["State"];

        book.friends.push_back(nfriend);
    }

    query.reset();

    query << "select * from friend_group where charID = " << charID << " order by groupID asc";

    res = query.store();

    FriendGroup group;
    mysqlpp::SimpleResult gres;

    for (size_t i = 0; i < res.num_rows(); ++i)
    {
        group.refID = res[i]["refgroupID"];
        group.Name = static_cast<std::string>(res[i]["Name"]);
        group.groupID = res[i]["groupID"];

        query.reset();

        query << "select * from friend_group_entry where refgroupID = " << group.refID;

        gres = query.execute();

        group.friendCount = gres.rows();

        book.groups.push_back(group);
    }

    return book;
}


void Delete::operator() (mysqlpp::Connection &conn, const uint32_t CharID_A, const uint32_t CharID_B)
{
    mysqlpp::Query query = conn.query();

    query << "delete from friends where (CharID_A =" << CharID_A << " AND CharID_B = " << CharID_B
        << ") OR (CharID_A = " << CharID_B << " AND CharID_B =" << CharID_A << ")";

    query.exec();
}

void Insert::operator() (mysqlpp::Connection &conn, const uint32_t CharID_A, const uint32_t CharID_B)
{
    mysqlpp::Query query = conn.query();

    query << "insert into friends values (" << CharID_A << "," << CharID_B << ")";

    query.exec();
}

bool GroupValid::operator() (mysqlpp::Connection &conn, const std::string &name)
{
    mysqlpp::Query query = conn.query();

    query << "select * from friend_group where Name = " << mysqlpp::quote << name;

    mysqlpp::SimpleResult res = query.execute();

    return res.rows() > 0;
}

uint32_t GroupCreate::operator () (mysqlpp::Connection &conn, const uint32_t charID, const uint16_t groupID,
    const std::string &name)
{
    uint32_t ID = 0;

    GroupValid valid_query;
    if (valid_query(conn,name))
    {
        mysqlpp::Query query = conn.query();

        query << "insert into friend_group (charID,groupID,Name) values (" << charID << ","
            << groupID << "," << mysqlpp::quote << name << ")";

        query.exec();

        ID = query.insert_id();
    }

    return ID;
}

void GroupDelete::operator () (mysqlpp::Connection &conn, const uint32_t refgroupID)
{
    mysqlpp::Query query = conn.query();

    query << "delete from friend_group where refgroupID = " << refgroupID;

    query.exec();

    query << "delete from friend_group_entry where refgroupID = " << refgroupID;

    query.exec();
}

void GroupEntryInsert::operator () (mysqlpp::Connection &conn, const uint32_t charID, const uint32_t refgroupID)
{
    mysqlpp::Query query = conn.query();

    query << "insert into friend_group_entry values (" << refgroupID << "," << charID << ")";

    query.exec();
}

void GroupEntryRemove::operator () (mysqlpp::Connection &conn, const uint32_t charID, const uint32_t refgroupID)
{
    mysqlpp::Query query = conn.query();

    query << "delete from friend_group_entry where charID = " << charID << " and refgroupID = " << refgroupID;

    query.exec();
}

}	}
