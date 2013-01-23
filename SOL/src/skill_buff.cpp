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

#include "skill_buff.hpp"

Buff::Buff ()
    : SkillID(0),CastID(0),GroupID(0),Level(0),
    duration(0),passive(false),persistent(false),
    ReqArmor(REQ_ARMOR_NONE),ReqShield(false)
{
    for (int i = 0; i < 3; ++i)
        ReqWeapon[i] = REQ_WEAPON_NONE;
}

bool Buff::IsCastable (const Buff &buff, bool allow_same_lv) const
{
    if (GroupID == buff.GroupID)
    {
        if (allow_same_lv)
            return buff.Level >= Level;
        else
            return buff.Level > Level;
    }

    return true;
}

bool Buff::IsEffectStackable (const Buff &buff) const
{
    if (GroupID == buff.GroupID)
        return true;

    /// PASSIVE BONUS IF THEY DONT BELONG TO THE SAME GROUP CAN BE STACKED.
    if (passive)
        return true;
    
    /// CHECK IF EFFECTS CAN BE STACKED.
    for (size_t i = 0; i < buff.Effects.size(); ++i)
    {
        for (size_t j = 0; j < Effects.size(); ++j)
        {
            bool valid = true;

            if (buff.Effects[i].ID == Effects[j].ID)
            {
                switch(Effects[j].ID)
                {
                case BUFF_DAMAGE_INC:
                case BUFF_DEFENSE_INC:
                    for (int n = 0; n < 4; ++n)
                    {
                        if (buff.Effects[i].Arg[n] && Effects[i].Arg[n])
                            valid = false;
                    }
                    break;
                default:
                    valid = false;
                    break;
                }

                if (!valid)
                    return false;
            }
        }
    }

    return true;
}

bool Buff::is_weapon_valid (const REQ_WEAPON_TYPE weapon) const
{
    if (ReqWeapon[0] == REQ_WEAPON_NONE)
        return true;

    for (int j=0; j < 3; ++j)
    {
        if (ReqWeapon[j] == weapon)
            return true;
    }

    return false;
}
