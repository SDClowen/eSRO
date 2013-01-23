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

#include "skill_builder.hpp"

enum STATUS_EFFECT
{
    EFFECT_BURN = 25205,
    EFFECT_SHOCK = 25971,
    EFFECT_FROSTBITE = 26210,
    EFFECT_FREEZE = 26234,
    EFFECT_POISON = 28787,
    EFFECT_ZOMBIE = 31330
};

enum SKILL_CURSE
{
    CURSE_BLEED = 25196,
    CURSE_CONFUSE = 25441,
    CURSE_DARKNESS = 25710,
    CURSE_DISEASE = 25715,
    CURSE_FEAR = 26213,
    CURSE_SHORT_SIGHT = 28025,
    CURSE_BIND = 29300,
    CURSE_SLEEP = 29541,
    CURSE_DULL = 29548,
    CURSE_STUN = 29556,
    //CURSE_HIDDEN = 29794,
    CURSE_PANIC = 1668507760,
    CURSE_DIVISION = 1668508020,
    CURSE_WEAKEN = 1668509028,
    CURSE_COMBUSTION = 1668509040,
    CURSE_DECAY = 1668509796,
    CURSE_IMPOTENT = 1668510578,
    CURSE_HIDDEN = 84545280,
};

enum SKILL_DEFINITION
{
    BUILDER_KNOCKDOWN_BONUS = 25697,
    BUILDER_KNOCKBACK = 27490,
    BUILDER_KNOCKDOWN = 27503,

    BUILDER_UNK2 = 28003,
    BUILDER_ATTACK = 6386804,
    BUILDER_ATTACK_AREA = 6645362,
    BUILDER_UNK5 = 7564131,
    BUILDER_CURSE_RESIST = 23168512,
    BUILDER_CURE_CURSE = 25145280,
    BUILDER_EFFECTIVE_DISTANCE = 1633840738,
    BUILDER_UNK8 = 1635017569,
    BUILDER_EFFECT_RESIST = 1650946657,
    BUILDER_UNK9 = 1668182893,
    BUILDER_CURE_STATUS = 1668641396,
    BUILDER_UNK3 = 1684366962,
    BUILDER_UNK12 = 1684891508,	//TODO: CHANGE TO DAMAGE CONVERT TO HP AND MP
    BUILDER_AGGRO_REDUCE = 1685352052,
    BUILDER_SKILL_DURATION = 1685418593,
    BUILDER_PET_GROWTH = 1702391913,
    BUILDER_HEAL_AMOUNT = 1751474540,
    BUILDER_INVISIBLE = 1751737445,
    BUILDER_CHANGE_TARGET = 1751741549,
    BUILDER_CROWN_BUFF = 1752656242,
    BUILDER_HP_DRAIN = 1818653556,
    BUILDER_ABSORB_AGGRO = 1818976615,
    BUILDER_DAMAGE_DIVIDE = 1818977380,
    BUILDER_DAMAGE_2_MP = 1818977384,
    BUILDER_ABSORB_DAMAGE = 1818977394,
    BUILDER_RANGED_BUFF = 1819175795,
    BUILDER_TRANSFORM_MASK = 1835229552,
    BUILDER_UNK1 = 1836278627,
    BUILDER_UNK4 = 1836278632,
    BUILDER_WARLOCK_MAG_REFLECT = 1836541044,
    BUILDER_CLERIC_MAG_REFLECT = 1836542056,
    BUILDER_UNK6 = 1836542067,
    BUILDER_BARD_MAG_REFLECT = 1836543336,
    BUILDER_MP_CONSUME = 1869506150,
    BUILDER_UNK10 = 1870031922,
    BUILDER_UNK17 = 1885629746, //RELATED TO FW TOWER BUFF!!
    BUILDER_DAMAGE_ABSOLUTE = 1885629799,
    BUILDER_ATTACK_PREVENTION = 1886350433,
    BUILDER_SKILL_ACTIVATE = 1886743667,
    BUILDER_WARRIOR_REFLECT = 1886876788,
    BUILDER_CAPTURE_NPC = 1902474100,
    BUILDER_CURE_STATUS_AMOUNT = 1919120754,
    BUILDER_UNK7 = 1919246708,
    BUILDER_REQUIRED_KNOCKDOWN = 1919250787,
    BUILDER_REQUIRED_WEAPON = 1919250793,
    BUILDER_RESURRECT = 1919251317,
    BUILDER_BASE_SKILL = 1919776116,
    BUILDER_MAX_TAMBOUR_CASTED = 1935895667,
    BUILDER_SKILL_BONUS = 1936028790,
    BUILDER_NPC_SPAWN = 1936945013,
    BUILDER_UNK14 = 1937010291,
    BUILDER_BOW_CALL = 1937075565,
    BUILDER_EFFECT_UNK1 = 1952542324,
    BUILDER_TELEPORT = 1952803890,
    BUILDER_UNK15 = 1952803891,
    BUILDER_TELEPORT_V2 = 1952803941,
    BUILDER_AGGRO_INC = 1953395762
};

