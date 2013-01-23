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

#include "parser/leveldata.hpp"
#include <stdlib.h>

void ParserLeveldata (const std::vector<std::string> &entries, boost::function<void (const LevelProperty&)> func)
{
    size_t idx = 0;
    LevelProperty property;

    property.Level = atoi(entries[idx++].c_str());
    property.RequiredXP = atol(entries[idx++].c_str());
    property.RequiredSP = atoi(entries[idx++].c_str());

    /// SKIP 2 O FIELDS
    idx +=2;

    property.RequiredJobXP = atol(entries[idx++].c_str());

    /// NEXT 2 COLUMNS ARE THE SAME XP FOR JOB

    func(property);
}

