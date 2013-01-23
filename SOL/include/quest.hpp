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

#ifndef __SLES_GAMESERVER_QUEST_HPP__
#define __SLES_GAMESERVER_QUEST_HPP__

#include "types.hpp"
#include <string>

class Quest
{
public:
	inline void SetID (const uint32_t id) { m_ID = id; }
	inline void SetName (const std::string& name) { m_Name = name; }
	inline uint32_t GetID() const { return m_ID; }
	inline std::string GetName() const { return m_Name; }

protected:
	std::string m_Name;
	uint32_t m_ID;
};

#endif //__SLES_GAMESERVER_QUEST_HPP__
