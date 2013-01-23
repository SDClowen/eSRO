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

#ifndef __SR_GAME_SERVER_SKILL_MANAGER_HPP__
#define __SR_GAME_SERVER_SKILL_MANAGER_HPP__

#include <stdint.h>
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

class Skill;

class SkillManager
{
public:

    typedef boost::unordered_map<uint32_t,boost::shared_ptr<Skill> > skill_map;

    SkillManager (const skill_map &skill_list);

    boost::shared_ptr<Skill> find (const uint32_t ID) const;

    boost::shared_ptr<Skill> find_by_group_and_level (const uint32_t groupID, const uint8_t Level) const;

private:

    mutable boost::mutex m_mutex;
    skill_map m_skill_list;
};

#endif
