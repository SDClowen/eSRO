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

#include "formulas.hpp"

#include <skill.hpp>
#include <npc.hpp>

#include <math.h>

uint64_t ExpGained (uint32_t baseXP, uint8_t playerMaxLv, uint8_t gap, uint8_t mobLv)
{
    int64_t xp = 0;

    if (mobLv == playerMaxLv)
        xp = static_cast<int64_t>(baseXP*(0.109*(10-gap)));
    else
    {
        int diff = mobLv - playerMaxLv;

        if (abs(diff) > 11)
            xp = 1;
        else
            xp = static_cast<int64_t>(baseXP*0.109*(10-gap)*((3*diff - 1)/100 + 1));
    }

    if ( xp < 1)
        xp = 1;

    return xp;
}

float SkillRange (const boost::shared_ptr<NPC> &caster, const boost::shared_ptr<Skill> &skill, uint32_t targetID)
{
    float range = 5;

    if (skill->ActionType() == SKILL_ACTION_ATTACK)
    {
        range += caster->attack_distance();

        AttackInfo info = skill->attack();

        AttackArea area = skill->area();

        switch(info.type)
        {
        case ATTACK_PHYSICAL_RANGED:
        case ATTACK_MAGICAL:
        case ATTACK_MAGICAL_RANGED:
            if (range < 50)
                range = 50;
            break;
        default:
            break;
        }
    }
    else
    {
        if (targetID)
            range = 50;
    }

    return range;
}

Coord RangePosition (const Coord &src, const Coord &dest, const float range, uint16_t *dir)
{
    float dx = ((dest.get_zone() & 0xFF)-(src.get_zone() & 0xFF))*1920 + dest.get_x() - src.get_x();
    float dy = ((dest.get_zone() >> 8)-(src.get_zone() >> 8))*1920 + dest.get_y() - src.get_y();

    float angle = atan2(dy,dx);
    float dist = sqrt(dx*dx+dy*dy);

    if (dir)
        *dir = angle*10430.38208;

    Coord pos;

    if (range < dist)
    {
        pos = src;

        int16_t zone = src.get_zone();


        float x = pos.get_x()+(dist-range)*cos(angle);
        float y = pos.get_y()+(dist-range)*sin(angle);

        if (x > 1920)
        {
            x -= 1920;
            ++zone;
        }
        else if ( x < 0)
        {
            x += 1920;
            --zone;
        }

        if (y > 1920)
        {
            y -= 1920;
            zone += 256;
        }
        else if ( y < 0)
        {
            y += 1920;
            zone -= 256;
        }

        pos.set_zone(zone);
        pos.set_x(x);
        pos.set_y(y);
    }

    return pos;
}

Coord TeleportPosition (const Coord &src, const Coord &dest, const float range, uint16_t *dir)
{
    float dx = ((dest.get_zone() & 0xFF)-(src.get_zone() & 0xFF))*1920 + dest.get_x() - src.get_x();
    float dy = ((dest.get_zone() >> 8)-(src.get_zone() >> 8))*1920 + dest.get_y() - src.get_y();

    int16_t zone = src.get_zone();
    float angle = atan2(dy,dx);

    if (dir)
        *dir = angle*10430.38208;

    float x = src.get_x()+range*cos(angle);
    float y = src.get_y()+range*sin(angle);

    if (x > 1920)
    {
        x -= 1920;
        ++zone;
    }
    else if ( x < 0)
    {
        x += 1920;
        --zone;
    }

    if (y > 1920)
    {
        y -= 1920;
        zone += 256;
    }
    else if ( y < 0)
    {
        y += 1920;
        zone -= 256;
    }

    return Coord(x,y,0,zone);
}

Coord KnockbackPosition (const Coord &src, const Coord &dest, uint16_t distance)
{
    float dx = ((dest.get_zone() & 0xFF)-(src.get_zone() & 0xFF))*1920 + dest.get_x() - src.get_x();
    float dy = ((dest.get_zone() >> 8)-(src.get_zone() >> 8))*1920 + dest.get_y() - src.get_y();

    int16_t zone = src.get_zone();
    float angle = atan2(dy,dx);

    float x = dest.get_x()+distance*cos(angle);
    float y = dest.get_y()+distance*sin(angle);

    if (x > 1920)
    {
        x -= 1920;
        ++zone;
    }
    else if ( x < 0)
    {
        x += 1920;
        --zone;
    }

    if (y > 1920)
    {
        y -= 1920;
        zone += 256;
    }
    else if ( y < 0)
    {
        y += 1920;
        zone -= 256;
    }

    return Coord(x,y,0,zone);
}
