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

#include "trade_manager.hpp"
#include "job_query.hpp"

TradeManager::TradeManager (const std::string &usr, const std::string &pw, const std::string &host,
    const std::string &db)
    : m_db(db), m_db_host(host), m_db_usr(usr), m_db_pw(pw), m_db_conn(false)
{
    m_db_conn.connect(m_db.c_str(),m_db_host.c_str(),m_db_usr.c_str(),m_db_pw.c_str());
}

bool TradeManager::Initialize ()
{
    if (!m_db_conn.connected())
        return false;

    DB::JOB::Products query;
    m_Markets = query(m_db_conn);

    /// CALCULATE REAL PROFIT
    uint64_t max = 0;
    std::map<uint32_t,MarketStock>::iterator mit;

    for (iterator it = m_Markets.begin(); it != m_Markets.end(); ++it)
    {
        max = 0;

        for (mit = it->second.Chart.begin(); mit != it->second.Chart.end(); ++mit)
            max += mit->second.QuantitySold;

        it->second.Max = max;
    }

    return true;
}

void TradeManager::Release ()
{
}

Market TradeManager::FindMarket (const uint32_t refCharID) const
{
    Market market;
    market.refCharID = 0;

    boost::mutex::scoped_lock lock(m_mutex);

    const_iterator it = m_Markets.find(refCharID);

    if (it != m_Markets.end())
        market = it->second;

    return market;
}

void TradeManager::AddQuantity (const uint32_t refCharID, const uint32_t refItemID, const uint16_t Quantity)
{
    boost::mutex::scoped_lock lock(m_mutex);

    iterator mit = m_Markets.find(refCharID);

    if (mit != m_Markets.end())
    {
        mit->second.Max += Quantity;

        std::map<uint32_t,MarketStock>::iterator sit = mit->second.Chart.find(refItemID);

        if (sit != mit->second.Chart.end())
        {
            sit->second.QuantitySold += Quantity;

            if (!m_db_conn.connected())
                m_db_conn.connect(m_db.c_str(),m_db_host.c_str(),m_db_usr.c_str(),m_db_pw.c_str());

            DB::JOB::Products query;
            query(m_db_conn,refCharID,refItemID,Quantity);
        }
    }
}
