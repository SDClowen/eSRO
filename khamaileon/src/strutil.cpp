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

#include "strutil.hpp"

std::vector<std::string> split (const std::string &line, const char delim)
{
    std::vector<std::string> entries;

    size_t beg = 0, end = 0;
    do
    {
        end = line.find_first_of(delim,beg+1);

        entries.push_back(line.substr(beg,end-beg));

        beg = end +1;

    } while (end != std::string::npos);

    return entries;
}
