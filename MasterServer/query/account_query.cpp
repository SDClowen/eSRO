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

#include "account_query.hpp"

namespace DB {
namespace Account {

LOGIN_RET Login::operator () (mysqlpp::Connection *conn, const std::string &usr, const std::string &pw,
    const std::string &IP)
{
    mysqlpp::Query query = conn->query();

    query << "select * from accounts where username = " << mysqlpp::quote << usr
        << " and ban_expiration_date > now()";

    mysqlpp::StoreQueryResult res = query.store();

    if (res.num_rows())
        return LOGIN_BANNED;

    query.reset();

    Valid valid_query;

    if (valid_query(conn,usr,pw))
    {
        IsLogged is_logged_query;

        if (is_logged_query(conn,usr))
            return LOGIN_ALREDY_LOGGED;

        query << "update accounts set last_login_ip = " << mysqlpp::quote << IP
            << " where username = " << mysqlpp::quote << usr;

        query.exec();
    }
    else
    {
        query << "select * from accounts where username = " << mysqlpp::quote << usr;

        mysqlpp::StoreQueryResult res = query.store();

        if (res.num_rows())
        {
            int count = res[0]["login_attempt"];

            ++count;

            if (count >= 3) ///CHECK IF THE ATTEMPTS IS THE MAX AVAILABLE
            {
                query.reset();

                query << "update set ban_expiration_date = NOW() + INTERVAL 10 MINUTES "
                    << " where username = " << mysqlpp::quote << usr;

                query.exec();

                return LOGIN_MAX_ATTEMPTS;
            }

            query.reset();

            query << "update accounts set login_attempt = " << count
                << " where username = " << mysqlpp::quote << usr;

            query.exec();
        }

        return LOGIN_REJECTED;
    }

    return LOGIN_ACCEPTED;
}

bool Valid::operator() (mysqlpp::Connection *conn, const std::string& user, const std::string& pass)
{
	mysqlpp::Query query = conn->query();

	query << "select * from accounts where username = " << mysqlpp::quote
		<< user << " and password = " << mysqlpp::quote << pass;

	mysqlpp::StoreQueryResult res = query.store();

	return res.num_rows() > 0;
}

bool IsLogged::operator() (mysqlpp::Connection *conn, const std::string& name)
{
	mysqlpp::Query query = conn->query();
	query << "select * from accounts where username = " << mysqlpp::quote << name;

	mysqlpp::StoreQueryResult res = query.store();

	bool logged = false;

	if (res.num_rows())
		logged = res[0]["state"];

	return logged;
}

void SetLogged::operator() (mysqlpp::Connection *conn, const uint32_t accountID, bool status)
{
	mysqlpp::Query query = conn->query();

	query << "update accounts set state = " << status << " "
		<< "where id = " << mysqlpp::quote << accountID;

	query.exec();
}

void Clear::operator() (mysqlpp::Connection *conn)
{
	mysqlpp::Query query = conn->query();

	query << "update accounts set state = 0";

	query.exec();
}

uint32_t Silk::operator() (mysqlpp::Connection *conn, const uint32_t accountID)
{
	uint32_t amount = 0;

	mysqlpp::Query query = conn->query();

	query << "select silk from accounts where id = " << mysqlpp::quote << accountID;

	mysqlpp::StoreQueryResult res = query.store();

	if (res && res.num_rows())
		amount = res[0]["silk"];

	return amount;
}

void Silk::operator() (mysqlpp::Connection *conn, const uint32_t accountID, const uint32_t silk)
{
	mysqlpp::Query query = conn->query();

	query << "update accounts set silk= " << silk << " where id = " << mysqlpp::quote << accountID;

	query.exec();
}

int32_t Attempt::operator () (mysqlpp::Connection *conn, const std::string &usr)
{
    mysqlpp::Query query = conn->query();

    query << "select * from accounts where username = " << mysqlpp::quote << usr;

    mysqlpp::StoreQueryResult res = query.store();

    int count = -1;

    if (res.num_rows())
    {
        count = res[0]["login_attempt"];
    }

    return count;
}

mysqlpp::DateTime BanExpirationDate::operator () (mysqlpp::Connection *conn, const std::string &usr)
{
    mysqlpp::Query query = conn->query();

    query << "select * from accounts where username = " << mysqlpp::quote << usr;

    mysqlpp::StoreQueryResult res = query.store();

    mysqlpp::DateTime dt;

    if (res.num_rows())
    {
        dt = res[0]["ban_expiration_date"];
    }

    return dt;
}

Ext::return_type Ext::operator() (mysqlpp::Connection *conn, const std::string &usr)
{
    return_type ext = boost::make_tuple(0,0,0,0);

    mysqlpp::Query query = conn->query();

    query << "select id,access_level,security_level,silk from accounts where username = "
        << mysqlpp::quote << usr;

    mysqlpp::StoreQueryResult res = query.store();

    if (res.num_rows())
    {
        ext.get<0>() = res[0]["id"];
        ext.get<1>() = res[0]["access_level"];
        ext.get<2>() = res[0]["security_level"];
        ext.get<3>() = res[0]["silk"];
    }

    return ext;
}

}   }
