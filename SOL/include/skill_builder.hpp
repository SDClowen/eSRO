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

#ifndef __SLES_SKILL_BUILDER_HPP__
#define __SLES_SKILL_BUILDER_HPP__

#include <map>
#include <boost/shared_ptr.hpp>
#include <skill.hpp>

class SkillBuilder
{
public:

	typedef void (SkillBuilder::*builderfunc)();

	enum { MAX_INDEX = 37 };

	SkillBuilder ();

	void build (const SkillProperty &property);

	boost::shared_ptr<Skill> get_built_object () const;

protected:

	void OnEnd ();

	/**
	 *
	 *	PK2_ENTRY_ID 25196
	 *
	 *	@build  Bleed curse information.
	 *
	 **/

	void OnCurseBleed ();

	/**
	 *
	 *	PK2_ENTRY_ID 25202
	 *
	 *	@build block passive skill. Increase the block rate of the player
	 *	when the passive is active.
	 *
	 **/

	void OnBlockInc ();

	/**
	 *
     *	PK2_ENTRY_ID 26210 (Frostbite)
     *	PK2_ENTRY_ID 26234 (Freeze)
     *	PK2_ENTRY_ID 31330 (Zombie)
     *	PK2_ENTRY_ID 25971 (Shock)
	 *	PK2_ENTRY_ID 25441 (Confuse)
	 *	PK2_ENTRY_ID 26213 (Fear)
	 *	PK2_ENTRY_ID 29556 (Stun)
	 *	PK2_ENTRY_ID 29300 (Bind)
	 *	PK2_ENTRY_ID 29541 (Sleep)
     *	PK2_ENTRY_ID 25210 (Darkness)
     *	PK2_ENTRY_ID 25715 (Disease)
     *  PK2_ENTRY_ID 1668509040 (Combustion)
     *	PK2_ENTRY_ID 84545280 (Hidden)
	 *
	 *	@build Curse status information.
	 *
	 **/

	void OnCurse ();

	void OnBuff (int argc);

	void OnBuff_1Arg ();

	void OnBuff_2Arg ();

	void OnBuff_3Arg ();

	void OnBuff_4Arg ();

	void OnTimedBuff ();

	void OnBuffEnd ();

	/**
	 *
	 *	PK2_ENTRY_ID 25697
	 *
	 *	@build  Damage increase bonus information when the
	 *	target is knockdown.
	 *
	 **/

	void OnKnockdownBonus ();

	/**
	 *
	 *	PK2_ENTRY_ID 27490
	 *
	 *	@build Knockback information.
	 *
	 **/

	void OnKnockback ();

	/**
	 *
	 *	PK2_ENTRY_ID 27503
	 *
	 *	@build Knockdown information.
	 *
	 **/

	void OnKnockdown ();

	/**
	 *
	 *	PK2_ENTRY_ID 28003
	 *
	 *	@build Unk1 information.
	 *
	 **/

	void OnUnk2 ();

	/**
	 *
	 *	PK2_ENTRY_ID 1685418593
	 *
	 *	@build Skill and/or buff duration information.
	 *
	 **/

	void OnSkillDuration ();

	void OnPetGrowth ();

	/**
	 *
	 *	PK2_ENTRY_ID 1751474540
	 *
	 *	@build HP and MP healing amount information.
	 *
	 **/

	void OnHealAmount ();

	/**
	 *
	 *	PK2_ENTRY_ID 1751737445
	 *
	 *	@build Invisible status information.
	 *
	 **/

	void OnInvisible ();

	/**
	 *
	 *	CHANGE THE SELECTED PLAYERS TARGET TO THE SKILL CASTER.
	 *
	 **/

	void OnChangeTarget ();

	void OnCrownBuff ();

	void OnHPDrain ();

	void OnAbsorbAggro ();

	void OnDamageDivide ();

	void OnDamage2MP ();

	void OnAbsorbDamage ();

	void OnRangedBuff ();

	void OnTransformMask ();

	void OnUnk14 ();

	/**
	 *
	 *	PK2_ENTRY_ID 6386804
	 *
	 *	@build Attack damage information.
	 *
	 **/

