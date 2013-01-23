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

#ifndef __SR_GAME_SERVER_TRADE_MANAGER_HPP__
#define __SR_GAME_SERVER_TRADE_MANAGER_HPP__

#include <market_stock.hpp>
#include <mysql++/mysql++.h>
#include <boost/thread/mutex.hpp>
#include <boost/unordered_map.hpp>

class TradeManager
{
public:

    typedef boost::unordered_map<uint32_t,Market>::iterator iterator;
    typedef boost::unordered_map<uint32_t,Market>::const_iterator const_iterator;

    TradeManager (const std::string &usr, const std::string &pw, const std::string &host, const std::string &db);

    bool Initialize ();

    void Release ();

    Market FindMarket (const uint32_t refCharID) const;

    void AddQuantity (const uint32_t refCharID, const uint32_t refItemID, const uint16_t Quantity);

private:

    mutable boost::mutex m_mutex;

    std::string m_db;
    std::string m_db_host;
    std::string m_db_usr;
    std::string m_db_pw;

    mysqlpp::Connection m_db_conn;

    boost::unordered_map<uint32_t,Market> m_Markets;
};

#endif