enum EMPTY_TAGS
{
    END_TAG1 = 1734702198,
    END_TAG2 = 1160921409,
    END_TAG3 = 1160921416,
    END_TAG4 = 1160860481,
    END_TAG5 = 1160926017,
    END_TAG6 = 1464421700,
    END_TAG7 = 1464422997,
    END_TAG8 = 1161904468,
    END_TAG9 = 1111772484,
    END_TAG10 = 1297432916,
    END_TAG11 = 1112293716,
    END_TAG12 = 1212957012,
    END_TAG13 = 1668641388,
    END_TAG14 = 1702062192,
    END_TAG15 = 1650619750,
    END_TAG16 = 1667396966,
    END_TAG17 = 1380992085,
    END_TAG18 = 1380996181,
    END_TAG19 = 1380991573,
    END_TAG20 = 1212957264,
    END_TAG21 = 1398031445,
    END_TAG22 = 1398035280,
    END_TAG23 = 1953653104,
    END_TAG24 = 1919246700,
    END_TAG25 = 1701213281,
    END_TAG26 = 1146307922,
    END_TAG27 = 1146307410,
    END_TAG28 = 1212960073,
    END_TAG29 = 1212958291,
    END_TAG30 = 1818981170,
    END_TAG31 = 1752069232
};

