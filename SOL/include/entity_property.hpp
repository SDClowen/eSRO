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

#ifndef __SRLF_CHARACTER_PROPERTY_HPP__
#define __SRLF_CHARACTER_PROPERTY_HPP__

#include "entity_constants.hpp"

#include <stdint.h>
#include <string>
#include <boost/array.hpp>

#define MAX_CHARACTER_TYPE 6
#define MAX_CHARACTER_BASE_SKILL 10
#define MAX_ALLOW_ATTRIBUTES 11
#define MAX_CHARACTER_REQUERIMENTS 4

struct EntityProperty
{
    inline bool operator == (const EntityProperty &pr) const
    {
        return refID == pr.refID;
    }

    ENTITY_TYPE type () const;

    uint32_t refID;                 /// UNIQUE IDENTIFIER
    std::string refName;            /// REFERENCE NAME
    std::string refBaseName;        /// BASE REFERENCE NAME
    std::string refResName;         /// RESOURCE NAME
    std::string refNextName;        /// REFERENCE NAME OF NEXT ENTITY AFTER LEVEL UP
    boost::array<int8_t,MAX_CHARACTER_TYPE> type_list;
    uint32_t Scale;                 /// MODEL SCALE
    uint8_t Race;
    uint8_t Quality;
    bool AllowAttr[MAX_ALLOW_ATTRIBUTES];
    uint32_t StructureCost;
    uint16_t MaxCapacity;
    std::pair<int8_t,uint8_t> Requirements[4];
    int16_t GroupID;                /// GROUP IDENTIFIER
    int16_t UnkFields[5];
    double WalkSpeed;
    double RunSpeed;
    double BersekSpeed;
    uint8_t Unk2Fields[3];
    std::string Model;
    std::string Icon;
    std::string Fortress;
    uint8_t Level;
    uint8_t Gender;
    uint32_t HP;
    uint8_t UnkFlag;
    uint16_t PhysicalDefense;
    uint16_t MagicalDefense;
    uint16_t ParryRatio;
    uint8_t BlockRatio;
    uint16_t HitRatio;
    uint8_t AbsorbRatio;
    uint32_t XP;
    boost::array<uint32_t,MAX_CHARACTER_BASE_SKILL> skill_list;
    uint8_t SummonLevel;
};

#endif
