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

#ifndef __SRLF_SKILL_PROPERTY_HPP__
#define __SRLF_SKILL_PROPERTY_HPP__

#include <stdint.h>
#include <boost/array.hpp>

#define MAX_SKILL_PREREQUISITE_SKILLS 2
#define MAX_SKILL_REQUIRED_WEAPONS 2
#define MAX_SKILL_EFFECTS 48

struct SkillProperty
{
    uint32_t refID;
    uint32_t GroupID;
    std::string refName;
    std::string refGroupName;
    uint32_t refBaseID;
    uint8_t Level;
    uint8_t Type;
    uint32_t NextSkill;
    uint16_t InitTime;
    uint16_t DelayTime;
    uint16_t NextTime;
    uint32_t Cooldown;
    uint16_t MastID;
    uint8_t MastLv;
    std::pair<uint32_t,uint8_t> Skills[MAX_SKILL_PREREQUISITE_SKILLS];
    uint16_t SP;
    uint8_t Race;
    uint8_t Weapons[MAX_SKILL_REQUIRED_WEAPONS];
    uint16_t MP;
    uint8_t MinHPRatio;
    std::string Icon;
    std::string refResName;
    std::string refDesc;
    std::string refStudy;
    uint8_t MaxHPRatio;
    boost::array<int32_t,MAX_SKILL_EFFECTS> Effects;
};

#endif
