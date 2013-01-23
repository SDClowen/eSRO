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

#include "npc_util.hpp"
#include "npc_list.hpp"

bool IsInteractiveNPC (const uint32_t refCharID)
{
    bool ret;

    switch(refCharID)
    {
    case NPC_CH_SMITH:
    case NPC_CH_ARMOR:
    case NPC_CH_POTION:
    case NPC_CH_POTION2:
    case NPC_CH_POTION3:
    case NPC_CH_ACCESSORY:
    case NPC_CH_HORSE:
    case NPC_CH_SPECIAL:
    case NPC_CH_FERRY:
    case NPC_CH_BOATMAN:
    case NPC_CH_WAREHOUSE_M:
    case NPC_CH_WAREHOUSE_W:
    case NPC_CH_TICKET:
    case NPC_CH_LOTTERY:
    case NPC_CH_SHAMAN:
    case NPC_CH_CHEF:
    case NPC_CH_GENARAL:
    case NPC_CH_GENARAL_SW:
    case NPC_CH_GENARAL_SP:
    case NPC_CH_GENARAL_BO:
    case NPC_CH_PRIEST:
    case NPC_CH_DOCTOR:
    case NPC_CH_ISLAM:
    case NPC_CH_EUROPE:
    case NPC_CH_INDIA:
    case NPC_CH_SOLDIER_EM1:
    case NPC_CH_SOLDIER_EM2:
    case NPC_CH_SOLDIER_EA1:
    case NPC_CH_SOLDIER_EA2:
    case NPC_CH_SOLDIER_WE1:
    case NPC_CH_SOLDIER_WE2:
    case NPC_CH_SOLDIER_SO1:
    case NPC_CH_SOLDIER_SO2:
    case NPC_CH_BEGGARBOY:
    case NPC_CH_BIGMAN:
    case NPC_CH_KISAENG1:
    case NPC_CH_KISAENG2:
    case NPC_CH_KISAENG3:
    case NPC_CH_KISAENG4:
    case NPC_CH_KISAENG5:
    case NPC_CH_KISAENG6:
    case NPC_CH_HORSE1:
    case NPC_CH_HORSE2:
    case NPC_CH_HORSE3:
    case NPC_CH_HORSE4:
    case NPC_WC_SMITH:
    case NPC_WC_ARMOR:
    case NPC_WC_POTION:
    case NPC_WC_ACCESSORY:
    case NPC_WC_HORSE:
    case NPC_WC_FERRY:
    case NPC_WC_WAREHOUSE_M:
    case NPC_WC_WAREHOUSE_W:
    case NPC_WC_SPECIAL:
    case NPC_WC_PRIEST1:
    case NPC_WC_PRIEST2:
    case NPC_WC_GENARAL_SW:
    case NPC_WC_GENARAL_SP:
    case NPC_WC_GENARAL_BO:
    case NPC_WC_DOCTOR:
    case NPC_WC_SOLDIER_EA1:
    case NPC_WC_SOLDIER_EA2:
    case NPC_WC_SOLDIER_WE1:
    case NPC_WC_SOLDIER_WE2:
    case NPC_WC_SOLDIER_SO:
    case NPC_WC_PRIEST3:
    case NPC_KT_SMITH:
    case NPC_KT_ARMOR:
    case NPC_KT_POTION:
    case NPC_KT_ACCESSORY:
    case NPC_KT_HORSE:
    case NPC_KT_SPECIAL:
    case NPC_KT_DESIGNER:
    case NPC_KT_FERRY:
    case NPC_KT_BOATMAN1:
    case NPC_KT_BINIKI1:
    case NPC_KT_BINIKI2:
    case NPC_KT_WAREHOUSE:
    case NPC_KT_MANAGER:
    case NPC_KT_MINISTER:
    case NPC_KT_SOLDIER_EM1:
    case NPC_KT_SOLDIER_EM2:
    case NPC_KT_SOLDIER_EA1:
    case NPC_KT_SOLDIER_EA2:
    case NPC_KT_SOLDIER_SO1:
    case NPC_KT_SOLDIER_SO2:
    case NPC_KT_SOLDIER_WE1:
    case NPC_KT_SOLDIER_WE2:
    case NPC_CH_FERRY2:
    case NPC_WC_FERRY2:
    case NPC_TD_THIEF_BUY:
    case NPC_TD_THIEF_SELL:
    case NPC_TD_THIEF_A:
    case NPC_TD_THIEF_B:
    case NPC_TD_THIEF_C:
    case NPC_TD_THIEF_D:
    case NPC_CH_EVENT_SANTA_KISAENG1:
    case NPC_WC_FERRY3:
    case NPC_WC_FERRY4:
    case NPC_KT_FERRY2:
    case NPC_KT_FERRY3:
    case NPC_CH_EVENT_KISAENG1:
    case NPC_KT_FLYSHIP1:
    case NPC_KT_FLYSHIP2:
    case NPC_RM_VILLAGECHIEF:
    case NPC_RM_SLAVE1:
    case NPC_RM_SLAVE2:
    case NPC_RM_SLAVE3:
    case NPC_RM_YOUNGWOMAN1:
    case NPC_RM_YOUNGWOMAN2:
    case NPC_RM_OLDWOMAN:
    case NPC_RM_FLYSHIP1:
    case NPC_RM_FLYSHIP2:
    case NPC_CH_SPECIAL2:
    case NPC_WC_SPECIAL2:
    case NPC_RM_SPECIAL:
    case NPC_EU_SMITH:
    case NPC_EU_ARMOR:
    case NPC_EU_POTION:
    case NPC_EU_ACCESSORY:
    case NPC_EU_HORSE:
    case NPC_EU_SPECIAL:
    case NPC_EU_MERCHANT:
    case NPC_EU_FERRY:
    case NPC_EU_WAREHOUSE:
    case NPC_EU_HUNTER:
    case NPC_EU_WITCH:
    case NPC_EU_MADAM:
    case NPC_EU_PRIEST:
    case NPC_EU_ADVENTURER:
    case NPC_EU_MANAGER:
    case NPC_EU_CHINABOY:
    case NPC_EU_CONSUL:
    case NPC_EU_UNION1:
    case NPC_EU_UNION2:
    case NPC_EU_GENERAL:
    case NPC_EU_SOLDIER_CA1:
    case NPC_EU_SOLDIER_CA2:
    case NPC_EU_SOLDIER_NO1:
    case NPC_EU_SOLDIER_NO2:
    case NPC_EU_SOLDIER_EA1:
    case NPC_EU_SOLDIER_EA2:
    case NPC_EU_SOLDIER_PO1:
    case NPC_EU_SOLDIER_PO2:
    case NPC_EU_SOLDIER_CC1:
    case NPC_EU_FERRY2:
    case NPC_EU_GUILD:
    case NPC_EU_ADVICE:
    case NPC_AM_FERRY1:
    case NPC_AM_FERRY2:
    case NPC_AM_FLYSHIP:
    case NPC_AM_SPECIAL:
    case NPC_CA_SMITH:
    case NPC_CA_ARMOR:
    case NPC_CA_POTION:
    case NPC_CA_ACCESSORY:
    case NPC_CA_HORSE:
    case NPC_CA_SPECIAL:
    case NPC_CA_MERCHANT:
    case NPC_CA_WAREHOUSE:
    case NPC_CA_HUNTER:
    case NPC_CA_SOLDIER_EA1:
    case NPC_CA_SOLDIER_EA2:
    case NPC_CA_SOLDIER_SO1:
    case NPC_CA_SOLDIER_SO2:
    case NPC_CA_SOLDIER_WE1:
    case NPC_CA_SOLDIER_WE2:
    case NPC_CA_TUNNEL_NO:
    case NPC_CA_TUNNEL_SO:
    case NPC_CA_FLYSHIP:
    case NPC_TK_SPECIAL:
    case NPC_TK_TUNNEL_NO:
    case NPC_TK_TUNNEL_SO:
    case NPC_CH_GACHA_MACHINE:
    case NPC_CH_GACHA_OPERATOR:
    case NPC_WC_GUILD:
    case NPC_KT_GUILD:
    case NPC_CA_GUILD:
    case NPC_RM_FLYSHIP3:
    case NPC_KT_ARMOR_SILSAMO:
    case NPC_EU_ADVICE2:
    case NPC_CH_SOLDIER_MOVE_EM1:
    case NPC_CH_SOLDIER_MOVE_WE1:
    case NPC_CH_SOLDIER_MOVE_EA2:
    case NPC_CH_SOLDIER_MOVE_SO1:
    case NPC_EU_ADVICE3:
    case NPC_CH_FORTRESS_OFFICIAL:
    case NPC_CH_FORTRESS_MANAGER1:
    case NPC_CH_FORTRESS_BATTLEAIDE1:
    case NPC_CH_FORTRESS_SMITH1:
    case NPC_CH_FORTRESS_TRAINER1:
    case NPC_CH_FORTRESS_MANAGER2:
    case NPC_CH_FORTRESS_BATTLEAIDE2:
    case NPC_CH_FORTRESS_SMITH2:
    case NPC_CH_FORTRESS_TRAINER2:
    case NPC_WC_FORTRESS_OFFICIAL:
    case NPC_WC_FORTRESS_MANAGER1:
    case NPC_WC_FORTRESS_BATTLEAIDE1:
    case NPC_WC_FORTRESS_SMITH1:
    case NPC_WC_FORTRESS_TRAINER1:
    case NPC_WC_FORTRESS_MANAGER2:
    case NPC_WC_FORTRESS_BATTLEAIDE2:
    case NPC_WC_FORTRESS_SMITH2:
    case NPC_WC_FORTRESS_TRAINER2:
    case NPC_KT_FORTRESS_OFFICIAL:
    case NPC_KT_FORTRESS_MANAGER:
    case NPC_KT_FORTRESS_BATTLEAIDE:
    case NPC_KT_FORTRESS_SMITH:
    case NPC_KT_FORTRESS_TRAINER:
    case NPC_EU_FORTRESS_OFFICIAL:
    case NPC_EU_FORTRESS_MANAGER:
    case NPC_EU_FORTRESS_BATTLEAIDE:
    case NPC_EU_FORTRESS_SMITH:
    case NPC_EU_FORTRESS_TRAINER:
    case NPC_CA_FORTRESS_OFFICIAL:
    case NPC_CA_FORTRESS_MANAGER1:
    case NPC_CA_FORTRESS_BATTLEAIDE1:
    case NPC_CA_FORTRESS_SMITH1:
    case NPC_CA_FORTRESS_TRAINER1:
    case NPC_CA_FORTRESS_MANAGER2:
    case NPC_CA_FORTRESS_BATTLEAIDE2:
    case NPC_CA_FORTRESS_SMITH2:
    case NPC_CA_FORTRESS_TRAINER2:
    case NPC_CH_MOONSHADOW:
    case NPC_TQ_TREASUREBOX1:
    case NPC_TQ_TREASUREBOX2:
    case NPC_TQ_TREASUREBOX3:
    case NPC_TQ_TREASUREBOX4:
    case NPC_TQ_SNAKEGATE:
    case NPC_CH_QT_MOONSHADOW:
    case NPC_CH_QT_FLAMEMASTER:
    case NPC_CH_QT_GENARAL_SP:
    case NPC_EVENT_FLAGWAR_MASTERKEY:
    case NPC_EVENT_FLAGWAR_FLAGPOLE_BLUE:
    case NPC_EVENT_FLAGWAR_FLAP_FLAGPOLE_BLUE:
    case NPC_EVENT_FLAGWAR_FLAGPOLE_RED:
    case NPC_EVENT_FLAGWAR_FLAP_FLAGPOLE_RED:
    case NPC_EVENT_FLAGWAR_FLAGPOLE_WHITE:
    case NPC_EVENT_FLAGWAR_FLAP_FLAGPOLE_WHITE:
    case NPC_EVENT_FLAGWAR_FLAG:
    case NPC_EVENT_FLAGWAR_DOOR_BLUE:
    case NPC_EVENT_FLAGWAR_DOOR_RED:
    case NPC_CH_EVENT_CLOWN:
    case NPC_EU_EVENT_CARNIVAL:
    case NPC_EU_EVENT_CARNIVAL_OBJECT:
    case NPC_CH_EVENT_GRANDFATHER:
    case NPC_CH_EVENT_GRANDMOTHER:
    case NPC_CH_EVENT_CHILD:
    case NPC_CH_EVENT_YUTNORI:
    case NPC_SD_M_AREA_SMITH:
    case NPC_SD_M_AREA_POTION:
    case NPC_SD_M_AREA_ACCESSORY:
    case NPC_SD_M_AREA_SPECIAL:
    case NPC_SD_M_AREA_WAREHOUSE:
    case NPC_SD_M_AREA_GUILD:
    case NPC_SD_M_AREA_LIGHTHOUSE:
    case NPC_SD_M_AREA_HORSE:
    case NPC_SD_M_AREA_FERRY:
    case NPC_SD_M_AREA_MERCHANT:
    case NPC_SD_M_AREA_HUNTER:
    case NPC_SD_M_AREA_AHHA:
    case NPC_SD_M_AREA_EYPTSOLIDER_1:
    case NPC_SD_M_AREA_EYPTSOLIDER_2:
    case NPC_SD_M_AREA_EYPTSOLIDER_3:
    case NPC_SD_M_AREA_WATERMAN_1:
    case NPC_SD_M_AREA_WATERMAN_2:
    case NPC_SD_M_AREA_SPECIAL2:
    case NPC_SD_M_AREA_SPECIAL3:
    case NPC_SD_M_AREA_SENMUTE:
    case NPC_SD_M_AREA_MENTUHOTEP:
    case NPC_SD_M_AREA_LENENUNE:
    case NPC_SD_M_AREA_MANETO:
    case NPC_SD_M_AREA_BERENICE:
    case NPC_SD_T_AREA_THIEF:
    case NPC_SD_T_AREA_GUILD2:
    case NPC_SD_T_AREA_WAREHOUSE2:
    case NPC_SD_T_AREA_SMITH:
    case NPC_SD_T_AREA_POTION:
    case NPC_SD_T_AREA_ACCESSORY:
    case NPC_SD_GUILD_AGENT:
    case NPC_SD_GUILD_AGENT2:
    case NPC_SD_MERCHANT_CHANGER:
    case NPC_SD_THIEF_AGENT_CHANGER:
    case NPC_SD_GUILD_AGENT3:
    case NPC_SD_SELKIS_HEART:
    case NPC_SD_NEITH_HEART:
    case NPC_SD_ANUBIS_HEART:
    case NPC_SD_ISIS_HEART:
    case NPC_SD_SETH_HEART:
    case NPC_SD_M_AREA_ARMOR:
    case NPC_SD_M_AREA_EYPTSOLIDER_4:
    case NPC_SD_M_AREA_EYPTSOLIDER_5:
    case NPC_SD_M_AREA_EYPTSOLIDER_6:
    case NPC_SD_T_AREA_ARMOR:
    case NPC_SD_M_AREA_MENTUHOTEP_DEAD:
    case NPC_SD_HAROERIS_HEART:
        ret = true;
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}

bool IsTeleportNPC (const uint32_t refCharID)
{
    bool ret;

    switch(refCharID)
    {
    case NPC_CH_FERRY:
    case NPC_CH_FERRY2:
    case NPC_CH_KISAENG6:
    case NPC_CH_SOLDIER_MOVE_EM1:
    case NPC_CH_SOLDIER_MOVE_EA2:
    case NPC_CH_SOLDIER_MOVE_SO1:
    case NPC_CH_SOLDIER_MOVE_WE1:
    case NPC_CH_SOLDIER_EM1:
    case NPC_CH_SOLDIER_EA2:
    case NPC_CH_SOLDIER_SO1:
    case NPC_CH_SOLDIER_WE1:
    case NPC_WC_FERRY:
    case NPC_WC_FERRY2:
    case NPC_WC_FERRY3:
    case NPC_WC_FERRY4:
    case NPC_KT_FERRY2:
    case NPC_KT_FERRY3:
    case NPC_KT_FLYSHIP1:
    case NPC_KT_FLYSHIP2:
    case NPC_RM_FLYSHIP1:
    case NPC_RM_FLYSHIP2:
    case NPC_RM_FLYSHIP3:
    case NPC_EU_FERRY2:
    case NPC_EU_ADVICE:
    case NPC_EU_ADVICE3:
    case NPC_EU_ADVICE2:
    case NPC_EU_SOLDIER_PO2:
    case NPC_AM_FERRY1:
    case NPC_AM_FERRY2:
    case NPC_AM_FLYSHIP:
    case NPC_CA_TUNNEL_NO:
    case NPC_CA_TUNNEL_SO:
    case NPC_CA_FLYSHIP:
    case NPC_TK_TUNNEL_NO:
    case NPC_TK_TUNNEL_SO:
    case NPC_SD_M_AREA_FERRY:
        ret = true;
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}

bool IsShop (const uint32_t refCharID)
{
    bool ret;

    switch(refCharID)
    {
    case NPC_CH_SMITH:
    case NPC_CH_ARMOR:
    case NPC_CH_ACCESSORY:
    case NPC_CH_POTION:
    case NPC_CH_HORSE:
    case NPC_CH_SPECIAL:
    case NPC_CH_DOCTOR:
    case NPC_CH_GENARAL_SW:
    case NPC_WC_SMITH:
    case NPC_WC_ARMOR:
    case NPC_WC_ACCESSORY:
    case NPC_WC_POTION:
    case NPC_WC_HORSE:
    case NPC_WC_SPECIAL:
    case NPC_WC_DOCTOR:
    case NPC_WC_GENARAL_SW:
    case NPC_KT_SMITH:
    case NPC_KT_ARMOR:
    case NPC_KT_ACCESSORY:
    case NPC_KT_POTION:
    case NPC_KT_HORSE:
    case NPC_KT_SPECIAL:
    case NPC_KT_DESIGNER:
    case NPC_KT_MINISTER:
    case NPC_TD_THIEF_BUY:
    case NPC_TD_THIEF_SELL:
    case NPC_CH_SPECIAL2:
    case NPC_WC_SPECIAL2:
    case NPC_TK_SPECIAL:
    case NPC_RM_SPECIAL:
    case NPC_AM_SPECIAL:
    case NPC_CH_GENARAL_SP:
    case NPC_WC_GUILD:
    case NPC_KT_GUILD:
    case NPC_EU_GUILD:
    case NPC_EU_SMITH:
    case NPC_EU_ARMOR:
    case NPC_EU_ACCESSORY:
    case NPC_EU_POTION:
    case NPC_EU_HORSE:
    case NPC_EU_SPECIAL:
    case NPC_CA_SMITH:
    case NPC_CA_ARMOR:
    case NPC_CA_ACCESSORY:
    case NPC_CA_POTION:
    case NPC_CA_HORSE:
    case NPC_CA_SPECIAL:
    case NPC_EU_MERCHANT:
    case NPC_EU_HUNTER:
    case NPC_CA_MERCHANT:
    case NPC_CA_HUNTER:
    case NPC_CA_GUILD:
    case NPC_KT_ARMOR_SILSAMO:
    case NPC_CH_WAREHOUSE_M:
    case NPC_CH_WAREHOUSE_W:
    case NPC_WC_WAREHOUSE_M:
    case NPC_WC_WAREHOUSE_W:
    case NPC_KT_WAREHOUSE:
    case NPC_EU_WAREHOUSE:
    case NPC_CA_WAREHOUSE:
    case NPC_CH_FORTRESS_OFFICIAL:
    case NPC_KT_FORTRESS_OFFICIAL:
    case NPC_CH_FORTRESS_BATTLEAIDE1:
    case NPC_KT_FORTRESS_BATTLEAIDE:
    case NPC_CH_FORTRESS_BATTLEAIDE2:
    case NPC_SD_T_AREA_THIEF:
    case NPC_SD_MERCHANT_CHANGER:
    case NPC_SD_THIEF_AGENT_CHANGER:
    case NPC_SD_M_AREA_SMITH:
    case NPC_SD_M_AREA_ARMOR:
    case NPC_SD_M_AREA_ACCESSORY:
    case NPC_SD_M_AREA_POTION:
    case NPC_SD_M_AREA_HORSE:
    case NPC_SD_M_AREA_HUNTER:
    case NPC_SD_M_AREA_GUILD:
    case NPC_SD_T_AREA_SMITH:
    case NPC_SD_T_AREA_ARMOR:
    case NPC_SD_T_AREA_ACCESSORY:
    case NPC_SD_T_AREA_POTION:
    case NPC_SD_T_AREA_GUILD2:
    case NPC_SD_M_AREA_SPECIAL:
    case NPC_SD_M_AREA_SPECIAL2:
    case NPC_SD_M_AREA_SPECIAL3:
    case NPC_SD_M_AREA_MERCHANT:
    case NPC_SD_M_AREA_WAREHOUSE:
    case NPC_SD_T_AREA_WAREHOUSE2:
        ret = true;
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}

bool IsBlacksmithShop (const uint32_t refCharID)
{
    bool ret;

    switch(refCharID)
    {
    case NPC_CH_SMITH:
    case NPC_WC_SMITH:
    case NPC_KT_SMITH:
    case NPC_EU_GUILD:
    case NPC_EU_SMITH:
    case NPC_CA_SMITH:
    case NPC_SD_M_AREA_SMITH:
    case NPC_SD_T_AREA_SMITH:
        ret = true;
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}

bool IsProtectorShop (const uint32_t refCharID)
{
    bool ret;

    switch(refCharID)
    {
    case NPC_CH_ARMOR:
    case NPC_WC_ARMOR:
    case NPC_KT_ARMOR:
    case NPC_EU_ARMOR:
    case NPC_CA_ARMOR:
    case NPC_SD_M_AREA_ARMOR:
    case NPC_SD_T_AREA_ARMOR:
        ret = true;
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}


bool IsSpecialShop (const uint32_t refCharID)
{
    bool ret;

    switch(refCharID)
    {
    case NPC_CH_SPECIAL:
    case NPC_CH_SPECIAL2:
    case NPC_WC_SPECIAL:
    case NPC_WC_SPECIAL2:
    case NPC_KT_SPECIAL:
    case NPC_RM_SPECIAL:
    case NPC_EU_SPECIAL:
    case NPC_AM_SPECIAL:
    case NPC_CA_SPECIAL:
    case NPC_TK_SPECIAL:
    case NPC_SD_M_AREA_SPECIAL:
    case NPC_SD_M_AREA_SPECIAL2:
    case NPC_SD_M_AREA_SPECIAL3:
        ret = true;
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}

bool IsStorageManager (const uint32_t refCharID)
{
    bool ret;

    switch(refCharID)
    {
    case NPC_CH_WAREHOUSE_M:
    case NPC_CH_WAREHOUSE_W:
    case NPC_WC_WAREHOUSE_M:
    case NPC_WC_WAREHOUSE_W:
    case NPC_KT_WAREHOUSE:
    case NPC_EU_WAREHOUSE:
    case NPC_CA_WAREHOUSE:
    case NPC_SD_M_AREA_WAREHOUSE:
    case NPC_SD_T_AREA_WAREHOUSE2:
        ret = true;
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}

bool IsGuildManager (const uint32_t refCharID)
{
    bool ret;

    switch(refCharID)
    {
    case NPC_CH_GENARAL:
    case NPC_CH_GENARAL_SW:
    case NPC_CH_GENARAL_SP:
    case NPC_CH_GENARAL_BO:
    case NPC_WC_GENARAL_SW:
    case NPC_WC_GENARAL_SP:
    case NPC_WC_GENARAL_BO:
    case NPC_EU_GUILD:
    case NPC_WC_GUILD:
    case NPC_KT_GUILD:
    case NPC_CA_GUILD:
    case NPC_SD_M_AREA_GUILD:
    case NPC_SD_T_AREA_GUILD2:
    case NPC_SD_GUILD_AGENT:
    case NPC_SD_GUILD_AGENT2:
    case NPC_SD_GUILD_AGENT3:
        ret = true;
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}

bool IsJanganShop (const uint32_t refCharID)
{
    bool ret;

    switch(refCharID)
    {
    case NPC_CH_SMITH:
    case NPC_CH_ARMOR:
    case NPC_CH_POTION:
    case NPC_CH_POTION2:
    case NPC_CH_POTION3:
    case NPC_CH_ACCESSORY:
        ret = true;
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}

bool IsJanganTransport (const uint32_t refCharID)
{
    bool ret;

    switch(refCharID)
    {
    case NPC_CH_FERRY:
    case NPC_CH_FERRY2:
        ret = true;
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}

bool IsJanganSpecialShop (const uint32_t refCharID)
{
    bool ret;

    switch(refCharID)
    {
    case NPC_CH_SPECIAL:
    case NPC_CH_SPECIAL2:
        ret = true;
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}

bool IsHotanShop (const uint32_t refCharID)
{
    bool ret;

    switch(refCharID)
    {
    case NPC_KT_SMITH:
    case NPC_KT_ARMOR:
    case NPC_KT_POTION:
    case NPC_KT_ACCESSORY:
        ret = true;
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}

bool IsUniqueNPC (const uint32_t refCharID)
{
    bool ret;

    switch(refCharID)
    {
    case MOB_CH_TIGERWOMAN:
    case MOB_OA_URUCHI:
    case MOB_KK_ISYUTARU:
    case MOB_TK_BONELORD:
    case MOB_RM_TAHOMET:
    case MOB_RM_ROC:
    case MOB_EU_KERBEROS:
    case MOB_AM_IVY:
    case MOB_TQ_WHITESNAKE:
    case MOB_SD_HAROERIS:
    case MOB_SD_SETH:
    case MOB_SD_SELKIS:
    case MOB_SD_NEITH:
    case MOB_SD_ANUBIS:
    case MOB_SD_ISIS:
        ret = true;
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}
