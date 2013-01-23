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

#include "hotkeys_query.hpp"

using namespace DB::HOTKEY;

void Insert::operator() (mysqlpp::Connection& conn, const uint32_t CharID, const Hotkey& hotkey)
{
	mysqlpp::Query query = conn.query();

	query << "insert into hotkeys values ("
		<< mysqlpp::quote << CharID << ","
		<< mysqlpp::quote << hotkey.pos << ","
		<< mysqlpp::quote << hotkey.type << ","
		<< mysqlpp::quote << (mysqlpp::sql_int)hotkey.id << ")";

	query.exec();
}

void Remove::operator() (mysqlpp::Connection &conn, const uint32_t CharID)
{
    mysqlpp::Query query = conn.query();

	query << "delete from hotkeys where CharID = " << CharID;

	query.exec();
}

void Remove::operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint8_t pos)
{
	mysqlpp::Query query = conn.query();

	query << "delete from hotkeys where CharID = " << CharID
		<< " and pos = " << mysqlpp::quote << pos;

	query.exec();
}

std::vector<Hotkey> Select::operator() (mysqlpp::Connection& conn, const uint32_t CharID)
{
	mysqlpp::Query query = conn.query();

	query << "select * from hotkeys where CharID = " << CharID;

	mysqlpp::StoreQueryResult res = query.store();

	std::vector<Hotkey> v;
	Hotkey hotkey;
	for (size_t i = 0; i < res.num_rows(); ++i)
	{
		hotkey.pos = res[i]["pos"];
		hotkey.type = res[i]["type"];
		hotkey.id = (mysqlpp::sql_int)res[i]["id"];

		v.push_back(hotkey);
	}

	return v;
}
