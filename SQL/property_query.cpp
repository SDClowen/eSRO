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

#include "property_query.hpp"
#include <entity_property.hpp>

using namespace DB::PROPERTY;

std::multimap<uint32_t,std::pair<uint8_t,uint8_t> > CharacterQuality::operator() (mysqlpp::Connection &conn)
{
    mysqlpp::Query query = conn.query();

    query << "select * from refcharacter_quality";

    mysqlpp::StoreQueryResult res = query.store();

    std::multimap<uint32_t,std::pair<uint8_t,uint8_t> > v;

    for (size_t i = 0; i < res.num_rows(); ++i)
        v.insert(std::make_pair(res[i]["refCharID"],std::make_pair(res[i]["Quality"],res[i]["Weight"])));

    return v;
}

std::multimap<uint32_t,NPCTask> NPC::operator() (mysqlpp::Connection &conn)
{
    mysqlpp::Query query = conn.query();

    query << "select refCharID,TaskID,MinLv from reftask";

    mysqlpp::StoreQueryResult res = query.store();

    NPCTask task;
    std::multimap<uint32_t,NPCTask> v;

    for (size_t i = 0; i < res.num_rows(); ++i)
    {
        task.ID = res[i]["TaskID"];
        task.MinLv = res[i]["MinLv"];

        v.insert(std::make_pair(res[i]["refCharID"],task));
    }

    return v;
}

std::vector<MagicOptionProperty> MagicOption::operator() (mysqlpp::Connection &conn)
{
    mysqlpp::Query query = conn.query();

    query << "select * from refmagicoption";

    mysqlpp::StoreQueryResult vres;
    mysqlpp::StoreQueryResult res = query.store();

    const char *fields[] = { "Req1", "Req2", "Req3", "Req4" };

    MagicOptionProperty pr;
    MagicOptionValue value;
    std::vector<MagicOptionProperty> v;

    for (size_t i = 0; i < res.num_rows(); ++i)
    {
        pr.ID = res[i]["ID"];
        pr.Name = static_cast<std::string>(res[i]["Name"]);
        pr.DecValue = res[i]["DecValue"];
        pr.Degree = res[i]["Degree"];

        for (int j = 0; j < MAGIC_OPT_MAX_REQ_COUNT; ++j)
            pr.Requirement[j] = static_cast<std::string>(res[i][fields[j]]);

        /// READ THE AVAILABLE VALUES FOR THE OPTION
        query.reset();
        pr.Values.clear();

        query << "select * from refmagicoption_values where ID = " << pr.ID;

        vres = query.store();

        for (size_t j = 0; j < vres.num_rows(); ++j)
        {
            value.Val = vres[j]["Value"];
            value.Probability = vres[j]["Probability"];

            pr.Values.push_back(value);
        }

        v.push_back(pr);
    }

    return v;
}
