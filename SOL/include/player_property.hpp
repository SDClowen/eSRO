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

#ifndef __SRFL_PLAYER_PROPERTY_HPP__
#define __SRFL_PLAYER_PROPERTY_HPP__

#include <stdint.h>
#include <string>

/**
 *
 *  BASE PLAYER PROPERTIES THIS ONES ARE SAVED IN THE DB
 *
 **/

struct PlayerProperty
{
    PlayerProperty();

    uint32_t ID;            ///< Character UniqueID (only used for DB functions).
    uint32_t RefCharID;     ///< ID of the refcharacterdata used.
    std::string Name;       ///< Character name.
    uint8_t Volume;         ///< Character volumes.
    uint8_t Level;          ///< Character current level.
    uint8_t MaxLevel;       ///< Character max level achieved.
    uint16_t Str;           ///< Character base strength.
    uint16_t Int;           ///< Character base intelligence.
    uint32_t HP;            ///< Character current hit points.
    uint32_t MP;            ///< Character current mana points.
    uint32_t Stats;
    uint32_t SP;
    uint64_t Exp;
    uint16_t SPExp;
    uint64_t Gold;
    uint64_t GoldStored;    ///< Amount of gold inside the storage.
    uint8_t BskCount;
    uint8_t PK;
    uint32_t PKLevel;
    int32_t Zone;
    float X;
    float Y;
    float Z;
    uint16_t Angle;
    uint8_t Rank;
    bool GM;
    uint8_t InvSlots;
    bool InvSlotsIncreased;
    uint8_t StoreSlots;
    bool StorageSlotsIncreased;
    uint32_t GuildID;
    uint32_t AcademyID;
    uint32_t ReturnID;
    uint32_t StallAvatarID;
    uint32_t AbilityPetID;
    uint32_t AttackPetID;
    uint32_t TransportID;
    bool AutopotHPActive;
    uint8_t AutopotHPValue;
    uint8_t AutopotHPBar;
    uint8_t AutopotHPSlot;
    bool AutopotMPActive;
    uint8_t AutopotMPValue;
    uint8_t AutopotMPBar;
    uint8_t AutopotMPSlot;
    bool AutopotPillActive;
    uint8_t AutopotPillBar;
    uint8_t AutopotPillSlot;
    uint8_t AutopotDelay;
    bool ExpirationMark;
    uint32_t ExpirationTime;
};

#endif //__SRFL_PLAYER_PROPERTY_HPP__
