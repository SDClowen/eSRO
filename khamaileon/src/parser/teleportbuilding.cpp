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

#include "parser/teleportbuilding.hpp"
#include <stdlib.h>

void ParserTeleportBuilding (const std::vector<std::string> &entries,
    boost::function<void (const BuildingProperty&)> func)
{
    size_t idx = 0;
    BuildingProperty property;

    if (atoi(entries[idx++].c_str()) != 1)
        return;

    property.refID = atoi(entries[idx++].c_str());
    property.refName = entries[idx++];

    /// SKIP KOREAN NAME AND xxx COLUMN
    idx +=2;

    property.refResName = entries[idx++];

    /// SKIP 35 UNKNOWN COLUMNS
    idx += 35;

    property.Zone = atoi(entries[idx++].c_str());

    /// SKIP EMPTY COLUMN
    idx++;

    property.x = atoi(entries[idx++].c_str());
    property.z = atoi(entries[idx++].c_str());
    property.y = atoi(entries[idx++].c_str());

    /// SKIP 6 UNKNOWN COLUMNS
    idx += 6;

    property.Model = entries[idx++];

    /// SKIP 2 UNKNOWN COLUMNS
    idx += 2;

    property.TaxFortress = entries[idx++];
    property.OwnerFortress = entries[idx++];

    /// LAST COLUMN 0

    func(property);
}
