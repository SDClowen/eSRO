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

#ifndef __SLES_SKILL_HPP__
#define __SLES_SKILL_HPP__

#include "types.hpp"
#include "skill_property.hpp"
#include "skill_requirement.hpp"
#include "skill_status.hpp"
#include "skill_buff.hpp"
#include "skill_attack.hpp"

#include <vector>
#include <boost/array.hpp>

enum ESSENTIAL_ITEM_TYPE
{
	ESS_SHIELD = 4,
	ESS_WEAPON = 6,
	ESS_ARMOR = 10,
	ESS_DEVIL = 14
};

enum SKILL_TYPE
{
    SKT_PASSIVE,
	SKT_IMBUE,
	SKT_ATTACK,
	SKT_NONE
};

enum SKILL_ACTION_TYPE
{
    SKILL_ACTION_ATTACK,
    SKILL_ACTION_BUFF,
    SKILL_ACTION_SUMMON,
    SKILL_ACTION_TELEPORT,
    SKILL_ACTION_HEAL,
    SKILL_ACTION_RESURRECT,
    SKILL_ACTION_UNDEFINED
};

enum SKILL_BASE_ATTACKS
{
    SKILL_PUNCH_01 = 1,
    SKILL_CH_SWORD_BASE_01 = 2,
    SKILL_CH_SPEAR_BASE_01 = 40,
    SKILL_CH_BOW_BASE_01 = 70,
    SKILL_EU_SWORD_BASE_01 = 7127,
    SKILL_EU_TSWORD_BASE_01 = 7128,
    SKILL_EU_AXE_BASE_01 = 7129,
    SKILL_EU_CROSSBOW_BASE_01 = 7909,
    SKILL_EU_DAGGER_BASE_01 = 7910,
    SKILL_EU_STAFF_BASE_01 = 8454,
    SKILL_EU_WAND_WARLOCK_BASE_01 = 9069,
    SKILL_EU_HARP_BASE_01 = 9606,
    SKILL_EU_WAND_CLERIC_BASE_01 = 9970
};

struct NPC_SPAWN_INFO
{
    uint32_t refCharID;
    uint8_t GiantCount;
    uint8_t ChampionCount;
    uint8_t NormalCount;
};

class SkillBuilder;

class Skill
{
public:

    friend class SkillBuilder;

	Skill ();

	Skill (const SkillProperty& property);

	virtual ~Skill ();

	inline uint32_t id () const { return m_ID; }

	inline int group_id () const { return m_GroupID; }

	inline int next_skill () const { return m_NextSkill; }

	inline SKILL_TYPE type () const { return m_Type; }

	inline uint8_t level () const { return m_Level; }

	inline uint32_t required_sp () const { return m_SP; }

	inline RACE required_race () const { return m_Race; }

	inline int required_mastery () const { return m_Mastery; }

	inline int required_mastery_lv () const { return m_MasteryLvl; }

	int required_group_skill1 () const { return m_GroupSkill1; }

	uint8_t required_group_skill1_lv () const { return m_GroupSkillLv1; }

	int required_group_skill2 () const { return m_GroupSkill2; }

	uint8_t required_group_skill2_lv () const { return m_GroupSkillLv2; }

	inline uint32_t required_mp () const { return m_MP; }

	inline uint32_t required_hp_percent () const { return m_HP; }

	inline int duration () const { return m_Duration; }

	inline int init () const { return m_Init; }

	inline int delay () const { return m_Delay; }

	inline int next_delay () const { return m_NextDelay; }

	inline int cooldown () const { return m_Cooldown; }

	bool is_usable (const REQ_WEAPON_TYPE type) const;

	bool is_usable (const REQ_ARMOR_TYPE type) const;

	bool require_shield () const;

	bool require_devil () const;

    SKILL_ACTION_TYPE ActionType () const;

	inline const AttackInfo& attack () const { return m_Attack; }

	inline const AttackArea& area () const { return m_Area; }

