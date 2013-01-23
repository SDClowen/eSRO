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

#ifndef __SLES_SPAWNABLE_OBJECT_HPP__
#define __SLES_SPAWNABLE_OBJECT_HPP__

#include "object.hpp"
#include "coord.hpp"

class SpawnObject : public Object
{
public:

	SpawnObject (float x=0.0, float y=0.0, float z=0.0, int zone = 0);

	SpawnObject (const Coord &position);

	void set_position (const Coord& position);

    Coord get_position() const;

	void set_angle (const uint16_t angle);

    uint16_t get_angle () const;

protected:

	Coord m_CurrentPos;
    uint16_t m_Angle;
};

#endif //__SLES_SPAWNABLE_OBJECT_HPP__