SkillBuilder::SkillBuilder ()
{
    m_builder_table[EFFECT_FROSTBITE] = &SkillBuilder::OnCurse;
    m_builder_table[EFFECT_FREEZE] = &SkillBuilder::OnCurse;
    m_builder_table[EFFECT_ZOMBIE] = &SkillBuilder::OnCurse;
    m_builder_table[EFFECT_BURN] = &SkillBuilder::OnCurse;
    m_builder_table[EFFECT_SHOCK] = &SkillBuilder::OnCurse;
    m_builder_table[EFFECT_POISON] = &SkillBuilder::OnCurse;
    m_builder_table[CURSE_CONFUSE] = &SkillBuilder::OnCurse;
    m_builder_table[CURSE_DARKNESS] = &SkillBuilder::OnCurse;
    m_builder_table[CURSE_DISEASE] = &SkillBuilder::OnCurse;
    m_builder_table[CURSE_FEAR] = &SkillBuilder::OnCurse;
    m_builder_table[CURSE_SHORT_SIGHT] = &SkillBuilder::OnCurse;
    m_builder_table[CURSE_BIND] = &SkillBuilder::OnCurse;
    m_builder_table[CURSE_SLEEP] = &SkillBuilder::OnCurse;
    m_builder_table[CURSE_DULL] = &SkillBuilder::OnCurse;
    m_builder_table[CURSE_STUN] = &SkillBuilder::OnCurse;
    m_builder_table[CURSE_DIVISION] = &SkillBuilder::OnCurse;
    m_builder_table[CURSE_WEAKEN] = &SkillBuilder::OnCurse;
    m_builder_table[CURSE_DECAY] = &SkillBuilder::OnCurse;
    m_builder_table[CURSE_IMPOTENT] = &SkillBuilder::OnCurse;
    m_builder_table[CURSE_BLEED] = &SkillBuilder::OnCurse;
    m_builder_table[CURSE_PANIC] = &SkillBuilder::OnCurse;
    m_builder_table[CURSE_COMBUSTION] = &SkillBuilder::OnCurse;
    m_builder_table[CURSE_HIDDEN] = &SkillBuilder::OnCurse;

	m_builder_table[BUFF_BLOCK_INC] = &SkillBuilder::OnBuff_2Arg;
	m_builder_table[BUFF_CRITICAL_INC] = &SkillBuilder::OnBuff_2Arg;
	m_builder_table[BUFF_PARRY_INC] = &SkillBuilder::OnBuff_2Arg;
	m_builder_table[BUFF_HIT_INC] = &SkillBuilder::OnBuff_2Arg;
	m_builder_table[BUFF_RANGE_INC] = &SkillBuilder::OnBuff_1Arg;
	m_builder_table[BUFF_DAMAGE_INC] = &SkillBuilder::OnBuff_2Arg;
	m_builder_table[BUFF_GOLD_DROP_INC] = &SkillBuilder::OnBuff_1Arg;
	m_builder_table[BUFF_HP_INC] = &SkillBuilder::OnBuff_2Arg;
	m_builder_table[BUFF_MP_INC] = &SkillBuilder::OnBuff_2Arg;
	m_builder_table[BUFF_MAGICOPTION_LUCK_INC] = &SkillBuilder::OnBuff_1Arg;
	m_builder_table[BUFF_CRIT_PARRY_INC] = &SkillBuilder::OnBuff_1Arg;
	m_builder_table[BUFF_DEFENSE_INC] = &SkillBuilder::OnBuff_3Arg;
	m_builder_table[BUFF_SPEED_INC2] = &SkillBuilder::OnBuff_1Arg;
	m_builder_table[BUFF_SPEED_INC3] = &SkillBuilder::OnBuff_1Arg;
	m_builder_table[BUFF_SPEED_INC] = &SkillBuilder::OnBuff_1Arg;
	m_builder_table[BUFF_INT_INC] = &SkillBuilder::OnBuff_2Arg;
	m_builder_table[BUFF_ENCHANT_LUCK_INC] = &SkillBuilder::OnBuff_1Arg;
	m_builder_table[BUFF_IGNORE_DEFENSE] = &SkillBuilder::OnBuff_1Arg;
	m_builder_table[BUFF_STR_INC] = &SkillBuilder::OnBuff_2Arg;
	m_builder_table[BUFF_PARRY_DEC] = &SkillBuilder::OnBuff_1Arg;
	m_builder_table[BUFF_HIT_DEC] = &SkillBuilder::OnBuff_1Arg;
	m_builder_table[BUFF_SHIELD_PWR_4_DAMAGE] = &SkillBuilder::OnBuff_2Arg;
	m_builder_table[BUFF_REINCARNATION] = &SkillBuilder::OnBuff_2Arg;
	m_builder_table[BUFF_DAMAGE_PWR_INC] = &SkillBuilder::OnBuff_2Arg;
	m_builder_table[BUFF_MP_ABSORB] = &SkillBuilder::OnBuff_1Arg;
	m_builder_table[BUFF_ABSORB_WALL] = &SkillBuilder::OnBuff_4Arg;
	m_builder_table[BUFF_DETECT] = &SkillBuilder::OnBuff_2Arg;
	m_builder_table[BUFF_DETECT_V2] = &SkillBuilder::OnBuff_2Arg;
	m_builder_table[BUFF_DAMAGE_REFLECT] = &SkillBuilder::OnBuff_4Arg;
	m_builder_table[BUFF_REQUIRED_MP_DEC] = &SkillBuilder::OnBuff_1Arg;
	m_builder_table[BUFF_HEAL_INC] = &SkillBuilder::OnBuff_2Arg;
	m_builder_table[BUFF_ABSORB] = &SkillBuilder::OnBuff_3Arg;
	m_builder_table[BUFF_DAMAGE_DEC] = &SkillBuilder::OnTimedBuff;
	m_builder_table[BUFF_DEFENSE_DEC] = &SkillBuilder::OnTimedBuff;
	m_builder_table[BUFF_HP_DEC] = &SkillBuilder::OnTimedBuff;

	m_builder_table[BUFF_END_HP_LOST] = &SkillBuilder::OnBuffEnd;
	m_builder_table[BUFF_END_MP_LOST] = &SkillBuilder::OnBuffEnd;

	m_builder_table[BUILDER_KNOCKDOWN_BONUS] = &SkillBuilder::OnKnockdownBonus;
	m_builder_table[BUILDER_KNOCKBACK] = &SkillBuilder::OnKnockback;
	m_builder_table[BUILDER_KNOCKDOWN] = &SkillBuilder::OnKnockdown;
	m_builder_table[BUILDER_ATTACK] = &SkillBuilder::OnAttack;
	m_builder_table[BUILDER_WARLOCK_MAG_REFLECT] = &SkillBuilder::OnMagicReflect;
	m_builder_table[BUILDER_CLERIC_MAG_REFLECT] = &SkillBuilder::OnMagicReflect;
	m_builder_table[BUILDER_BARD_MAG_REFLECT] = &SkillBuilder::OnMagicReflect;
	m_builder_table[BUILDER_WARRIOR_REFLECT] = &SkillBuilder::OnPhysicReflect;
	m_builder_table[BUILDER_CHANGE_TARGET] = &SkillBuilder::OnChangeTarget;
	m_builder_table[BUILDER_DAMAGE_ABSOLUTE] = &SkillBuilder::OnDamageAbsolute;
	m_builder_table[BUILDER_AGGRO_REDUCE] = &SkillBuilder::OnAggroReduce;
	m_builder_table[BUILDER_SKILL_DURATION] = &SkillBuilder::OnSkillDuration;
	m_builder_table[BUILDER_HEAL_AMOUNT] = &SkillBuilder::OnHealAmount;
	m_builder_table[BUILDER_AGGRO_INC] = &SkillBuilder::OnAggroInc;
	m_builder_table[BUILDER_ATTACK_AREA] = &SkillBuilder::OnAttackArea;

	m_builder_table[BUILDER_UNK2] = &SkillBuilder::OnUnk2;
	m_builder_table[BUILDER_UNK5] = &SkillBuilder::OnUnk5;
	m_builder_table[BUILDER_CURSE_RESIST] = &SkillBuilder::OnCurseResist;
	m_builder_table[BUILDER_CURE_CURSE] = &SkillBuilder::OnCureCurse;
	m_builder_table[BUILDER_EFFECTIVE_DISTANCE] = &SkillBuilder::OnEffectiveDistance;
	m_builder_table[BUILDER_UNK8] = &SkillBuilder::OnUnk8;
	m_builder_table[BUILDER_EFFECT_RESIST] = &SkillBuilder::OnEffectResist;
	m_builder_table[BUILDER_UNK9] = &SkillBuilder::OnUnk9;
	m_builder_table[BUILDER_CURE_STATUS] = &SkillBuilder::OnCureStatus;
	m_builder_table[BUILDER_UNK3] = &SkillBuilder::OnUnk3;
	m_builder_table[BUILDER_UNK12] = &SkillBuilder::OnUnk12;
	m_builder_table[BUILDER_PET_GROWTH] = &SkillBuilder::OnPetGrowth;
	m_builder_table[BUILDER_INVISIBLE] = &SkillBuilder::OnInvisible;
	m_builder_table[BUILDER_CROWN_BUFF] = &SkillBuilder::OnCrownBuff;
	m_builder_table[BUILDER_HP_DRAIN] = &SkillBuilder::OnHPDrain;
	m_builder_table[BUILDER_ABSORB_AGGRO] = &SkillBuilder::OnAbsorbAggro;
	m_builder_table[BUILDER_DAMAGE_DIVIDE] = &SkillBuilder::OnDamageDivide;
	m_builder_table[BUILDER_ABSORB_DAMAGE] = &SkillBuilder::OnAbsorbDamage;
	m_builder_table[BUILDER_RANGED_BUFF] = &SkillBuilder::OnRangedBuff;
	m_builder_table[BUILDER_TRANSFORM_MASK] = &SkillBuilder::OnTransformMask;
	m_builder_table[BUILDER_UNK1] = &SkillBuilder::OnUnk1;
	m_builder_table[BUILDER_UNK4] = &SkillBuilder::OnUnk4;
	m_builder_table[BUILDER_MP_CONSUME] = &SkillBuilder::OnMPConsume;
	m_builder_table[BUILDER_UNK10] = &SkillBuilder::OnUnk10;
	m_builder_table[BUILDER_UNK17] = &SkillBuilder::OnUnk17;
	m_builder_table[BUILDER_ATTACK_PREVENTION] = &SkillBuilder::OnAttackPrevention;
	m_builder_table[BUILDER_SKILL_ACTIVATE] = &SkillBuilder::OnSkillActivate;
	m_builder_table[BUILDER_CAPTURE_NPC] = &SkillBuilder::OnCaptureNPC;
	m_builder_table[BUILDER_CURE_STATUS_AMOUNT] = &SkillBuilder::OnCureStatusAmount;
	m_builder_table[BUILDER_UNK6] = &SkillBuilder::OnUnk6;
	m_builder_table[BUILDER_UNK7] = &SkillBuilder::OnUnk7;
	m_builder_table[BUILDER_REQUIRED_KNOCKDOWN] = &SkillBuilder::OnRequiredKnockdown;
	m_builder_table[BUILDER_REQUIRED_WEAPON] = &SkillBuilder::OnRequiredWeapon;
	m_builder_table[BUILDER_RESURRECT] = &SkillBuilder::OnResurrect;
	m_builder_table[BUILDER_BASE_SKILL] = &SkillBuilder::OnBaseSkill;
	m_builder_table[BUILDER_MAX_TAMBOUR_CASTED] = &SkillBuilder::OnMaxTambourCasted;
	m_builder_table[BUILDER_SKILL_BONUS] = &SkillBuilder::OnSkillBonus;
	m_builder_table[BUILDER_NPC_SPAWN] = &SkillBuilder::OnNPCSpawn;
	m_builder_table[BUILDER_BOW_CALL] = &SkillBuilder::OnBowCall;
	m_builder_table[BUILDER_UNK14] = &SkillBuilder::OnUnk14;
	m_builder_table[BUILDER_EFFECT_UNK1] = &SkillBuilder::OnEffectUnk1;
	m_builder_table[BUILDER_TELEPORT] = &SkillBuilder::OnTeleport;
	m_builder_table[BUILDER_UNK15] = &SkillBuilder::OnUnk15;
	m_builder_table[BUILDER_TELEPORT_V2] = &SkillBuilder::OnTeleport;
	m_builder_table[BUILDER_DAMAGE_2_MP] = &SkillBuilder::OnDamage2MP;

	m_builder_table[END_TAG1] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG2] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG3] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG4] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG5] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG6] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG7] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG8] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG9] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG10] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG11] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG12] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG13] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG14] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG15] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG16] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG17] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG18] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG19] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG20] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG21] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG22] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG23] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG24] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG25] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG26] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG27] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG28] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG29] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG30] = &SkillBuilder::OnEnd;
	m_builder_table[END_TAG31] = &SkillBuilder::OnEnd;
}

