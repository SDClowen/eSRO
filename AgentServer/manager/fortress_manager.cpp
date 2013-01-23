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

#include "fortress_manager.hpp"

FortressManager::FortressManager ()
{
}

bool FortressManager::Initialize ()
{
    return true;
}

void FortressManager::Release ()
{
}

void FortressManager::addFortress (const FortressProperty &property)
{
    m_NameMapping.insert(std::make_pair(property.refName,property.refID));
    m_Fortresses.insert(std::make_pair(property.refID,property));
}

bool FortressManager::isFortressActive (const std::string &name) const
{
    return m_NameMapping.find(name) != m_NameMapping.end();
}
