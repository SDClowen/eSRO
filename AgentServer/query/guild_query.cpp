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

#include "guild_query.hpp"

/// SRLF INCLUDES
#include <guild.hpp>
#include <guild_member.hpp>

using namespace DB::GUILD;

uint32_t Create::operator() (mysqlpp::Connection& conn, const std::string& name)
{
	mysqlpp::Query query = conn.query();

	query << "insert into guild (Name) values (" << mysqlpp::quote << name << ")";

	query.exec();

	return query.insert_id();
}

bool Valid::operator() (mysqlpp::Connection& conn, const std::string& name)
{
	mysqlpp::Query query = conn.query();

	query << "select * from guild where Name = " << mysqlpp::quote << name;

	mysqlpp::StoreQueryResult res = query.store();

	return !res.num_rows();
}

void Delete::operator() (mysqlpp::Connection& conn, const uint32_t guildID)
{
    /// TODO: CREATE A STORED PRODECURE!!
    /// DELETE THE GUILD AND GUILD MEMBER INFORMATION AND SET GuildID OF CHARACTERS TO 0.
    /// DELETE ALL ITEMS FROM GUILD STORAGE

	mysqlpp::Query query = conn.query();

	query << "delete from guild where ID = " << guildID;

	query.exec();

	query.reset();

	query << "delete from guild_member where GuildID = " << guildID;

	query.exec();

	query.reset();

	query << "update characters set GuildID = 0 where GuildID = " << guildID;

	query.exec();

	///TODO DELETE ALL ITEMS FROM GUILD STORAGE ALSO!!
}

boost::shared_ptr<Guild> Select::operator() (mysqlpp::Connection& conn, const uint32_t guildID)
{
	mysqlpp::Query query = conn.query();

	query << "select * from guild where ID = " << guildID;

	mysqlpp::StoreQueryResult res = query.store();

    boost::shared_ptr<Guild> guild;

	if (res.num_rows())
	{
	    guild = boost::shared_ptr<Guild>(new Guild(guildID,static_cast<std::string>(res[0]["Name"]),
            res[0]["Level"],res[0]["StorageCapacity"]));

        if (guild)
        {
            guild->set_gp(res[0]["GP"]);
            guild->set_notice(static_cast<std::string>(res[0]["Title"]),static_cast<std::string>(res[0]["Msg"]));
            guild->set_union_id(res[0]["UnionID"]);
            guild->set_stored_gold(res[0]["StoredGold"]);

            query.reset();

            query << "select * from guild_member where GuildID = " << guildID;

            res = query.store();

            boost::shared_ptr<GuildMember> member;
            const char *fields[] = { "JoinRights", "WithdrawRights", "UnionRights", "StorageRights", "NoticeRights" };

            for ( size_t i = 0; i < res.num_rows(); ++i)
            {
                member.reset(new GuildMember());

                if (member)
                {
                    member->ID = res[i]["ID"];
                    member->GuildID = guild->get_id();
                    member->Name = static_cast<std::string>(res[i]["Name"]);
                    member->GrantName = (std::string)res[i]["GrantName"];
                    member->Level = res[i]["Level"];
                    member->GP = res[i]["GP"];
                    member->CombatPoints = res[i]["CombatPoints"];
                    member->Master = res[i]["Master"];
                    member->refCharID = res[i]["refCharID"];
                    member->Position = res[i]["Position"];
                    member->Zone = res[i]["Zone"];
                    member->Status = true;

                    for ( int j = 0; j < 5; ++j)
                        member->Rights[j] = static_cast<bool>(res[i][fields[j]]);

                    guild->insert(member);
                }
            }
        }
	}

	return guild;
}

void Notice::operator() (mysqlpp::Connection& conn, const uint32_t guildID, const std::string& title,
    const std::string& notice)
{
	mysqlpp::Query query = conn.query();

	query << "update guild set Title = " << mysqlpp::quote << title << ","
		<< "Msg = " << mysqlpp::quote << notice << " "
		<< "where ID = " << guildID;

	query.exec();
}

void GP::operator() (mysqlpp::Connection& conn, const uint32_t guildID, uint32_t gp)
{
	mysqlpp::Query query = conn.query();

	query << "update guild set GP = " << gp << " where ID = " << guildID;

	query.exec();
}

void Level::operator() (mysqlpp::Connection& conn, const uint32_t guildID, uint8_t lv)
{
	mysqlpp::Query query = conn.query();

	query << "update guild set Level = " << mysqlpp::quote << lv << " where ID = " << guildID;

	query.exec();
}

void Gold::operator() (mysqlpp::Connection &conn, const uint32_t guildID, const uint32_t gold)
{
	mysqlpp::Query query = conn.query();

	query << "update guild set StoredGold = " << gold << " where ID = " << guildID;

	query.exec();
}

void Union::operator() (mysqlpp::Connection &conn, const uint32_t guildID, const uint32_t unionID)
{
	mysqlpp::Query query = conn.query();

	query << "update guild set UnionID = " << unionID << " where ID = " << guildID;

	query.exec();
}

void StorageCapacity::operator() (mysqlpp::Connection &conn, const uint32_t guildID, const uint8_t capacity)
{
    mysqlpp::Query query = conn.query();

    query << "update guild set StorageCapacity = " << capacity << " where ID = " << guildID;

    query.exec();
}

std::map<uint8_t,std::pair<uint16_t,uint16_t> > PromotePolicy::operator() (mysqlpp::Connection &conn)
{
    mysqlpp::Query query = conn.query();

    query << "select * from refguildpromotepolicy";

    mysqlpp::StoreQueryResult res = query.store();

    std::map<uint8_t,std::pair<uint16_t,uint16_t> > v;

    for (size_t i = 0; i < res.num_rows(); ++i)
        v.insert(std::make_pair(res[i]["Level"],std::make_pair(res[i]["Gold"],res[i]["GP"])));

    return v;
}
