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

#include "parser/refpricepolicyofitem.hpp"
#include <stdlib.h>

void ParserRefpricepolicyofitem (const std::vector<std::string> &entries,
    boost::function<void (const ItemPricePolicy_t&)> func)
{
    size_t idx = 0;
    ItemPricePolicy_t policy;

    if (atoi(entries[idx++].c_str()) != 1)
        return;

    if (atoi(entries[idx++].c_str()) != 4)
        return;

    policy.refPKName = entries[idx++];
    policy.Type = atoi(entries[idx++].c_str());

    /// SKIP 0 COLUMN
    idx++;

    policy.Price = atol(entries[idx++].c_str());

    /// LAST 8 COLUMNS ARE EMPTY

    func(policy);
}
