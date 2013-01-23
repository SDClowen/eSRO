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

#include "message_query.hpp"

namespace DB {
namespace MESSAGE {

bool Valid::operator() (SLES::DB::Connection &conn, const std::string &name)
{
	mysqlpp::Query query = conn.query();

	query << "select Name from player where Name = " << mysqlpp::quote << name;

	mysqlpp::StoreQueryResult res = query.store();

	return res.num_rows() > 0;
}

bool Max::operator() (SLES::DB::Connection &conn, const std::string &name)
{
	mysqlpp::Query query = conn.query();

	query << "select * from offline_msgs where name = " << mysqlpp::quote << name;

	mysqlpp::StoreQueryResult res = query.store();

	return res.num_rows() == 20;
}

void Insert::operator() (SLES::DB::Connection &conn, const std::string &name, const std::string &sender,
	const std::string &msg, const uint32_t model)
{
	mysqlpp::Query query = conn.query();

	query << "insert into offline_msgs values ("
		<< mysqlpp::quote << name << "," << mysqlpp::quote << sender << "," << mysqlpp::quote << msg << ","
		<< model;

	query.exec();
}

std::string Select::operator() (SLES::DB::Connection &conn, const std::string &name, const uint8_t msg_number)
{
	mysqlpp::Query query = conn.query();

	query << "select * from offline_msgs where name = " << mysqlpp::quote << name;

	mysqlpp::StoreQueryResult res = query.store();

	std::string msg;

	if (res.num_rows() > msg_number)
		msg = static_cast<std::string>(res[msg_number]["msg"]);

	return msg;
}

void Delete::operator() (SLES::DB::Connection &conn, const std::string &name, const uint8_t msg_number)
{
	mysqlpp::Query query = conn.query();

	query << "select * from offline_msgs where name = " << mysqlpp::quote << name;

	mysqlpp::StoreQueryResult res = query.store();

	if (res.num_rows() > msg_number)
	{
		query.reset();

		query << "delete from offline_msgs where ID = " << res[msg_number]["ID"];

		query.exec();
	}
}

}	}