	void OnAttack ();

	/**
	 *
	 *	PK2_ENTRY_ID 6645362
	 *
	 *	@build Attack area information.
	 *
	 **/

	void OnAttackArea ();

	/**
	 *
	 *	PK2_ENTRY_ID 7564131
	 *
	 *	@build Unknown information.
	 *
	 **/

	void OnUnk5 ();

	void OnCurseResist ();

	/**
	 *
	 *	PK2_ENTRY_ID 25145280
	 *
	 *	@build Cure curse information. (not sure)
	 *
	 **/

	 void OnCureCurse ();

	void OnEffectiveDistance ();

	/**
	 *
	 *	PK2_ENTRY_ID 1635017569
	 *
	 *	@build Unknown information.
	 *	(Related to skill damage bonus percent like dull,stun,etc, for ex. 1h double stab)
	 *
	 **/

	void OnUnk8 ();

	void OnEffectResist ();

	/**
	 *
	 *	PK2_ENTRY_ID 1635017569
	 *
	 *	@build Unknown information.
	 *	(Related to rogue power shot b (lv50+) maybe knockback or something )
	 *
	 **/

	void OnUnk9 ();

	/**
	 *
	 *	PK2_ENTRY_ID 1668510578
	 *
	 *	@build Cure status information.
	 *
	 **/

	void OnCureStatus ();

	void OnMPRequiredDec ();

	void OnUnk3 ();

	void OnUnk12 ();

	/**
	 *
	 *	PK2_ENTRY_ID 1685352052
	 *
	 *	@build Aggro reduce information.
	 *
	 **/

	void OnAggroReduce ();

	void OnUnk1 ();

	void OnUnk4 ();

	/**
	 *
	 *	PK2_ENTRY_ID 1836541044
	 *
	 *	@build Warlock rod magical damage reflect information.
	 *
	 **/

	void OnMagicReflect ();

	void OnMPConsume ();

	void OnUnk10 ();

	void OnUnk17 ();

	void OnDamageAbsolute ();

	void OnAttackPrevention ();

	//Perform the action in a period of time

	void OnSkillActivate ();

	void OnPhysicReflect ();

	void OnCaptureNPC ();

	/**
	 *
	 *	PK2_ENTRY_ID 1919120754
	 *
	 *	@build The amount of status cured information.
	 *
	 **/

	void OnCureStatusAmount ();

	void OnUnk6 ();

	/**
	 *
	 *	PK2_ENTRY_ID 1919246708
	 *
	 *	@build Unknown information. (Probly Effect decrease ).
	 *
	 **/

	void OnUnk7 ();

	/**
	 *
	 *	PK2_ENTRY_ID 1919250787
	 *
	 *	@build Knockdown target required information.
	 *
	 **/

	void OnRequiredKnockdown ();

	/**
	 *
	 *	PK2_ENTRY_ID 1919250793
	 *
	 *	@build Required weapon information.
	 *
	 **/

	void OnRequiredWeapon ();

	/**
	 *
	 *	PK2_ENTRY_ID 1919251317
	 *
	 *	@build Resurrect information.
	 *
	 **/

	void OnResurrect ();

	void OnBaseSkill ();

	void OnMaxTambourCasted ();

	void OnSkillBonus ();

	void OnNPCSpawn ();

	void OnBowCall ();

	/**
	 *
	 *	PK2_ENTRY_ID 1952542324
	 *
	 *	@build Effect unknown information. (maybe related to the pill use)
	 *
	 **/

	void OnEffectUnk1 ();

	void OnTeleport ();

	void OnUnk15 ();

	/**
	 *
	 *	PK2_ENTRY_ID 1953395762
	 *
	 *	@build Aggro increase information.
	 *
	 **/

	void OnAggroInc ();

private:

	int m_Index;
	uint32_t m_NextBuilder;

	SkillProperty m_Property;
	boost::shared_ptr<Skill> m_SkillObj;

	std::map<uint32_t,builderfunc> m_builder_table;
};

#endif //__SLES_SKILL_BUILDER_HPP__
