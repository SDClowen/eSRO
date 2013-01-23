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

#ifndef __SLES_GAME_SERVER_PLAYER_PACKET_HPP__
#define __SLES_GAME_SERVER_PLAYER_PACKET_HPP__

#include <hotkey.hpp>
#include <autopot.hpp>
#include <boost/shared_ptr.hpp>

class OPacket;
class Player;

namespace srv_pkt
{

enum PLAYER_EXTRA_STATS
{
    PLAYER_EXTRA_STATS_GOLD = 1,
    PLAYER_EXTRA_STATS_SP,
    PLAYER_EXTRA_STATS_BERSERK_GAUGE = 4
};

enum PLAYER_ACTION_TYPE
{
    PLAYER_ACTION_INIT = 0x0101,
    PLAYER_ACTION_END = 2,
    PLAYER_ACTION_ERROR = 3
};

enum PLAYER_DEATH_TYPE
{
    PLAYER_DEATH_CAPE = 0x02,
    PLAYER_DEATH_NORMAL = 0x04,
    PLAYER_DEATH_FORTRESS
};

void InitScreen (const boost::shared_ptr<OPacket> &pkt);

void EndScreen (const boost::shared_ptr<OPacket> &pkt);

/**
=======================================
D-uint16_t HERE WHATEVER ++++++++++++++++++++++++++++++++++++++++++
   [D-uint16_t] Character Model
     [uint8_t] Volume and Height
     [uint8_t] Level
     [uint8_t] Highest Level
   [Q-uint16_t] EXP
   [D-uint16_t] SP Bar
   [Q-uint16_t] Gold
   [D-uint16_t] SP
     [uint16_t] Available Stat Points
     [uint8_t] Berserk Gauge 1-5
   [D-uint16_t] Berserk EXP
   [D-uint16_t] HP
   [D-uint16_t] MP
     [uint8_t] Noob Icon
00
00 00
00 00 00 00
     [uint8_t] Rank
     [uint8_t] Maximum Item Slot
     [uint8_t] No. of Items
--------------------------------
ITEMS ------
--------------------------------
00 00 00 00 00 00 00 00 00 00 00 00 01 E0 00 00 00 01 00 00 00
     [uint8_t] Avatar List Start (05)
     [uint8_t] Avatar Count
--------------------------------
ITEMS ------
--------------------------------
     [uint8_t] Avatar List End (00)
     [uint8_t] Next Mastery (01)
   [D-uint16_t] Mastery
     [uint8_t] Level
     [uint8_t] Skill List (02)
     [uint8_t] Skill List Start (00)
--------------------------[]
     [uint8_t] Skill Start (01)
   [D-uint16_t] Skill ID
     [uint8_t] Skill Level
--------------------------()
     [uint8_t] Skill List End (02)
01 +++++++++++++++++++++++++++++++++++++++++++++++++++++++
     [uint8_t] Amount of Completed Quests
--------------------------[]
   [D-uint16_t] Event ID (01 00 00 00)
   [D-uint16_t] Quest ID
--------------------------()
     [uint8_t] Amount of Quests
--------------------------[]
   [D-uint16_t] Quest ID
 [1/2 uint8_t] Quest Repetition Amount
 [1/2 uint8_t] Quest Completetion Amount
00
58
01
     [uint8_t] Subdirectory Quest Order no.
     [uint8_t] Quest Status (01 - On) (00 - Done)
     [uint16_t] PK2 Name Define: Quest Objective Length
   [D-uint16_t] PK2 Name Define: Quest Objective
01
   [D-uint16_t] Amount of Items + Kills + NPC ID
--------------------------()
     [uint8_t] Quest List End (00)
   [D-uint16_t] Player ID
     [uint16_t] Zone
    [FLOAT] X
    [FLOAT] Z
    [FLOAT] Y
     [uint16_t] Angle
     [uint8_t] have_destination
     [uint8_t] Walk Flag (00-Walk) (01-Run)
--------------------------[]
if(no_destination)
     [uint8_t] No Destination
     [uint16_t] Angle
--------------------------()
--------------------------[]
if(have_destination)
     [uint16_t] Zone
     [uint16_t] X
     [uint16_t] Z
     [uint16_t] Y
--------------------------()
     [uint8_t] Death Flag
     [uint8_t] Movement Flag
     [uint8_t] Berserker Mode
    [FLOAT] Walking Speed
    [FLOAT] Running Speed
    [FLOAT] Berserk Speed
     [uint8_t] No. of Buffs
--------------------------[]
   [D-uint16_t] Skill ID
   [D-uint16_t] Duration in ms
--------------------------()
     [uint16_t] Player Name Length
   [STRING] Player Name
     [uint16_t] Alias Name Length
   [STRING] Alias Name
     [uint8_t] Job Level
     [uint8_t] Job Type
   [D-uint16_t] Trade EXP
   [D-uint16_t] Thief EXP
   [D-uint16_t] Hunter EXP
00
00
00
     [uint8_t] PVP Flag (FF-off)
D7 03
E0 00 00 00  ++++++++++++++++++++++++++++++++++++++++++
00 00
   [D-uint16_t] Account ID
00
07
     [uint8_t] Number of Hot Keys
--------------------------[]
     [uint8_t] Slot
     [uint8_t] Type
   [D-uint16_t] Item ID
--------------------------()
     [uint8_t] HP Slot
     [uint8_t] HP Value
     [uint8_t] MP Potion Slot
     [uint8_t] MP Value
     [uint8_t] Abnormal Status Pill Slot
     [uint8_t] Abnormal Status Pill Value
     [uint8_t] Potion Delay
     [uint8_t] Amount of Players Blocked
--------------------------[]
     [uint16_t] Player Name Length
   [STRING] Player Name
--------------------------()
01 00 01 00
00
00
=======================================
**/

void PlayerInfo (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Player> &player,
    const uint32_t accountID);

void PlayerID (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Player> &player);

void PlayerStats (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Player> &player);

void PlayerSP (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Player> &player);

void PlayerGold (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Player> &player);

void PlayerBerserkGauge (const boost::shared_ptr<OPacket> &pkt, const uint8_t gauge_count, const uint32_t objID);

void PlayerAction (const boost::shared_ptr<OPacket> &pkt, PLAYER_ACTION_TYPE type);

void PlayerPickup (const boost::shared_ptr<OPacket> &pkt, const uint32_t ID);

void PlayerLevel (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID);

void PlayerEmote (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Player> &player, uint8_t action);

void PlayerHelp (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Player> &player, uint8_t action);

void PlayerXPSP (const boost::shared_ptr<OPacket> &pkt, const uint32_t ID, const uint64_t xp, const uint64_t spxp);

void PlayerXPSP (const boost::shared_ptr<OPacket> &pkt, const uint32_t ID, const uint64_t xp, const uint64_t spxp,
    const uint16_t stats);

void PlayerSilk (const boost::shared_ptr<OPacket> &pkt, const uint32_t amount);

void PlayerStatus (const boost::shared_ptr<OPacket> &pkt, const uint32_t EncStatus);

void PlayerStatus (const boost::shared_ptr<OPacket> &pkt, const uint32_t EncStatus, const uint32_t Duration,
    const uint16_t Level);

void PlayerDeath (const boost::shared_ptr<OPacket> &pkt, const PLAYER_DEATH_TYPE type);

void IncInt (const boost::shared_ptr<OPacket> &pkt);

void IncStr (const boost::shared_ptr<OPacket> &pkt);

void InventorySlots (const boost::shared_ptr<OPacket> &pkt, const uint8_t size);

}

#endif //__SLES_GAME_SERVER_PLAYER_PACKET_HPP__
