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

#include "parser/teleportoptional.hpp"
#include <stdlib.h>

void ParserOptionalTeleport (const std::vector<std::string> &entries,
    boost::function<void (const TeleportOptional&)> func)
{
    size_t idx = 0;
    TeleportOptional teleport;

    if (atoi(entries[idx++].c_str()) == 1)
    {
        teleport.id = atoi(entries[idx++].c_str());

        /// SKIP KOREAN NAME AND ZONE RESOURCE NAME
        idx +=2;

        teleport.zone = atoi(entries[idx++].c_str());
        teleport.x = atoi(entries[idx++].c_str());
        teleport.z = atoi(entries[idx++].c_str());
        teleport.y = atoi(entries[idx++].c_str());

        /// SKIP COLUMN OF CONSTANT VALUE 1
        idx++;

        teleport.location = atoi(entries[idx++].c_str());
        teleport.free = atoi(entries[idx++].c_str());
        teleport.minlv = atoi(entries[idx++].c_str());
        teleport.maxlv = atoi(entries[idx++].c_str());

        /// LAST 6 COLUMNS ARE GARBAGE

        func(teleport);
    }
}