void SkillBuilder::SkillBuilder::build (const SkillProperty &property)
{
	m_Index = 0;
	m_Property = property;
    m_NextBuilder = m_NextBuilder = m_Property.Effects[m_Index++];

	m_SkillObj.reset();

	m_SkillObj = boost::shared_ptr<Skill>(new Skill(property));

	do
	{
		std::map<uint32_t,builderfunc>::iterator i = m_builder_table.find(m_NextBuilder);

		if (i != m_builder_table.end())
		{
			builderfunc function = i->second;

			if (function)
				(this->*function)();

			if (m_Index >= MAX_INDEX)
				break;

            m_NextBuilder = m_Property.Effects[m_Index++];
		}
		else
		{
			m_NextBuilder = 0;
		}
	}
	while (m_NextBuilder != 0 && m_Index < MAX_INDEX);
}

boost::shared_ptr<Skill> SkillBuilder::get_built_object () const
{
	return m_SkillObj;
}

void SkillBuilder::SkillBuilder::OnEnd ()
{
}

void SkillBuilder::OnCurse ()
{
    Status status;

    switch(m_Property.Effects[m_Index-1])
    {
    case EFFECT_FROSTBITE:
        status.ID = STATUS_FROST;
        break;
    case EFFECT_FREEZE:
        status.ID = STATUS_FROZE;
        break;
    case EFFECT_ZOMBIE:
        status.ID = STATUS_ZOMBIE;
        break;
    case EFFECT_BURN:
        status.ID = STATUS_BURN;
        break;
    case EFFECT_SHOCK:
        status.ID = STATUS_SHOCK;
        break;
    case EFFECT_POISON:
        status.ID = STATUS_POISON;
        break;
    case CURSE_CONFUSE:
        status.ID = STATUS_CONFUSION;
        break;
    case CURSE_FEAR:
        status.ID = STATUS_FEAR;
        break;
    case CURSE_BIND:
        status.ID = STATUS_PANIC;
        break;
    case CURSE_SLEEP:
        status.ID = STATUS_SLEEP;
        break;
    case CURSE_DULL:
        status.ID = STATUS_DULL;
        break;
    case CURSE_STUN:
        status.ID = STATUS_STUN;
        break;
    case CURSE_DARKNESS:
        status.ID = STATUS_DARKNESS;
        break;
    case CURSE_DISEASE:
        status.ID = STATUS_DISEASE;
        break;
    case CURSE_SHORT_SIGHT:
        status.ID = STATUS_SHORT_SIGHT;
        break;
    case CURSE_DIVISION:
        status.ID = STATUS_DIVISION;
        break;
    case CURSE_WEAKEN:
        status.ID = STATUS_WEAKEN;
        break;
    case CURSE_DECAY:
        status.ID = STATUS_DECAY;
        break;
    case CURSE_IMPOTENT:
        status.ID = STATUS_IMPOTENT;
        break;
    case CURSE_PANIC:
        status.ID = STATUS_PANIC;
        break;
    case CURSE_COMBUSTION:
        status.ID = STATUS_COMBUSTION;
        break;
    case CURSE_BLEED:
        status.ID = STATUS_BLEED;
        break;
    case CURSE_HIDDEN:
        status.ID = STATUS_HIDDEN;
        break;
    default:
        break;
    }

    int offset = 0;

    /// READ BASE INFO FOR BAD STATUS
    switch(m_Property.Effects[m_Index-1])
    {
    case EFFECT_FROSTBITE:
    case EFFECT_FREEZE:
    case EFFECT_ZOMBIE:
        status.Effect = m_Property.Effects[m_Index];
        status.Probability = m_Property.Effects[m_Index+1];
        status.Duration = status.Effect*100;
        offset = 2;
        break;
    case EFFECT_BURN:
    case EFFECT_SHOCK:
    case EFFECT_POISON:
        status.Effect = m_Property.Effects[m_Index];
        status.Probability = m_Property.Effects[m_Index+1];
        status.Payload = m_Property.Effects[m_Index+2];
        status.Duration = status.Effect*100;
        offset = 3;
        break;
    default:
        break;
    }

    /// READ BASE INFO FOR DEBUFF
    switch(m_Property.Effects[m_Index-1])
    {
    case CURSE_CONFUSE:
    case CURSE_FEAR:
    case CURSE_BIND:
    case CURSE_SLEEP:
    case CURSE_DULL:
    case CURSE_STUN:
    case CURSE_DARKNESS:
    case CURSE_DISEASE:
    case CURSE_SHORT_SIGHT:
    case CURSE_DIVISION:
    case CURSE_WEAKEN:
    case CURSE_DECAY:
    case CURSE_IMPOTENT:
    case CURSE_PANIC:
    case CURSE_COMBUSTION:
        status.Duration = m_Property.Effects[m_Index];
        status.Probability = m_Property.Effects[m_Index+1];
        status.Level = m_Property.Effects[m_Index+2];
        offset = 3;
        break;
    case CURSE_HIDDEN:
        status.Probability = m_Property.Effects[m_Index];
        status.Level = m_Property.Effects[m_Index+1];
        status.Payload = m_Property.Effects[m_Index+2];
        offset = 3;
        break;
    case CURSE_BLEED:
        //DURATION [MS]
        //UNK
        //LEVEL
        //EFFECT
        //PROBABILITY (%)
        offset = 5;
        break;
    default:
        break;
    }

    /// READ PAYLOAD DATA FOR CURSES
    switch(m_Property.Effects[m_Index-1])
    {
    case CURSE_DARKNESS:
    case CURSE_DISEASE:
    case CURSE_SHORT_SIGHT:
    case CURSE_DIVISION:
    case CURSE_WEAKEN:
    case CURSE_DECAY:
    case CURSE_IMPOTENT:
        status.Payload = m_Property.Effects[m_Index+3];
        offset = 4;
        break;
    case CURSE_PANIC:
        //0
        //50
        offset = 6;
        break;
    case CURSE_COMBUSTION:
        //unk1 (30,50)
        //50
        //5 +6
        offset = 7;
        break;
    default:
        break;
    }

    m_SkillObj->m_status_list.push_back(status);

    m_Index += offset;
}

