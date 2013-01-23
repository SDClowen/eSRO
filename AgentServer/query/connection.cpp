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

#include "connection.hpp"

namespace SLES {
namespace DB {

Connection::Connection()
{}

Connection::Connection (const std::string& db, const std::string& user, const std::string& pass)
    : m_DB(db), m_User(user), m_Pass(pass)
{
}

void Connection::SetConnectionInfo(const std::string& db, const std::string& user, const std::string& pass)
{
	m_DB = db;
	m_User = user;
	m_Pass = pass;
}

bool Connection::Connect ()
{
	return m_Conn.connect(m_DB.c_str(),0,m_User.c_str(),m_Pass.c_str());
}

mysqlpp::Query Connection::query()
{
	return m_Conn.query();
}

}   }
