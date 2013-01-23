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

#include "parser/siegefortress.hpp"

void ParserSiegefortress (const std::vector<std::string> &entries, boost::function<void (const FortressProperty&)> func)
{
    size_t idx = 0;
    FortressProperty property;

    if (atoi(entries[idx++].c_str()) != 1)
        return;

    property.refID = atoi(entries[idx++].c_str());
    property.refName = entries[idx++];

    /// SKIP KOREAN NAME
    idx++;

    property.refResName = entries[idx++];
    property.refBuilding = entries[idx++];
    property.TypeFlag = atoi(entries[idx++].c_str());
    property.Capacity = atoi(entries[idx++].c_str());

    /// SKIP 3 UNKNOWN COLUMNS
    idx += 3;

    property.Feed = atol(entries[idx++].c_str());
    property.Icon = entries[idx++];
    property.refNPC = entries[idx++];

    func(property);
}
