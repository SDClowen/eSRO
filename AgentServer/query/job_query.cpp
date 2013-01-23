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

#include "job_query.hpp"

namespace DB {
namespace JOB {

bool Check::operator() (mysqlpp::Connection &conn, const std::string& alias)
{
    mysqlpp::Query query = conn.query();
    query << "select * from job where alias =" << mysqlpp::quote << alias;

    mysqlpp::StoreQueryResult res = query.store();

    return res.num_rows();
}

void Create::operator() (mysqlpp::Connection &conn, const uint32_t CharID, const Job& job)
{
    using mysqlpp::quote;

    mysqlpp::Query query = conn.query();
    query << "insert into job values ("
            << quote << CharID << ","
            << quote << job.get_alias() << ","
            << quote << (int)job.get_type() << ","
            << quote << (int)job.get_level() << ","
            << quote << job.get_experience() << ")";

    query.exec();
}

void Delete::operator() (mysqlpp::Connection &conn, const uint32_t CharID)
{
    mysqlpp::Query query = conn.query();

    query << "delete from job where CharID = " << CharID;

    query.exec();
}

Job Select::operator() (mysqlpp::Connection &conn, const uint32_t CharID)
{
    mysqlpp::Query query = conn.query();
    query << "select * from job where CharID = " << mysqlpp::quote << CharID;

    mysqlpp::StoreQueryResult res = query.store();

    Job job;
    if (res.num_rows())
    {
        job.set_alias((std::string)res[0]["alias"]);
        job.set_type(res[0]["type"]);
        job.set_level(res[0]["level"]);
        job.set_experience(res[0]["exp"]);
    }

    return job;
}

void Alias::operator() (mysqlpp::Connection &conn, const uint32_t CharID, const std::string &alias)
{
    mysqlpp::Query query = conn.query();

    query << "update job set alias = " << mysqlpp::quote << alias << " where CharID = " << CharID;

    query.exec();
}

std::vector<Job> Ranking::operator() (mysqlpp::Connection &conn, const uint8_t type)
{
    std::vector<Job> list;

    mysqlpp::Query query = conn.query();

    query << "select * from job where type = " << mysqlpp::quote << type
            << " and exp > 0 order by level desc, exp desc limit 0, 50";

    mysqlpp::StoreQueryResult res = query.store();

    Job job;
    for (size_t i = 0; i < res.num_rows(); ++i)
    {
        job.set_alias(static_cast<std::string>(res[i]["alias"]));
        job.set_level(res[i]["level"]);
        job.set_experience(res[i]["exp"]);

        list.push_back(job);
    }

    return list;
}

std::vector<Job> Weekly::operator() (mysqlpp::Connection &conn, const uint8_t type)
{
    std::vector<Job> list;

    /*mysqlpp::Query query = conn.query();

    query << "select * from job where type = " << mysqlpp::quote << type
            << " and last_contribution > 0 order by last_contribution desc limit 0,50";

    mysqlpp::StoreQueryResult res = query.store();

    Job job;
    for (size_t i = 0; i < res.num_rows(); ++i)
    {
        job.set_alias(static_cast<std::string>(res[i]["alias"]));
        job.set_level(res[i]["level"]);
        job.set_contribution(res[i]["last_contribution"]);

        list.push_back(job);
    }*/

    return list;
}

uint32_t Payment::operator() (mysqlpp::Connection &conn, const uint32_t CharID)
{
    mysqlpp::Query query = conn.query();

    query << "select * from job_payment where CharID = " << CharID;

    mysqlpp::StoreQueryResult res = query.store();

    uint32_t amount = 0;

    for (size_t i = 0; i < res.num_rows(); ++i)
        amount += static_cast<uint32_t>(res[i]["amount"]);

    return amount;
}

boost::unordered_map<uint32_t,Market> Products::operator() (mysqlpp::Connection &conn)
{
    mysqlpp::Query query = conn.query();

    query << "select * from market";

    mysqlpp::StoreQueryResult res = query.store();

    MarketStock stock;
    boost::unordered_map<uint32_t,Market> v;
    boost::unordered_map<uint32_t,Market>::iterator iter;

    for (size_t i = 0; i < res.num_rows(); ++i)
    {
        iter = v.find(res[i]["refCharID"]);

        stock.refItemID = res[i]["refItemID"];
        stock.MinProfit = res[i]["MinProfit"];
        stock.QuantitySold = res[i]["QuantitySold"];

        if (iter == v.end())
        {
            Market market;
            market.refCharID = res[i]["refCharID"];

            std::pair<boost::unordered_map<uint32_t,Market>::iterator,bool> ret;

            ret = v.insert(std::make_pair(market.refCharID,market));

            if (ret.second)
                iter = ret.first;
        }

        if (iter != v.end())
            iter->second.Chart.insert(std::make_pair(stock.refItemID,stock));
    }

    return v;
}

void Products::operator() (mysqlpp::Connection &conn, uint32_t refCharID, int32_t refItemID, uint32_t Quantity)
{
    mysqlpp::Query query = conn.query();

    query << "update market set QuantitySold = " << Quantity << " where refCharID = " << refCharID
        << " and refItemID = " << refItemID;

    query.exec();
}

void XP::operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint32_t XP)
{
    mysqlpp::Query query = conn.query();

    query << "update job set exp = " << XP << " where CharID = " << CharID;

    query.exec();
}

void Level::operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint8_t level)
{
    mysqlpp::Query query = conn.query();

    query << "update job set level = " << mysqlpp::quote << level << " where CharID = " << CharID;

    query.exec();
}

}   }
