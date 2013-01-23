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

#include "parser/characterdata.hpp"
#include <entity_property.hpp>

bool ParserCharacterdata (const std::vector<std::string> &entries, EntityProperty *property)
{
    assert(property);

    size_t idx = 0;

    if (atoi(entries[idx++].c_str()) != 1)
        return false;

    property->refID = atol(entries[idx++].c_str());
    property->refName = entries[idx++];

    /// SKIP KOREAN NAME FIELD
    idx++;

    property->refBaseName = entries[idx++];
    property->refResName = entries[idx++];
    property->refNextName = entries[idx++];

    for (int i = 0; i < MAX_CHARACTER_TYPE; ++i)
        property->type_list[i] = atoi(entries[idx++].c_str());

    property->Scale = std::atol(entries[idx++].c_str());
    property->Race = atoi(entries[idx++].c_str());
    property->Quality = atoi(entries[idx++].c_str());

    for (int i = 0; i < MAX_ALLOW_ATTRIBUTES; ++i)
        property->AllowAttr[i] = atoi(entries[idx++].c_str());

    property->StructureCost = atol(entries[idx++].c_str());

    /// SKIP 3 FIELDS THAT ARE ALWAYS 0
    idx += 3;

    property->MaxCapacity = atoi(entries[idx++].c_str());

    for (int i = 0; i < MAX_CHARACTER_REQUERIMENTS; ++i)
    {
        property->Requirements[i].first = atoi(entries[idx++].c_str());
        property->Requirements[i].second = atoi(entries[idx++].c_str());
    }

    property->GroupID = atoi(entries[idx++].c_str());

    for (int i = 0; i < 5; ++i)
        property->UnkFields[i] = atoi(entries[idx++].c_str());

    property->WalkSpeed = atoi(entries[idx++].c_str());
    property->RunSpeed = atoi(entries[idx++].c_str());
    property->BersekSpeed = atoi(entries[idx++].c_str());

    for (int i = 0; i < 3; ++i)
        property->Unk2Fields[i] = atoi(entries[idx++].c_str());

    property->Model = entries[idx++];

    /// SKIP EMPTY FIELD THAT IS ALWAYS xxx
    idx++;

    property->Icon = entries[idx++];
    property->Fortress = entries[idx++];

    /// SKIP EMPTY FIELD THAT IS ALWAYS xxx
    idx++;

    property->Level = atoi(entries[idx++].c_str());
    property->Gender = atoi(entries[idx++].c_str());
    property->HP = atol(entries[idx++].c_str());

    /// SKIP 3 FIELDS THAT ARE ALWAYS 0
    idx++;

    property->UnkFlag = atoi(entries[idx++].c_str());

    /// SKIP 9 FIELDS THAT ARE ALWAYS 0
    idx += 9;

    property->PhysicalDefense = atoi(entries[idx++].c_str());
    property->MagicalDefense = atoi(entries[idx++].c_str());

    /// SKIP 2 FIELDS THAT ARE REPEATED ENTRIES OF LEVEL
    idx += 2;

    property->ParryRatio = atoi(entries[idx++].c_str());
    property->BlockRatio = atoi(entries[idx++].c_str());
    property->HitRatio = atoi(entries[idx++].c_str());
    property->AbsorbRatio = atoi(entries[idx++].c_str());
    property->XP = atol(entries[idx++].c_str());

    /// SKIP 3 FIELDS THAT ARE UNKNOWN
    idx += 3;

    for (int i = 0; i < MAX_CHARACTER_BASE_SKILL; ++i)
        property->skill_list[i] = atol(entries[idx++].c_str());

    /// SKIP 4 FIELDS THAT ARE UNKNOWN
    idx += 4;

    property->SummonLevel = atoi(entries[idx++].c_str());

    /// SKIP 6 FIELDS THAT ARE UNKNOWN
    idx += 6;

    return true;
}

