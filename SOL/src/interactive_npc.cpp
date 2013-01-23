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

#include "interactive_npc.hpp"
#include "entity_property.hpp"

InteractiveNPC::InteractiveNPC (const boost::shared_ptr<EntityProperty> &property)
    : NPC(property), m_FortressActive(false), m_FortressTax(0), m_TeleportID(0)
{
}

InteractiveNPC::~InteractiveNPC ()
{
}

uint8_t InteractiveNPC::level () const
{
    return m_property->Level;
}

void InteractiveNPC::update ()
{
}

void InteractiveNPC::SetTeleportID (const uint16_t ID)
{
    m_TeleportID = ID;
}

uint16_t InteractiveNPC::GetTeleportID () const
{
    return m_TeleportID;
}

void InteractiveNPC::setFortressActive (bool active)
{
    m_FortressActive = active;
}

bool InteractiveNPC::isFortressActive () const
{
    return m_FortressActive;
}

std::string InteractiveNPC::GetFortress () const
{
    return m_property->Fortress;
}

void InteractiveNPC::SetFortressTax (const int16_t tax)
{
    m_FortressTax = tax;
}

int16_t InteractiveNPC::GetFortressTax () const
{
    return m_FortressTax;
}

void InteractiveNPC::InsertTask (const NPCTask &task)
{
    m_TaskList.push_back(task);
}

const std::vector<NPCTask>& InteractiveNPC::GetAllTasks () const
{
    return m_TaskList;
}

std::vector<uint8_t> InteractiveNPC::GetAvailableTasks (const uint8_t level) const
{
    std::vector<uint8_t> tasks;

    for (std::vector<NPCTask>::const_iterator i = m_TaskList.begin(); i != m_TaskList.end(); ++i)
    {
        if (level >= i->MinLv)
            tasks.push_back(i->ID);
    }

    return tasks;
}

bool InteractiveNPC::IsTaskAvailable (const uint8_t taskID, const uint8_t level) const
{
    for (std::vector<NPCTask>::const_iterator i = m_TaskList.begin(); i != m_TaskList.end(); ++i)
    {
        if (i->ID == taskID && level >= i->MinLv)
            return true;
    }

    return false;
}
