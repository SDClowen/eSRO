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

#include "spawnobject.hpp"

SpawnObject::SpawnObject (float x,float y, float z, int zone)
	: m_CurrentPos(x,y,z, zone),m_Angle(0)
{
}

SpawnObject::SpawnObject (const Coord &position)
	: m_CurrentPos(position), m_Angle(0)
{
}

void SpawnObject::set_position (const Coord& position)
{
	m_CurrentPos = position;
}

Coord SpawnObject::get_position () const
{
    return m_CurrentPos;
}

void SpawnObject::set_angle (const uint16_t angle)
{
	m_Angle = angle;
}

uint16_t SpawnObject::get_angle () const
{
	return m_Angle;
}
