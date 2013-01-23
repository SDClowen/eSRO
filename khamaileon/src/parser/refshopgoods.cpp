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

#include "parser/refshopgoods.hpp"
#include <stdlib.h>

void ParserRefshopgoods (const std::vector<std::string> &entries, boost::function<void (const ShopGood_t&)> func)
{
    size_t idx = 0;
    ShopGood_t good;

    if (atoi(entries[idx++].c_str()) != 1)
        return;

    if (atoi(entries[idx++].c_str()) != 4)
        return;

    good.refTab = entries[idx++];
    good.refPKItem = entries[idx++];
    good.Position = atoi(entries[idx++].c_str());

    /// LAST 8 COLUMNS ARE EMPTY

    func(good);
}
