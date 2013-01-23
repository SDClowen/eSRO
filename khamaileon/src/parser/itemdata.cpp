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

#include "parser/itemdata.hpp"

void ParserItemdata (const std::vector<std::string> &entries, boost::function<void (const ItemProperty&)> func)
{
    size_t idx = 0;
    ItemProperty property;

    if (atoi(entries[idx++].c_str()) == 1)
    {
        property.refID = atol(entries[idx++].c_str());

        property.refName = entries[idx++];

        /// SKIP KOREAN NAME FIELD
        idx++;

        property.refBaseName = entries[idx++];
        property.refResName = entries[idx++];
        property.refDescName = entries[idx++];

        for (int i = 0; i < ITEM_TYPE_FLAG_SIZE; ++i)
            property.Types[i] = atoi(entries[idx++].c_str());

        property.SpawnTimeout = atol(entries[idx++].c_str());
        property.Race = atoi(entries[idx++].c_str());
        property.Sox = atoi(entries[idx++].c_str());

        /// SKIP 14 UNKNOWN COLUMNS
        idx += 14;

        property.StoreFeed = atoi(entries[idx++].c_str());
        property.SellPrice = atol(entries[idx++].c_str());

        for (int i = 0 ; i < MAX_ITEM_REQUIREMENTS; ++i)
        {
            property.Requirements[i].first = atoi(entries[idx++].c_str());
            property.Requirements[i].second = atoi(entries[idx++].c_str());
        }

        property.QSP = atoi(entries[idx++].c_str());

        /// SKIP 11 FIELDS MOST OF THEM OF CONSTANT VALUE FOR ALL ENTRIES
        idx += 11;

        property.Model = entries[idx++];
        property.DropModel = entries[idx++];
        property.Icon = entries[idx++];
        property.GP = entries[idx++];

        /// SKIP 1 FIELD OF CONSTANT VALUE xxx
        idx++;

        property.MaxStack = atoi(entries[idx++].c_str());
        property.Gender = atoi(entries[idx++].c_str());

        /// SKIP 2 FIELDS OF 0 VALUES
        idx += 2;

        property.Degree = atoi(entries[idx++].c_str());
        property.SetID = atoi(entries[idx++].c_str());
        property.durability.first = atoi(entries[idx++].c_str());
        property.durability.second = atoi(entries[idx++].c_str());
        property.pd.first = strtod(entries[idx++].c_str(),NULL);
        property.pd.second = strtod(entries[idx++].c_str(),NULL);
        property.pd_lv = strtod(entries[idx++].c_str(),NULL);

        /// SKIP 3 UNKNOW FIELDS (RELATED TO ARMOR) LAST ONE IS ALWAYS 0
        idx += 3;

        property.ma.first = strtod(entries[idx++].c_str(),NULL);
        property.ma.second = strtod(entries[idx++].c_str(),NULL);
        property.ma_lv = strtod(entries[idx++].c_str(),NULL);
        property.br.first = atoi(entries[idx++].c_str());
        property.br.second = atoi(entries[idx++].c_str());
        property.md.first = strtod(entries[idx++].c_str(),NULL);
        property.md.second = strtod(entries[idx++].c_str(),NULL);
        property.md_lv = strtod(entries[idx++].c_str(),NULL);
        property.ma.first = strtod(entries[idx++].c_str(),NULL);
        property.ma.second = strtod(entries[idx++].c_str(),NULL);
        property.ma_lv = strtod(entries[idx++].c_str(),NULL);
        property.pdr = atoi(entries[idx++].c_str());
        property.pr = atoi(entries[idx++].c_str());
        property.mdr = atoi(entries[idx++].c_str());

        /// SKIP 9 UNKNOW FIELDS
        idx += 9;

        property.attk_dist = atoi(entries[idx++].c_str());
        property.min_patk.first = atoi(entries[idx++].c_str());
        property.min_patk.second = atoi(entries[idx++].c_str());
        property.max_patk.first = atoi(entries[idx++].c_str());
        property.max_patk.second = atoi(entries[idx++].c_str());
        property.patk_lv = strtod(entries[idx++].c_str(),NULL);
        property.min_matk.first = atoi(entries[idx++].c_str());
        property.min_matk.second = atoi(entries[idx++].c_str());
        property.max_matk.first= atoi(entries[idx++].c_str());
        property.max_matk.second = atoi(entries[idx++].c_str());
        property.matk_lv = strtod(entries[idx++].c_str(),NULL);
        property.min_phyref.first = atoi(entries[idx++].c_str());
        property.min_phyref.second = atoi(entries[idx++].c_str());
        property.max_phyref.first = atoi(entries[idx++].c_str());
        property.max_phyref.second = atoi(entries[idx++].c_str());
        property.min_magref.first = atoi(entries[idx++].c_str());
        property.min_magref.second = atoi(entries[idx++].c_str());
        property.max_magref.first = atoi(entries[idx++].c_str());
        property.max_magref.second = atoi(entries[idx++].c_str());
        property.ar.first = atoi(entries[idx++].c_str());
        property.ar.second = atoi(entries[idx++].c_str());

        /// SKIP EMPTY FIELDS
        idx++;

        property.crit.first = atoi(entries[idx++].c_str());
        property.crit.second = atoi(entries[idx++].c_str());

        for (int i = 0; i < MAX_ITEM_EFFECTS; ++i)
        {
            property.Effects[i].first = atoi(entries[idx++].c_str());
            property.Effects[i].second = entries[idx++];
        }

        property.MaxOpts = atoi(entries[idx++].c_str());
        property.AvatarFlag = atoi(entries[idx++].c_str());

        func(property);
    }
}