void SkillBuilder::OnBuff (int argc)
{
	BuffEffect buff;
    buff.ID = static_cast<SKILL_BUFF>(m_Property.Effects[m_Index-1]);

	for (int i=0; i < argc; ++i)
        buff.Arg[i] = m_Property.Effects[m_Index++];

	m_SkillObj->add_buff(buff);
}

void SkillBuilder::OnBuff_1Arg ()
{
	OnBuff(1);
}

void SkillBuilder::OnBuff_2Arg ()
{
	OnBuff(2);
}

void SkillBuilder::OnBuff_3Arg ()
{
	OnBuff(3);
}

void SkillBuilder::OnBuff_4Arg ()
{
	OnBuff(4);
}

void SkillBuilder::OnTimedBuff ()
{
    m_SkillObj->add_duration(m_Property.Effects[m_Index]);

	OnBuff(4);
}

void SkillBuilder::OnBuffEnd ()
{
	BuffEnd buff;
    buff.ID = static_cast<SKILL_BUFF_END>(m_Property.Effects[m_Index-1]);
    buff.Arg0 = m_Property.Effects[m_Index];

	m_SkillObj->add_buff_end(buff);
	//HP LOST AT THE END OF THE BUFF
	//HP (%)

	m_Index++;
}

void SkillBuilder::OnRangedBuff ()
{
	//RELATED TO FIRE TRAP!!

	//Type  (0 None )(1 Phy fence)(2 Mag Fence)(3 Protect)(5 str)(6 int)(13 Stealth point)(14 Fire trap)(15 mp steal)
	//Distance
	//Max Amount
	//Linked to char flag

	//FOR FW REPAIR KIT (0,0,1,1)

	m_Index += 4;
}

