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

#include "parser/skilldata.hpp"

void ParserSkilldata (const std::vector<std::string> &entries, boost::function<void (const SkillProperty&)> func)
{
    size_t idx = 0;
    SkillProperty property;

    if (atoi(entries[idx++].c_str()) == 1)
    {
        property.refID = atol(entries[idx++].c_str());
        property.GroupID = atoi(entries[idx++].c_str());
        property.refName = entries[idx++];

        /// SKIP KOREAN NAME
        idx++;

        property.refGroupName = entries[idx++];
        property.refBaseID = atol(entries[idx++].c_str());
        property.Level = atoi(entries[idx++].c_str());
        property.Type = atoi(entries[idx++].c_str());
        property.NextSkill = atol(entries[idx++].c_str());

        /// SKIP UNKNOWN FIELD WITH VALUES OF (0 or 99999999)
        idx++;

        property.InitTime = atoi(entries[idx++].c_str());
        property.DelayTime = atoi(entries[idx++].c_str());
        property.NextTime = atoi(entries[idx++].c_str());
        property.Cooldown = atol(entries[idx++].c_str());

        /// SKIP 19 UNKNOWN FIELDS
        idx += 19;

        property.MastID = atoi(entries[idx++].c_str());

        /// SKIP EMPTY FIELD
        idx++;

        property.MastLv = atoi(entries[idx++].c_str());

        /// SKIP 3 EMPTY FIELDS
        idx += 3;

        for (int i = 0; i < MAX_SKILL_PREREQUISITE_SKILLS; ++i)
            property.Skills[i].first = atol(entries[idx++].c_str());

        /// SKIP EMPTY FIELD
        idx++;

        for (int i = 0; i < MAX_SKILL_PREREQUISITE_SKILLS; ++i)
            property.Skills[i].second = atoi(entries[idx++].c_str());

        /// SKIP EMPTY FIELD
        idx++;

        property.SP = atoi(entries[idx++].c_str());
        property.Race = atoi(entries[idx++].c_str());

        /// SKIP 2 EMPTY FIELDS
        idx += 2;

        for (int i = 0; i < MAX_SKILL_REQUIRED_WEAPONS; ++i)
            property.Weapons[i] = atoi(entries[idx++].c_str());

        /// SKIP UNKNOWN FIELD RELATED TO BARD SKILL THAT EXCHANGES HP TO MP
        idx++;

        property.MP = atoi(entries[idx++].c_str());
        property.MinHPRatio = atoi(entries[idx++].c_str());

        /// SKIP 6 UNKNOWN FIELDS
        idx += 6;

        property.Icon = entries[idx++];
        property.refResName = entries[idx++];

        /// SKIP FIELD OF COLUMN WITH CONSTANT VALUE xxx
        idx++;

        property.refDesc = entries[idx++];
        property.refStudy = entries[idx++];
        property.MaxHPRatio = atoi(entries[idx++].c_str());

        /// SKIP 2 UNKNOWN FIELDS
        idx += 2;

        for (int i = 0; i < MAX_SKILL_EFFECTS; ++i)
            property.Effects[i] = atol(entries[idx++].c_str());

        func(property);
    }
}
