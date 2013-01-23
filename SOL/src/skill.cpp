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

#include "skill.hpp"

Skill::Skill ()
{
	m_ID = 0;
	m_GroupID = 0;
	m_Type = SKT_NONE;
	m_Level = 0;
	m_SP = 0;
	m_Race = RACE_NONE;
	m_Mastery = 0;
	m_MasteryLvl = 0;
	m_GroupSkill1 = 0;
	m_GroupSkillLv1 = 0;
	m_GroupSkill2 = 0;
	m_GroupSkillLv2 = 0;
	m_Weapon1 = REQ_WEAPON_ANY;
	m_Weapon2 = REQ_WEAPON_ANY;
	m_MP = 0;
	m_HP = 0;

	m_Duration = 0;
	m_Init = 0;
	m_Delay = 0;
	m_NextDelay = 0;

	m_Attack.min_pwr = 0;
	m_Attack.max_pwr = 0;
	m_Attack.percent = 0;
	m_Attack.type = ATTACK_NONE;

	m_KBProbability = 0;
	m_KBDistance = 0;

	m_KDProbability = 0;
	m_KDLevel = 0;
	m_KDBonus = 0;

	m_AbsDmg = 0;

	m_HealHP = 0;
	m_HealMP = 0;

	m_ConvHP = 0;
	m_ConvMP = 0;

	m_Aggro = 0;
	m_AggroBonus = 0;

	m_ChangeTarget = false;

    m_TeleportDistance = 0;

	for (int i=0; i < 3; ++i)
	{
		m_ReqWeapon[i] = REQ_WEAPON_NONE;
		m_ReqArmor[i] = REQ_ARMOR_NONE;
	}

	m_ReqShield = false;
	m_ReqDevil = false;
}

Skill::Skill (const SkillProperty& base)
    : m_ID(base.refID),
        m_GroupID(base.GroupID),
        m_Level(base.Level),
        m_NextSkill(base.NextSkill),
        m_SP(base.SP),
        m_Mastery(base.MastID),
        m_MasteryLvl(base.MastLv),
        m_GroupSkill1(base.Skills[0].first),
        m_GroupSkillLv1(base.Skills[0].second),
        m_GroupSkill2(base.Skills[1].first),
        m_GroupSkillLv2(base.Skills[1].second),
        m_MP(base.MP),
        m_HP(base.MinHPRatio),
        m_Init(base.InitTime),
        m_Delay(base.DelayTime),
        m_NextDelay(base.NextTime),
        m_Cooldown(base.Cooldown)
{
    m_Type = (SKILL_TYPE)base.Type;
    m_Weapon1 = (REQ_WEAPON_TYPE)base.Weapons[0];
    m_Weapon2 = (REQ_WEAPON_TYPE)base.Weapons[1];
    m_Race = (RACE)base.Race;

	m_Duration = 0;

	m_Attack.min_pwr = 0;
	m_Attack.max_pwr = 0;
	m_Attack.percent = 0;
	m_Attack.type = ATTACK_NONE;

	m_KBProbability = 0;
	m_KBDistance = 0;

	m_KDProbability = 0;
	m_KDLevel = 0;
	m_KDBonus = 0;

	m_AbsDmg = 0;

	m_HealHP = 0;
	m_HealMP = 0;

	m_ConvHP = 0;
	m_ConvMP = 0;

	m_Aggro = 0;
	m_AggroBonus = 0;

	m_PhyReflect = 0;
	m_MagReflect = 0;
	m_ChangeTarget = false;

    m_TeleportDistance = 0;

	for (int i=0; i < 3; ++i)
	{
		m_ReqWeapon[i] = REQ_WEAPON_NONE;
		m_ReqArmor[i] = REQ_ARMOR_NONE;
	}

	m_ReqShield = false;
	m_ReqDevil = false;
}

Skill::~Skill ()
{
}

bool Skill::is_usable (const REQ_WEAPON_TYPE weapon) const
{
	if (m_Weapon1 == weapon || m_Weapon2 == weapon)
		return true;

	if (m_Weapon1 == REQ_WEAPON_ANY && m_Weapon2 == REQ_WEAPON_ANY)
	{
		if (m_ReqWeapon[0] == REQ_WEAPON_NONE)
			return true;

		for (int i=0; i < 3; ++i)
		{
			if (m_ReqWeapon[i] == weapon)
				return true;
		}
	}

	return false;
}

bool Skill::require_shield () const
{
	return m_ReqShield;
}

bool Skill::require_devil () const
{
	return m_ReqDevil;
}

SKILL_ACTION_TYPE Skill::ActionType() const
{
    if ((m_Type == SKT_ATTACK && m_Attack.type != ATTACK_NONE) || (!m_status_list.empty() && m_Type != SKT_IMBUE))
        return SKILL_ACTION_ATTACK;

    if (!m_spawn_list.empty())
        return SKILL_ACTION_SUMMON;

    if (m_TeleportDistance)
        return SKILL_ACTION_TELEPORT;
    
    if (!m_buff_list.empty() || m_Type == SKT_IMBUE)
        return SKILL_ACTION_BUFF;

    return SKILL_ACTION_UNDEFINED;
}

void Skill::add_shield_requirement (const bool is_required)
{
	m_ReqShield = is_required;
}

void Skill::add_weapon_requirement (const REQ_WEAPON_TYPE type)
{
	for (int i=0; i < 3; ++i)
	{
		if (m_ReqWeapon[i] == REQ_WEAPON_NONE)
		{
			m_ReqWeapon[i] = type;
			break;
		}
	}
}

void Skill::add_armor_requirement (const REQ_ARMOR_TYPE type)
{
	for (int i=0; i < 3; ++i)
	{
		if (m_ReqArmor[i] == REQ_ARMOR_NONE)
		{
			m_ReqArmor[i] = type;
			break;
		}
	}
}

void Skill::add_devil_requirement (const bool is_required)
{
    m_ReqDevil = is_required;
}

void Skill::add_duration (const int duration)
{
	m_Duration = duration;
}

void Skill::add_knockdown (uint8_t level, uint8_t probability)
{
	m_KDProbability = probability;
	m_KDLevel = level;
}

void Skill::add_knockdown_bonus (uint8_t bonus)
{
	m_KDBonus = bonus;
}

void Skill::add_buff (const BuffEffect &buff)
{
	m_buff_list.push_back(buff);
}

void Skill::add_buff_end (const BuffEnd &buff)
{
	m_buff_end_list.push_back(buff);
}

void Skill::add_attack (const AttackInfo &atk)
{
	m_Attack = atk;
}

void Skill::add_area (const AttackArea &area)
{
	m_Area = area;
}

void Skill::add_absolute_damage (int damage)
{
	m_AbsDmg = damage;
}

void Skill::add_heal_amount (int HP, int MP)
{
	m_HealHP = HP;
	m_HealMP = MP;
}

void Skill::add_damage_conversion (int HP, int MP)
{
	m_ConvHP = HP;
	m_ConvMP = MP;
}

void Skill::add_aggro_reduce (int amount)
{
	m_Aggro -= amount;
}

void Skill::add_aggro (int amount, int bonus)
{
	m_Aggro = amount;
	m_AggroBonus = bonus;
}

void Skill::set_physical_reflect (int percent)
{
	m_PhyReflect = percent;
}

void Skill::set_magical_reflect (int percent)
{
	m_MagReflect = percent;
}

void Skill::set_change_target (bool change)
{
	m_ChangeTarget = change;
}
