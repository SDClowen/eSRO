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

#include "skill_manager.hpp"
#include <skill.hpp>

SkillManager::SkillManager (const skill_map &skill_list)
    : m_skill_list(skill_list)
{
}

boost::shared_ptr<Skill> SkillManager::find (const uint32_t ID) const
{
    boost::mutex::scoped_lock lock(m_mutex);

    skill_map::const_iterator i = m_skill_list.find(ID);

    return i->second;
}

boost::shared_ptr<Skill> SkillManager::find_by_group_and_level (const uint32_t groupID, const uint8_t Level) const
{
    boost::mutex::scoped_lock lock(m_mutex);

    boost::shared_ptr<Skill> ret;

    for (skill_map::const_iterator i = m_skill_list.begin(); i != m_skill_list.end(); ++i)
    {
        if (i->second->group_id() == groupID && i->second->level() == Level)
        {
            ret = i->second;
            break;
        }
    }

    return ret;
}
