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

#include "item_property.hpp"

ITEM_TYPE ItemProperty::GetType() const
{    
    return ItemType(Types[3],Types[4],Types[5]);
}

ITEM_TYPE ItemType (uint8_t t4, uint8_t t5, uint8_t t6)
{
    ITEM_TYPE type = ITEM_NOT_REGISTERED;

    if (t4 == 1 && t5 == 1 )
    {
        switch (t6)
        {
        case 1:
            type = ITEM_CH_CLOTHES_HA;
            break;
        case 2:
            type = ITEM_CH_CLOTHES_SA;
            break;
        case 3:
            type = ITEM_CH_CLOTHES_BA;
            break;
        case 4:
            type = ITEM_CH_CLOTHES_LA;
            break;
        case 5:
            type = ITEM_CH_CLOTHES_AA;
            break;
        case 6:
            type = ITEM_CH_CLOTHES_FA;
            break;
        default:
            break;
        }
    }
    else if (t4 == 1 && t5 == 2 )
    {
        switch (t6)
        {
        case 1:
            type = ITEM_CH_LIGHT_HA;
            break;
        case 2:
            type = ITEM_CH_LIGHT_SA;
            break;
        case 3:
            type = ITEM_CH_LIGHT_BA;
            break;
        case 4:
            type = ITEM_CH_LIGHT_LA;
            break;
        case 5:
            type = ITEM_CH_LIGHT_AA;
            break;
        case 6:
            type = ITEM_CH_LIGHT_FA;
            break;
        default:
            break;
        }
    }
    else if (t4 == 1 && t5 == 3 )
    {
        switch (t6)
        {
        case 1:
            type = ITEM_CH_HEAVY_HA;
            break;
        case 2:
            type = ITEM_CH_HEAVY_SA;
            break;
        case 3:
            type = ITEM_CH_HEAVY_BA;
            break;
        case 4:
            type = ITEM_CH_HEAVY_LA;
            break;
        case 5:
            type = ITEM_CH_HEAVY_AA;
            break;
        case 6:
            type = ITEM_CH_HEAVY_FA;
            break;
        default:
            break;
        }
    }
    else if ( t4 == 1 && t5 == 4 )
    {
        switch(t6)
        {
        case 1:
            type = ITEM_CH_SHIELD;
            break;
        case 2:
            type = ITEM_EU_SHIELD;
            break;
        default:
            break;
        }

    }
    else if (t4 == 1 && t5 == 5)
    {
        switch (t6)
        {
        case 1:
            type = ITEM_CH_EARRING;
            break;
        case 2:
            type = ITEM_CH_NECKLACE;
            break;
        case 3:
            type = ITEM_CH_RING;
            break;
        default:
            break;
        }
    }
    else if ( t4 == 1 && t5 == 6 )
    {
        switch(t6)
        {
        case 2:
            type = ITEM_CH_SWORD;
            break;
        case 3:
            type = ITEM_CH_BLADE;
            break;
        case 4:
            type = ITEM_CH_SPEAR;
            break;
        case 5:
            type = ITEM_CH_TBLADE;
            break;
        case 6:
            type = ITEM_CH_BOW;
            break;
        case 7:
            type = ITEM_EU_SWORD;
            break;
        case 8:
            type = ITEM_EU_TSWORD;
            break;
        case 9:
            type = ITEM_EU_AXE;
            break;
        case 10:
            type = ITEM_EU_DARKSTAFF;
            break;
        case 11:
            type = ITEM_EU_TSTAFF;
            break;
        case 12:
            type = ITEM_EU_CROSSBOW;
            break;
        case 13:
            type = ITEM_EU_DAGGER;
            break;
        case 14:
            type = ITEM_EU_HARP;
            break;
        case 15:
            type = ITEM_EU_STAFF;
            break;
        case 16:
            type = ITEM_FORTRESS_WEAPON;
            break;
        default:
            break;
        }
    }
    else if (t4 == 1 && t5 == 7)
    {
        switch(t6)
        {
        case 1:
        case 6:
            type = ITEM_JOB_TRADER;
            break;
        case 2:
            type = ITEM_JOB_THIEF;
            break;
        case 3:
        case 7:
            type = ITEM_JOB_HUNTER;
            break;
        case 5:
            type = ITEM_PVP_CAPE;
            break;
        default:
            break;
        }
    }
    else if (t4 == 1 && t5 == 9 )
    {
        switch (t6)
        {
        case 1:
            type = ITEM_EU_CLOTHES_HA;
            break;
        case 2:
            type = ITEM_EU_CLOTHES_SA;
            break;
        case 3:
            type = ITEM_EU_CLOTHES_BA;
            break;
        case 4:
            type = ITEM_EU_CLOTHES_LA;
            break;
        case 5:
            type = ITEM_EU_CLOTHES_AA;
            break;
        case 6:
            type = ITEM_EU_CLOTHES_FA;
            break;
        default:
            break;
        }
    }
    else if (t4 == 1 && t5 == 10 )
    {
        switch (t6)
        {
        case 1:
            type = ITEM_EU_LIGHT_HA;
            break;
        case 2:
            type = ITEM_EU_LIGHT_SA;
            break;
        case 3:
            type = ITEM_EU_LIGHT_BA;
            break;
        case 4:
            type = ITEM_EU_LIGHT_LA;
            break;
        case 5:
            type = ITEM_EU_LIGHT_AA;
            break;
        case 6:
            type = ITEM_EU_LIGHT_FA;
            break;
        default:
            break;
        }
    }
    else if (t4 == 1 && t5 == 11 )
    {
        switch (t6)
        {
        case 1:
            type = ITEM_EU_HEAVY_HA;
            break;
        case 2:
            type = ITEM_EU_HEAVY_SA;
            break;
        case 3:
            type = ITEM_EU_HEAVY_BA;
            break;
        case 4:
            type = ITEM_EU_HEAVY_LA;
            break;
        case 5:
            type = ITEM_EU_HEAVY_AA;
            break;
        case 6:
            type = ITEM_EU_HEAVY_FA;
            break;
        default:
            break;
        }
    }
    else if (t4 == 1 && t5 == 12)
    {
        switch (t6)
        {
        case 1:
            type = ITEM_EU_EARRING;
            break;
        case 2:
            type = ITEM_EU_NECKLACE;
            break;
        case 3:
            type = ITEM_EU_RING;
            break;
        default:
            break;
        }
    }
    else if (t4 == 1 && t5 == 13)
    {
        switch(t6)
        {
        case 1:
            type = ITEM_AVATAR_HAT;
            break;
        case 2:
            type = ITEM_AVATAR_ARMOR;
            break;
        case 3:
            type = ITEM_AVATAR_ATTACH;
            break;
        case 4:
            type = ITEM_AVATAR_FLAG;
            break;
        default:
            break;
        }
    }
    else if (t4 == 1 && t5 == 14)
    {
        if (t6 == 1)
            type = ITEM_AVATAR_NASRUN;
    }
    else if (t4 == 2)
    {
        if ( t5 == 1)
        {
            switch(t6)
            {
            case 1:
                type = ITEM_COS_PET;
                break;
            case 2:
                type = ITEM_COS_GRAB_PET;
                break;
            }
        }
        else if (t5 == 2)
        {
            if (t6 == 0)
                type = ITEM_MONSTER_MASK;
        }
        else if (t5 == 3)
        {
            if (t6 == 1)
                type = ITEM_MAGIC_CUBE_ELIXIR;
        }
    }
    else if (t4 == 3)
    {
        if ( t5 == 1)
        {
            switch(t6)
            {
            case 1:
                type = ITEM_HP_POT;
                break;
            case 2:
                type = ITEM_MP_POT;
                break;
            case 3:
                type = ITEM_ALL_POT;
                break;
            case 4:
                type = ITEM_COS_HP_POT;
                break;
            case 6:
                type = ITEM_COS_REVIVAL;
                break;
            case 8:
                type = ITEM_BERSEK_SCROLL;
                break;
            case 9:
                type = ITEM_HGP_POT;
                break;
            case 10:
                type = ITEM_FORT_REPAIR_KIT;
                break;
            default:
                break;
            }
        }
        else if (t5 == 2)
        {
            switch(t6)
            {
            case 1:
                type = ITEM_PURIFICATION_PILL;
                break;
            case 6:
                type = ITEM_CURE_ALL;
                break;
            case 7:
                type = ITEM_COS_CURE_ALL;
                break;
            default:
                break;
            }
        }
        else if (t5 == 3)
        {
            switch(t6)
            {
            case 1:
                type = ITEM_RETURN_SCROLL;
                break;
            case 2:
                type = ITEM_COS_SCROLL;
                break;
            case 3:
                type = ITEM_REVERSE_RETURN_SCROLL;
                break;
            case 4:
                type = ITEM_STALL_AVATAR_SCROLL;
                break;
            case 5:
                type = ITEM_GLOBAL_CHAT;
                break;
            case 6:
                type = ITEM_FORTRESS_MOB_SCROLL;
                break;
            case 7:
                type = ITEM_FORTRESS_GUARD_SCROLL;
                break;
            case 8:
                type = ITEM_FORTRESS_STRUCT_SCROLL;
                break;
            case 9:
                type = ITEM_FORTRESS_FLAG_SCROLL;
                break;
            case 10:
                type = ITEM_SP_EXP_SCROLL;
                break;
            case 11:
                type = ITEM_FORTRESS_SCROLL;
                break;
            default:
                break;
            }
        }
        else if (t5 == 4)
        {
            switch(t6)
            {
            case 1:
                type = ITEM_ARROW;
                break;
            case 2:
                type = ITEM_BOLT;
                break;
            default:
                break;
            }
        }
        else if (t5 == 5)
        {
            if (t6 == 0)
                type = ITEM_GOLD;
            else if (t6 == 1)
                type = ITEM_SD_TOKEN;
        }
        else if (t5 == 6)
        {
            if (t6 == 1)
                type = ITEM_FIREWORKS;
            else if (t6 == 2)
                type = ITEM_FORTRESS_SHOCK_BOMB;
        }
        else if (t5 == 7)
        {
            if (t6 == 1)
                type = ITEM_CAMP_FIRE;
        }
        else if (t5 == 8)
        {
            switch(t6)
            {
            case 0:
                type = ITEM_ETC_SCROLL;
                break;
            case 1:
                type = ITEM_TRADE_ITEM;
                break;
            case 2:
                type = ITEM_TRADE_SPECIAL;
                break;
            default:
                break;
            }
        }
        else if (t5 == 9)
        {
            if (t6 == 0)
                type = ITEM_QUEST;
        }
        else if (t5 == 10)
        {
            switch(t6)
            {
            case 1:
                type = ITEM_ALCHEMY_ELIXIR;
                break;
            case 2:
                type = ITEM_ALCHEMY_LUCKY_POWDER;
                break;
            case 4:
                type = ITEM_ALCHEMY_ADVANCED_ELIXIR;
                break;
            default:
                break;
            }
        }
        else if (t5 == 11)
        {
            switch(t6)
            {
            case 1:
                type = ITEM_ALCHEMY_MAGIC_STONE;
                break;
            case 2:
                type = ITEM_ALCHEMY_ATTR_STONE;
                break;
            case 3:
                type = ITEM_ALCHEMY_TABLET;
                break;
            case 4:
                type = ITEM_ALCHEMY_MATERIAL;
                break;
            case 5:
                type = ITEM_ALCHEMY_ELEMENT;
                break;
            case 6:
                type = ITEM_ALCHEMY_RONDO;
                break;
            case 7:
                type = ITEM_ALCHEMY_STATUS_STONE;
                break;
            default:
                break;
            }
        }
        else if (t5 == 12)
        {
            switch(t6)
            {
            case 1:
                type = ITEM_GUILD_SOLDIER_SCROLL;
                break;
            case 2:
                type = ITEM_GUILD_EMBLEM_SCROLL;
                break;
            case 3:
                type= ITEM_GUILD_RECALL_SCROLL;
                break;
            default:
                break;
            }
        }
        else if (t5 == 13)
        {
            switch(t6)
            {
            case 0:
                type = ITEM_SKILL_RESTORE_POT;
                break;
            case 1:
                type = ITEM_BUFF;
                break;
            case 4:
                type = ITEM_XP_SP_TICKET;
                break;
            case 5:
                type = ITEM_SP_TICKET;
                break;
            case 6:
                type = ITEM_RESURRECT_SCROLL;
                break;
            case 7:
                type = ITEM_REPAIR_SCROLL;
                break;
            case 8:
                type = ITEM_TRANSGENDER_SCROLL;
                break;
            case 9:
                type = ITEM_SKIN_SCROLL;
                break;
            case 10:
                type = ITEM_WAREHOUSE_SCROLL;
                break;
            case 11:
                type = ITEM_ENCHANT_SCROLL;
                break;
            case 12:
                type = ITEM_COS_EXT_SCROLL;
                break;
            case 13:
                type = ITEM_STAT_SCROLL;
                break;
            case 14:
                type = ITEM_XP_SCROLL;
                break;
            case 15:
                type = ITEM_PET_SCROLL;
                break;
            case 16:
                type = ITEM_DEVIL_EXT_SCROLL;
                break;
            case 17:
                type = ITEM_STORAGE_SPACE_EXPANSION;
                break;
            default:
                break;
            }
        }
        else if (t5 == 14)
        {
            switch(t6)
            {
            case 1:
                type = ITEM_LOTTERY_CARD;
                break;
            case 2:
                type = ITEM_LOTTERY_CARD_RESULT;
                break;
            }
        }
        else if (t5 == 15)
        {
            switch(t6)
            {
            case 1:
                type = ITEM_PANDORA_BOX;
                break;
            case 2:
                type = ITEM_PANDORA_PARTY_BOX;
                break;
            }
        }
    }

    return type;
}
