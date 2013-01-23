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

#include "server.hpp"
#include "server_connection.hpp"
#include "server_state_game.hpp"

#include "world.hpp"

#include <packet_util_functions.hpp>
/*
[3C97]
01 - 30 Minutes Before start of Fortress
02 - The Fortress War has started
03 - 30 Minutes Before end of Fortress
04 - 20 Minutes Before end of Fortress
05 - 10 Minutes Before end of Fortress
06 - The Fortess War has just complete
09 - 1 Minute till end of Fortress



{Guild Occupied the Fortress}
[S -> C][3C97][29 bytes]

08 03 00 00 00 06 00 6C C0 CE BF AC 6C
03 17 00 00 01 00 00 00 DB 00 00 00 06
00 00 00
=========================
08
    [D-WORD] Fortress ID
      [WORD] Guild Name Length
    [STRING] Guild Name
    [D-WORD] Guild ID
    [D-WORD] Emblem Index
    [D-WORD] Union ID
    [D-WORD] Emblem Index
==========================
*/
int StateGame::OnFortressApply (IPacket &packet)
{
    /*
    uint32_t npcID = packet.Read<uint32_t>();

    uint8_t unk = packet.Read<uint8_t>();
    */

    PrintPacketToSyslog(packet);

    /*
    {Response}
    [S -> C][B7C6][19 bytes]
    06 01 0B 0E 14 00 0B 0E 15 1E 0B 09 00 00 0B 0D
    17 3B 00
    ===============================
    06 01
    ~ War Start Time
      [BYTE] Month
      [BYTE] Day
      [BYTE] Hour
      [BYTE] Minutes
    ~ War End Time
      [BYTE] Month
      [BYTE] Day
      [BYTE] Hours
      [BYTE] Minutes
    ~ Application Start Time
      [BYTE] Month
      [BYTE] Day
      [BYTE] Hour
      [BYTE] Minutes
    ~ Application End Time
      [BYTE] Month
      [BYTE] Day
      [BYTE] Hour
      [BYTE] Minutes
    00
    ================================



    {You are not in guild to apply}
    [S -> C][B7C6][3 bytes]
    06 02 0B
    */

    return MSG_SUCCESS;
}

int StateGame::OnFortressReturn (IPacket &packet)
{
    uint32_t ID = packet.Read<uint32_t>();

    /*
    {You do not Possess a Fortress}
    [S -> C][B521][2 bytes]
    02 07
    */

    return MSG_SUCCESS;
}
