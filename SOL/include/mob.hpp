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

#ifndef __SLES_GAMESERVER_MOB_HPP__
#define __SLES_GAMESERVER_MOB_HPP__

#include "npc.hpp"

enum MOB_TASK
{
    MOB_TASK_IDLE,
    MOB_TASK_MOVE,
    MOB_TASK_MOVE_TO_TARGET,
    MOB_TASK_CAST_WAIT,
    MOB_TASK_CAST,
    MOB_TASK_STUNNED
};

enum MOB_QUALITY
{
    CH_QUALITY_NORMAL,
    CH_QUALITY_CHAMPION,
    CH_QUALITY_UNIQUE = 3,
    CH_QUALITY_GIANT,
    CH_QUALITY_TITAN,
    CH_QUALITY_ELITE,
    CH_QUALITY_EVENT_ELITE,
    CH_QUALITY_LESSER_UNIQUE
};

class Mob : public NPC
{
public:

    typedef NPC::attack_type attack_type;

    Mob (const boost::shared_ptr<EntityProperty> &property, const uint8_t quality);

	virtual ~Mob ();

    uint8_t Quality () const;

	virtual uint8_t level () const;

    virtual void calculate_stats ();

    virtual bool IsTaskInterruptable () const;

	const std::map<uint32_t,uint32_t>& get_attack_list () const;

    uint32_t get_next_skill ();

    void SetSpawnPoint (const size_t spawnIndex);

    size_t GetSpawnPoint () const;

    bool IsRespawnable () const;

private:

    uint8_t m_quality;
    uint8_t m_skill_idx;

    size_t m_spawn_idx;
};

#endif //__SLES_GAMESERVER_MOB_HPP__