	inline int absolute_damage () const { return m_AbsDmg; }

    inline uint8_t TeleportDistance () const { return m_TeleportDistance; }

	inline const std::vector<BuffEffect>& buff_list () const { return m_buff_list; }

	inline const boost::array<REQ_WEAPON_TYPE,3>& required_weapons () const { return m_ReqWeapon; }

public:

	void add_shield_requirement (const bool is_required);

	void add_weapon_requirement (const REQ_WEAPON_TYPE type);

	void add_armor_requirement (const REQ_ARMOR_TYPE type);

	void add_devil_requirement (const bool is_required);

	void add_duration (const int duration);

	void add_knockdown (uint8_t level, uint8_t probability);

	void add_knockdown_bonus (uint8_t bonus);

	void add_buff (const BuffEffect &buff);

	void add_buff_end (const BuffEnd &buff);

	void add_attack (const AttackInfo &atk);

	void add_area (const AttackArea &area);

	void add_absolute_damage (int damage);

	void add_heal_amount (int HP, int MP);

	void add_damage_conversion (int HP, int MP);

	void add_aggro_reduce (int amount);

	void add_aggro (int amount, int bonus);

	void set_physical_reflect (int percent);

	void set_magical_reflect (int percent);

	void set_change_target (bool change);

protected:

	uint32_t m_ID;					//Skill ID.
    uint16_t m_GroupID;				//Skill Group ID.
	SKILL_TYPE m_Type;			//Type of skill.
	uint8_t m_Level;				//Skill Level.
	uint32_t m_NextSkill;			//Next skill to cast.
	uint32_t m_SP;					//Required Skill Points.
	RACE m_Race;			//Required Race.
	int m_Mastery;				//Required Mastery.
	int m_MasteryLvl;			//Minimum Mastery Level.
	int m_GroupSkill1;		//Required Primary Group Skill.
	uint8_t m_GroupSkillLv1;		//Minimum Primary Group Skill level.
	int m_GroupSkill2;		//Required Secundary Group Skill.
	uint8_t m_GroupSkillLv2;		//Minimum Secundary Group Skill level.
	REQ_WEAPON_TYPE m_Weapon1;  //Required Primary Weapon.
	REQ_WEAPON_TYPE m_Weapon2;  //Required Secundary Weapon.
	uint32_t m_MP;					//Required Mana Points to cast skill.
	uint32_t m_HP;					//Required Hit Points Percent to cast skill.

	int m_Duration;				//Skill effect duration.
	int m_Init;					//Skill initiation time.
	int m_Delay;				//Skill delay time.
	int m_NextDelay;			//Skill next cast time.

public:

	int m_Cooldown;

	AttackInfo m_Attack;
	AttackArea m_Area;

    std::vector<Status> m_status_list;

	uint8_t m_KBProbability;
    uint16_t m_KBDistance;

	uint8_t m_KDProbability;
	uint8_t m_KDLevel;
	uint8_t m_KDBonus;

    uint16_t m_AbsDmg;

	uint32_t m_HealHP;
	uint32_t m_HealMP;

    uint32_t m_MPConsume;
    uint32_t m_MPConsumeTime;

	int m_ConvHP;
	int m_ConvMP;

	int m_Aggro;
	int m_AggroBonus;	//Aggro bonus (%)

	int m_MagReflect;
	int m_PhyReflect;
	bool m_ChangeTarget;

    uint8_t m_TeleportDistance;

	bool m_ReqShield;
	bool m_ReqDevil;	//Required devil suit
	boost::array<REQ_WEAPON_TYPE,3> m_ReqWeapon;
	boost::array<REQ_ARMOR_TYPE,3> m_ReqArmor;

	std::vector<BuffEffect> m_buff_list;
	std::vector<BuffEnd> m_buff_end_list;

    std::vector<NPC_SPAWN_INFO> m_spawn_list;
};

#endif //__SLES_SKILL_HPP__
