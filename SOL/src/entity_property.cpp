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

#include "entity_property.hpp"

ENTITY_TYPE EntityProperty::type () const
{
    int t4 = type_list[3];
	int t5 = type_list[4];
	int t6 = type_list[5];

	ENTITY_TYPE type = ET_UNKNOWN;

    if (t4 == 1 && t5 == 0 && t6 == 0)
    {
        type = ET_PLAYER;
    }
    else if (t4 == 2)
    {
        if (t5 == 1)
        {
            switch(t6)
            {
            case 1: ///MOBS
                type = ET_MOB;
                break;
            case 2: ///THIEF MOBS
                type = ET_MOB_THIEF;
                break;
            case 3: ///HUNTER MOBS
                type = ET_MOB_HUNTER;
                break;
            case 4: ///QUEST MOBS
                type = ET_MOB_QUEST;
                break;
            case 5: ///PANDORA MOBS OR EVENT
                type = ET_MOB_EVENT;
                break;
            default:
                break;
            }
        }
        else if ( t5 == 2)
        {
            switch(t6)
            {
            case 0:
            case 1: ///NPCS
                type = ET_INTERACTIVE;
                break;
            case 2: ///GATE PULLEY
                break;
            default:
                break;
            }
        }
        else if (t5 == 3)
        {
            switch(t6)
            {
            case 1: ///NORMAL TRANSPORT
                type = ET_COS_VEHICLE;
                break;
            case 2: ///JOB TRANSPORT
                type = ET_COS_TRANSPORT;
                break;
            case 3: ///GROW PET
                type = ET_COS_GROWTH;
                break;
            case 4: ///ABILITY PET
                type = ET_COS_ABILITY;
                break;
            case 5: ///MERCENARIES
                type = ET_COS_MERC;
                break;
            case 6: ///CAPTURED MOBS
                type = ET_COS_MOB;
                break;
            case 7: ///NPC_CH_QT_MOONSHADOW_COS
            case 8: ///NPC_CH_QT_FLAMEMASTER_COS
                break;
            default:
                break;
            }
        }
    }

    return type;
}