void SkillBuilder::OnKnockdownBonus ()
{
    m_SkillObj->add_knockdown_bonus(m_Property.Effects[m_Index]);

	m_Index++;
}

void SkillBuilder::OnKnockback ()
{
    m_SkillObj->m_KBProbability = m_Property.Effects[m_Index];
    m_SkillObj->m_KBDistance = m_Property.Effects[m_Index+1];

	m_Index += 2;
}

void SkillBuilder::OnKnockdown ()
{
    m_SkillObj->m_KDProbability = m_Property.Effects[m_Index];
    m_SkillObj->m_KDLevel = m_Property.Effects[m_Index+1];

	m_Index += 2;
}

void SkillBuilder::OnUnk2 ()
{
	//2
	//unk (1,2,3,4,5)

	m_Index += 2;

    if (!m_Property.Effects[m_Index] && m_Property.Effects[m_Index+1])
		++m_Index;
}

void SkillBuilder::OnSkillDuration ()
{
    m_SkillObj->add_duration(m_Property.Effects[m_Index]);

	m_Index++;
}

void SkillBuilder::OnPetGrowth ()
{
	//0
	//100 [%] I think its XP increase

	m_Index += 2;
}

void SkillBuilder::OnHealAmount ()
{
	//HP amount
	//(1,2,3 FW repair kit) (5 Jangan traps)
	//MP amount
	//0
    m_SkillObj->add_heal_amount(m_Property.Effects[m_Index],m_Property.Effects[m_Index+2]);

	m_Index += 4;
}

void SkillBuilder::OnInvisible ()
{
	//(1 Stealth) (2 INVISIBLE) (4 Fire Trap)
	//LEVEL
	//50 ???

	m_Index += 3;
}

void SkillBuilder::OnChangeTarget ()
{
	m_SkillObj->set_change_target(true);
}

void SkillBuilder::OnCrownBuff ()
{
	//RELATED TO CROWN BUFF GOT FROM ACADEMY 20163
	//Percent
	m_Index++;
}

void SkillBuilder::OnHPDrain ()
{
	//Amount

	m_Index++;
}

void SkillBuilder::OnAbsorbAggro ()
{
	//Amount (%)
	//0

	m_Index += 2;
}

void SkillBuilder::OnDamageDivide ()
{
	//Amount (%)
	m_Index++;
}

void SkillBuilder::OnAbsorbDamage ()
{
	//Type (4 physical) (8 magical)
	//Amount (%)
	//0

	m_Index += 3;
}

void SkillBuilder::OnDamage2MP ()
{
	//0
	//Damage converted to 2 MP (%)
	//Amount absorbed

	m_Index += 3;
}

void SkillBuilder::OnTransformMask ()
{
	//Max mob level
	//10364

	m_Index += 2;
}

void SkillBuilder::OnUnk14 ()
{
	//RELATED TO MEDUSA ATTACK
	//unk1 5000
	//unk2 12
	//unk3 5000

	m_Index += 3;
}

void SkillBuilder::OnAttack ()
{
	AttackInfo atk;
    atk.type = static_cast<ATTACK_TYPE>(m_Property.Effects[m_Index]);
    atk.percent = m_Property.Effects[m_Index+1];
    atk.min_pwr = m_Property.Effects[m_Index+2];
    atk.max_pwr = m_Property.Effects[m_Index+3];
	//unk +4

	m_SkillObj->add_attack(atk);

	m_Index += 5;
}

