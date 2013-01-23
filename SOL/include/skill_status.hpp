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

#ifndef __SLES_SKILL_STATUS_HPP__
#define __SLES_SKILL_STATUS_HPP__

/**
 *
 *	STATUS EFFECT
 *
 *	@BURN Induce fire damage over time for a fixed period of time.
 *	@SHOCK Lower player parry ratio for a fixed period of time.
 *	@FROSTBITE Slow player movements for a fixed period of time.
 *	@FREEZE Inmobilize player for a fixed period of time.
 *	@POSION Induce poison damage over time for a fixed period of time.
 *	@ZOMBIE Healing potions do the opposite effect for a fixed period of time,
 *	the effects dont kill the player.
 *
 *	CURSE EFFECTS
 *
 *	@Darkness = Decrease attack rating.
 *	@Disease = Increase the change to land curse and/or status.
 *	@ShortSight = Decrease attack range.
 *	@Division = Damage increase.
 *  @Weaken = Decrease Magical Defense.
 *  @Decay = Decrease Physical Defense.
 *
 **/

enum STATUS_TYPE
{
    STATUS_FROZE        = 0x01,
    STATUS_FROST        = 0x02,
    STATUS_SHOCK        = 0x04,
    STATUS_BURN         = 0x08,
    STATUS_POISON       = 0x10,
    STATUS_ZOMBIE       = 0x20,
    STATUS_SLEEP        = 0x40,
    STATUS_BIND         = 0x08,
    STATUS_DULL         = 0x0100,
    STATUS_FEAR         = 0x0200,
    STATUS_SHORT_SIGHT  = 0x0400,
    STATUS_BLEED        = 0x0800,
    STATUS_PETRIFY      = 0x1000,   /// ECKORO SAID THIS CRASHED
    STATUS_DARKNESS     = 0x2000,
    STATUS_STUN         = 0x4000,
    STATUS_DISEASE      = 0x8000,
    STATUS_CONFUSION    = 0x010000,
    STATUS_DECAY        = 0x020000,
    STATUS_WEAKEN       = 0x040000,
    STATUS_IMPOTENT     = 0x080000,
    STATUS_DIVISION     = 0x100000,
    STATUS_PANIC        = 0x200000,
    STATUS_COMBUSTION   = 0x400000,
    STATUS_HIDDEN       = 0x01000000,

    STATUS_GROUP_RESTRAIN = STATUS_STUN | STATUS_FROZE | STATUS_PETRIFY | STATUS_SLEEP | STATUS_BIND
};

struct Status
{
    Status ()
    {
        ID = 0;
        taskID = 0;
        payloadTaskID = 0;
        Duration = 0;
        Probability = 0;
        Level = 0;
        Payload = 0;
    }

    uint32_t ID;
    uint32_t taskID;        /// Assigned task ID.
    uint32_t payloadTaskID; /// Assigned payload task for over time payloads.
    uint32_t Duration;  /// [ms]
    uint8_t Probability;
    uint8_t Level;      /// Level of the debuff.
    uint16_t Effect;    /// Effect of the status (cold,shock,burn,etc)
    int16_t Payload;
};

#endif //__SLES_SKILL_STATUS_HPP__
