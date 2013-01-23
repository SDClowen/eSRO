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

#ifndef __SRDL_JOB_QUERY_HPP__
#define __SRDL_JOB_QUERY_HPP__

#include "connection.hpp"

#include <job.hpp>
#include <market_stock.hpp>
#include <boost/unordered_map.hpp>

namespace DB {
namespace JOB {

class Check
{
public:

    /**
     *
     *  @brief Check if the alias is available.
     *  @param conn Database connection.
     *  @param alias Job alias to check.
     *  @return if the job alias is available or not.
     *
     */

    bool operator() (mysqlpp::Connection &conn, const std::string& alias);
};

class Create
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t CharID, const Job& job);
};

class Delete
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t CharID);
};

class Select
{
public:

    Job operator() (mysqlpp::Connection &conn, const uint32_t CharID);
};

class Alias
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t CharID, const std::string &alias);
};

class Ranking
{
public:

    std::vector<Job> operator() (mysqlpp::Connection &conn, const uint8_t type);
};

class Weekly
{
public:

    std::vector<Job> operator() (mysqlpp::Connection &conn, const uint8_t type);
};

class Payment
{
public:

    uint32_t operator() (mysqlpp::Connection &conn, const uint32_t CharID);
};

class Products
{
public:

    boost::unordered_map<uint32_t,Market> operator() (mysqlpp::Connection &conn);

    void operator() (mysqlpp::Connection &conn, uint32_t refCharID, int32_t refItemID, uint32_t Quantity);
};

class XP
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint32_t XP);
};

class Level
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint8_t level);
};

}   }

#endif //__SRDL_JOB_QUERY_HPP__
