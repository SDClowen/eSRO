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

#include "guild_member_query.hpp"

/// SRLF INCLUDES
#include <guild_member.hpp>

namespace DB {
namespace GUILD_MEMBER {

uint32_t Insert::operator() (mysqlpp::Connection& conn, const boost::shared_ptr<GuildMember> member)
{
	using mysqlpp::quote;

	mysqlpp::Query query = conn.query();

    query << "insert into guild_member (GuildID,Name,Level,refCharID,Master,Position,Zone"
        << "JoinRights,WithdrawRights,UnionRights,StorageRights,NoticeRights) values ("
		<< quote << member->GuildID << "," << quote << member->Name << ","
		<< quote << member->Level << "," << member->refCharID << ","
        << member->Master << "," << member->Position << "," << member->Zone;

	for (int i = 0; i < 5; ++i)
        query << "," << member->Rights[i];

    query << ")";

	query.exec();

	return query.insert_id();
}

void Remove::operator() (mysqlpp::Connection& conn, const uint32_t memberID)
{
	mysqlpp::Query query = conn.query();

	query << "delete from guild_member where ID = " << memberID;

	query.exec();
}

void Master::operator() (mysqlpp::Connection &conn, const uint32_t memberID, const bool is_master)
{
	mysqlpp::Query query = conn.query();

	query << "update guild_member set Master = " << is_master << " where ID = " << memberID;

	query.exec();
}

void Grant::operator() (mysqlpp::Connection& conn, const uint32_t memberID, const std::string& grant)
{
	mysqlpp::Query query = conn.query();

	query << "update guild_member set GrantName = " << mysqlpp::quote << grant
		<< " where ID = " << memberID;

	query.exec();
}

void GP::operator() (mysqlpp::Connection &conn, const uint32_t memberID, const uint32_t GP)
{
	mysqlpp::Query query = conn.query();

	query << "update guild_member set GP =" << GP << " where ID = " << memberID;

	query.exec();
}

void Position::operator() (mysqlpp::Connection &conn, const uint32_t memberID, const uint8_t position)
{
    mysqlpp::Query query = conn.query();

    query << "update guild_member set Position = " << mysqlpp::quote << position
        << " where ID = " << memberID;

    query.exec();
}

void Zone::operator () (mysqlpp::Connection &conn, const uint32_t memberID, const int16_t zoneID)
{
    mysqlpp::Query query = conn.query();

    query << "update guild_member set Zone = " << zoneID << " where ID = " << memberID;

    query.exec();
}

void Authority::operator() (mysqlpp::Connection &conn, const uint32_t memberID, const uint32_t authority)
{
    mysqlpp::Query query = conn.query();

    const char *fields[] = { "JoinRights", "WithdrawRights", "UnionRights", "StorageRights", "NoticeRights" };

    query << "update guild_member set ";

    for (int i = 0; i < 5; ++i)
    {
        query << fields[i] << " = " << (authority & (1<<i));

        if ( i < 4)
            query << ", ";
    }

    query << " where ID = " << memberID;

    query.exec();
}

}   }
