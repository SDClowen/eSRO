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

#ifndef __SLES_TELEPORT_BUILDING_HPP__
#define __SLES_TELEPORT_BUILDING_HPP__

#include "object.hpp"
#include "building_property.hpp"
#include <vector>

enum TELEPORT_TASKS
{
    TELEPORT_TASK_DESIGNATE = 7,
    TELEPORT_TASK_TELEPORT,
    TELEPORT_TASK_TELEPORT_RETURN_POINT
};

struct TeleportBuilding : public Object
{
    TeleportBuilding ()
        : TeleportID(0), TaxRate(0), TaxActive(false)
    {}

    uint16_t TeleportID;
    uint16_t TaxRate;
    bool TaxActive;
    BuildingProperty m_Property;

    std::vector<uint8_t> Tasks;
};

#endif //__SLES_TELEPORT_BUILDING_HPP__