void SkillBuilder::OnAttackArea ()
{
	//UNK +0
	//ATTACK REFERENCE +1
	//ATTACK DISTANCE +2
	//MAX TARGETS +3
	//DAMAGE REDUCE +4
	//UNK2 +5

	AttackArea area;
    area.reference = static_cast<AREA_REFERENCE>(m_Property.Effects[m_Index+1]);
    area.distance = m_Property.Effects[m_Index+2];
    area.max_targets = m_Property.Effects[m_Index+3];
    area.damage_reduce = m_Property.Effects[m_Index+4];

	m_SkillObj->add_area(area);

	m_Index += 6;
}

void SkillBuilder::OnUnk5 ()
{
	//RELATED TO INVISIBLE
	//12
	//10
	//0

	//for dance (15,0,80)
	//For rogue copy and warlock ilussion (0,2,0)

	m_Index += 3;
}

void SkillBuilder::OnEffectResist ()
{
	//63
	//Max Effect
	//0

	m_Index += 3;
}

void SkillBuilder::OnCurseResist ()
{
	//PROBABILITY RESIST INC (%)
	//MAX LEVEL

	m_Index += 2;
}

 void SkillBuilder::OnCureCurse ()
 {
	//RELATED TO CURE CURSE
	//unk 80,100 +0
	//Level

	m_Index += 2;
 }

void SkillBuilder::OnEffectiveDistance ()
{
	//Distance

	m_Index++;
}

void SkillBuilder::OnUnk8 ()
{
	//std::cout << "OnUnk8()" << std::endl;

	//unk1 (16832) +0
	//unk2 (50%) +1 [BONUS DAMAGE INCREASE ??]

	m_Index += 2;
}

void SkillBuilder::OnUnk9 ()
{
	//std::cout << "OnUnk9()" << std::endl;

	//unk1 (4) +0
	//unk2 (2 - EU XBow, 1 - CH Bow) +1
	//unk3 (1) +2

	m_Index += 3;
}

void SkillBuilder::OnCureStatus ()
{
	//unk (63)
	//Effect to cure

	m_Index += 2;
}

void SkillBuilder::OnUnk3 ()
{
	//RELATED TO TRAPS BUFF

	m_Index += 2;
}

void SkillBuilder::OnUnk12 ()
{
    m_SkillObj->add_damage_conversion(m_Property.Effects[m_Index],m_Property.Effects[m_Index+1]);
	//Absorb HP(%)
	//Absorb MP(%)

	m_Index += 2;
}

void SkillBuilder::OnAggroReduce ()
{
    m_SkillObj->add_aggro_reduce(m_Property.Effects[m_Index]);
	//Amount
	//0

	m_Index += 2;
}

void SkillBuilder::OnUnk1 ()
{
	m_Index += 1;
}

void SkillBuilder::OnUnk4 ()
{
	//RELATED TO OBLATION
	//(1 - Monster mask i think) (2 Rogue copy skill) (3 Warlock ilussion) (4 - Oblation)
	//Max level

	m_Index += 2;
}

void SkillBuilder::OnMagicReflect ()
{
    m_SkillObj->set_magical_reflect(m_Property.Effects[m_Index]);

	++m_Index;
}

void SkillBuilder::OnPhysicReflect ()
{
    m_SkillObj->set_physical_reflect(m_Property.Effects[m_Index]);

	++m_Index;
}

void SkillBuilder::OnMPConsume ()
{   
    m_SkillObj->m_MPConsumeTime = m_Property.Effects[m_Index];
    m_SkillObj->m_MPConsume = m_Property.Effects[m_Index+1];

	m_Index += 2;
}

void SkillBuilder::OnUnk10 ()
{
	//RELATED TYPE OF BUFF

	//(53 earth barrier),(34 discord wave) (56 dance warrior) (57 dance magic)
	//(63 dance heal) (64 dance mp) (30 Guard Tambour) (31 Mana Tambour) (61 Hit tambour)

	m_Index++;
}

void SkillBuilder::OnUnk17 ()
{
	//RELATED MSKILL_TQ_BOMBSTONEGHOST_ATTACK05

	m_Index++;
}

void SkillBuilder::OnDamageAbsolute ()
{
    m_SkillObj->add_absolute_damage(m_Property.Effects[m_Index]);

	m_Index++;
}

void SkillBuilder::OnAttackPrevention ()
{
	//Max number of players.
	//Max level of attack prevention.

	m_Index += 2;
}

//Perform the action in a period of time

void SkillBuilder::OnSkillActivate ()
{
	//RELATED TO Healing and mana cycle

	//Time [ms]

	m_Index++;
}

void SkillBuilder::OnCaptureNPC ()
{
	//Amount
	//ID1
	//ID2
	//ID3

	m_Index += 4;
}

void SkillBuilder::OnCureStatusAmount ()
{
	//COUNT

	m_Index++;
}

void SkillBuilder::OnUnk6 ()
{
	//80 (Probly reflect %) RELATED TO VAMPIRE KISS AND TOUCH

	m_Index++;
}

