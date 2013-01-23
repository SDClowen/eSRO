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

#include "parser/teleportdata.hpp"
#include "teleport.hpp"
#include <stdlib.h>

bool ParserTeleportdata (const std::vector<std::string> &entries, Teleport *teleport)
{
    if (!teleport)
        return false;

    size_t idx = 0;

    if (atoi(entries[idx++].c_str()) != 1)
        return false;

    teleport->ID = atoi(entries[idx++].c_str());
    teleport->refName = entries[idx++];
    teleport->AssignedID = atoi(entries[idx++].c_str());
    teleport->refZone = entries[idx++];
    teleport->Zone = atoi(entries[idx++].c_str());
    teleport->x = atoi(entries[idx++].c_str());
    teleport->z = atoi(entries[idx++].c_str());
    teleport->y = atoi(entries[idx++].c_str());

    /// LAST 4 COLUMNS UNKNOWN

    return true;
}
