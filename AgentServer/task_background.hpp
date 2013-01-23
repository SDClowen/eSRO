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

#ifndef TASK_BACKGROUND_HPP
#define TASK_BACKGROUND_HPP

#include <stdint.h>
#include <boost/weak_ptr.hpp>

class NPC;

enum BG_TASK
{
    BG_TASK_COOLDOWN_SKILL,
    BG_TASK_COOLDOWN_ITEM,
    BG_TASK_COOLDOWN_ATTACK,
    BG_TASK_IMBUE,
    BG_TASK_STATUS,
    BG_TASK_BUFF,
    BG_TASK_STUN,
    BG_TASK_CONSUME_MP,
    BG_TASK_STATUS_DAMAGE,
    BG_TASK_SPAWN_JOB_NPC
};

enum STATE_TASK
{
    STATE_TASK_BERSERK,
    STATE_TASK_INVUNERABLE
};

struct BTask
{
    BG_TASK task;
    uint32_t ID;
    uint32_t seconds;
    uint32_t arg;
    uint32_t arg1;
    bool persistent;
    boost::weak_ptr<NPC> entity;
};

struct STask
{
    STATE_TASK task;
    uint32_t seconds;
    uint32_t arg;
    uint32_t playerID;
};

#endif // TASK_BACKGROUND_HPP
