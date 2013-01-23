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

#ifndef GUILD_ERROR_HPP
#define GUILD_ERROR_HPP

enum GUILD_ERROR_TYPE
{
    ERROR_GUILD_INVALID_GUILD_NAME = 0x1A,
    ERROR_GUILD_MAX_LEVEL = 0x21,
    ERROR_GUILD_INSUFFICIENT_PERMISSION = 0x24,
    ERROR_GUILD_INVALID_NAME = 0x25,
    ERROR_GUILD_INSUFFICIENT_GOLD = 0x31,
    ERROR_GUILD_INSUFFICIENT_GP = 0x32,
    ERROR_GUILD_ALLY = 0x41,
    ERROR_GUILD_STORAGE_IN_USE = 0x48,
    ERROR_GUILD_OBJECT_FAR = 0x60
};

/*
02 06 - The current player is busy
02 07 - Cannot manage a guild task while dead
02 12 - The current player belongs to the same guild
02 13 - Cannot join the guild due to the maximum member limit
02 16 - Canceled the application for the guild
02 17 - The current player belongs to another guild
02 18 - Guild name must consist of 2~12 English letters
02 19 - Cannot create a new guild due to an invalid name
02 21 - Cannot promote the guild anymore
02 22 - The title has no entry
02 23 - Please enter the contents of the note
02 24 - The other player is not authorized
02 25 - The selected guild doesnt exist
02 26 - Cannot form an alliance with your own guild
02 27 - The player is under the same alliance
02 28 - Cannot propose an alliance because the selected player belongs to your alliance
02 29 - You have already reached the max amount of guilds that can form an alliance
02 36 - You are the leader of the guild and you cannot be banned
02 38 - Voters and candidates cannot leave the guild during the vote
02 39 - Voters and candidates cannot be banned the guild during the vote
02 41 - The selected guild is your ally so you can't hostile to it
02 42 - Cannot propose an alliance to a hostile guild
02 43 - If the leader of guild doesnt have Guild War Points to spend, you cant accept the proposal
02 44 - Unknown error
02 46 - Cannot disband the guild during a war
02 48 - Guild member is using guild storage, so corresponding command cannot be carried out
02 52 - You are not authorized
02 53 - Only 2 abilities can be added to a mercenary, so no more can be added
02 54 - The ability has already been added. Choose another ability
02 55 - When guild mercenary is summoned, guild master cannot pass its authority
02 56 - Guild registration cannot be proceeded when job item is equipped
02 57 - Guild creation cannot be proceeded when job item is equipped
02 58 - You are not allowed to change the position of the chief of the guild
02 59 - The concerned position has already been provided to other member of guild
02 70 - The behavior is not allowed
02 71 - The behavior is not allowed
02 72 - The behavior is not allowed
02 73 - The behavior is not allowed
02 74 - The behavior is not allowed
02 75 - The behavior is not allowed
02 76 - The behavior is not allowed
02 77 - The behavior is not allowed
02 0A - Cannot create a new guild because your level is too low
02 0C - Cannot create a new guild due to insufficient gold
02 1A - The selected guild name already exists
02 1B - Please try again
02 1C - The guild task has not been carried out. Please try again
02 1E - You are not authorized
02 1F - You are the leader of the guild and you cannot be banned
02 2A - Your guild level is below the minimum level required to create an alliance
02 2B - Level of invited guild is below the required level to participate in the alliance
02 3A - The selected guild is already at war with you
02 3B - The leader of the selected guild is not on the server
02 3C - No period that can make guild
02 3D - Current this user is not allowed to join guild
02 3E - Can not make more hostile guild
02 3F - Opposite guild can not make more hostile guilds
02 4A - Guild level must be 2 or higher to use guild storage
02 4B - Max allowed number of union chatting privileges is full
02 4C - At the moment corresponding guild member is using guild storage, so authority cannot be changed
02 4D - The current name is invalid because it includes special or invalid letters
02 5A - No position is provided since you have already been provided with a position
02 5B - You dont own any fortress
02 5C - The guild member bears no position
02 5D - No position is provided while you are participating in the fortress war
02 5E - Maximum reward when applying for a guild war is limited to (500,000,000)
*/

#endif // GUILD_ERROR_HPP
