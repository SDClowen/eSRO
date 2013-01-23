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

#ifndef __SLSE_DATABASE_CONNECTION_HPP__
#define __SLSE_DATABASE_CONNECTION_HPP__

#include <mysql++/mysql++.h>
#include <mysql++/result.h>
#include <string>

namespace SLES {
namespace DB {

class Connection
{
public:

	Connection();

	Connection (const std::string& db, const std::string& user, const std::string& pass);

	void SetConnectionInfo(const std::string& db, const std::string &user, const std::string &pass);

	bool Connect();

	mysqlpp::Query query();

protected:

    std::string m_DB;
	std::string m_User;
	std::string m_Pass;

	mysqlpp::Connection m_Conn;
};

}   }

#endif //__SLSE_DATABASE_CONNECTION_HPP__