void SkillBuilder::OnUnk7 ()
{
	//RELATED TO CLERIC ABNORMAL_B (672)
	//63 (32 Rogue poison circle)
	//EFFECT

	m_Index +=2;
}

void SkillBuilder::OnRequiredKnockdown ()
{
	//std::cout << "OnRequiredKnockdown()" << std::endl;

	//FLAG REQUIRED (1 - knockdown) (32 - tambour probly)

	m_Index++;
}

void SkillBuilder::OnRequiredWeapon ()
{
    /// TYPE (4 SHIELD) (6 WEAPON) (14 DEVIL AVATAR) (10 ARMOR)
    /// ITEM (7 EUROPEAN 1H) (8 EUROPEAN 2H) (9 EUROPEAN AXE)
	/// (12 EU XBOW) (13 EU Daggers) (15 EU CLERIC ROD) (6 CH BOW) (11 2H Staff) (10 darkstaff)

    switch(m_Property.Effects[m_Index])
	{
	case ESS_SHIELD:
		///(1 CHINESE SHIELD) (2 EUROPEAN SHIELD)
		m_SkillObj->add_shield_requirement(true);
		break;
	case ESS_WEAPON:
        m_SkillObj->add_weapon_requirement(static_cast<REQ_WEAPON_TYPE>(m_Property.Effects[m_Index+1]));
		break;
	case ESS_ARMOR:
		///10 and 0 probly required light armor
        m_SkillObj->add_armor_requirement(static_cast<REQ_ARMOR_TYPE>(m_Property.Effects[m_Index+1]));
		break;
	case ESS_DEVIL:
		///14 and 1 devil avatar
		m_SkillObj->add_devil_requirement(true);
		break;
	default:
		break;
	}

	m_Index += 2;
}

void SkillBuilder::OnResurrect ()
{
	//Max level
	//Experience (%) of the lost XP gained.

	m_Index += 2;
}

void SkillBuilder::OnBaseSkill ()
{
	//ID

	m_Index++;
}

void SkillBuilder::OnMaxTambourCasted ()
{
	//Amount

	m_Index++;
}

void SkillBuilder::OnSkillBonus ()
{
	//TYPE
	//Bonus (%)

	/*
	TYPES MEANING 0
	1111772484 Bard lower mp consumption
	1112293716 Warlock nuke damage inc
	1112754256 Warlock Shadow skill line inc damage **CHECK**
	1396785473 Warlock draining hp inc
	1128415572 XBow damage inc
	1128419905 XBow range inc
	1129267540 Wizard cold damage inc
	1145520449 Dagger stealth mode damage inc
	1145520468 Dagger damage inc
	1145522258 Dagger hit rate inc
	1146307410 Bard dance resist
	1146307922 Bard dance range inc
	1146372436 Warlock DoT damage inc
	1146373202 Warlock DoT time inc
	1160860481 1h damage inc
	1160921409 Dual axe damage inc
	1160921416 Dual axe hit rate inc
	1160926017 2H damage inc
	1161904468 Wizard earth damage inc
	1179205972 Wizard fire damage inc
	1212957012 Cleric battle line damage inc
	1279869268 Wizard light damage inc
	1297432916 Bard damage inc
	1297433426 Bard tambour resist
	1297433938 Bard tambour range inc
	1380991573 Dagger poison time inc
	1380992085 Dagger poison damage inc
	1380996181 Dagger poison effect inc
	1398031445 Rogue stealth time inc
	1398035280 Rogue stealth speed inc;
	1414676801 Warlock trap damage inc
	1464421700 Wizard mana consumed dec
	1464422997 Wizard range inc
	1212958291 ???
	1212961365 ???

	*/

	m_Index += 3;
}

void SkillBuilder::OnNPCSpawn ()
{
    NPC_SPAWN_INFO info;

    for (int i = 0; i < 9; ++i)
    {
        info.refCharID = m_Property.Effects[m_Index+4*i+0];

        if (!info.refCharID)
            break;

        info.GiantCount = m_Property.Effects[m_Index+4*i+1];
        info.ChampionCount = m_Property.Effects[m_Index+4*i+2];
        info.NormalCount = m_Property.Effects[m_Index+4*i+3];

        m_SkillObj->m_spawn_list.push_back(info);
    }

	m_Index += 36;
}

void SkillBuilder::OnBowCall ()
{
	//Duration [MS]
	//10
	//Attack period [MS]
	//Damage

	m_Index += 4;
}

void SkillBuilder::OnEffectUnk1 ()
{
	//UNK +0
	//0 +1

	m_Index += 2;
}

void SkillBuilder::OnTeleport ()
{
	//500
    m_SkillObj->m_TeleportDistance = m_Property.Effects[m_Index+1];

	m_Index += 2;
}

void SkillBuilder::OnUnk15 ()
{
	//RELATED TO SPRINT ASSAULT MAYBE ATTACK RANGE

	//500
	//unk2

	m_Index += 2;
}

void SkillBuilder::OnAggroInc ()
{
    m_SkillObj->add_aggro(m_Property.Effects[m_Index],m_Property.Effects[m_Index+1]);

	m_Index += 2;
}
