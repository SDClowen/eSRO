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

#include "parser/npcpos.hpp"
#include <spawn_point.hpp>
#include <stdlib.h>
#include <cmath>

bool ParserNpcpos (const std::vector<std::string> &entries, SpawnPoint *point)
{
    if (!point)
        return false;

    size_t idx = 0;

    point->refCharID = atoi(entries[idx++].c_str());
    point->Zone = atoi(entries[idx++].c_str());
    point->x = floor(strtod(entries[idx++].c_str(),NULL));
    point->z = floor(strtod(entries[idx++].c_str(),NULL));
    point->y = floor(strtod(entries[idx++].c_str(),NULL));

    return true;
}

