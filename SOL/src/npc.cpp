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

#include "npc.hpp"
#include "entity_property.hpp"

#include <math.h>

NPC::NPC (const boost::shared_ptr<EntityProperty> &property)
	:   m_property(property),
        m_WalkSpeed(property->WalkSpeed),
        m_RunSpeed(property->RunSpeed),
        m_BerserkSpeed(property->BersekSpeed),
        m_StatusState(STATUS_ACTION_ALIVE),
        m_AttributeState(ATTRIBUTE_ACTION_NORMAL),
        m_AttackState(ATTACK_ACTION_STOP),
        m_MoveState(MOVEMENT_ACTION_RUN),
        m_StanceState(MOVEMENT_ACTION_STAND)
{
    m_PD = m_property->PhysicalDefense;
    m_MD = m_property->MagicalDefense;

    m_PR = m_property->ParryRatio;
    m_HR = m_property->HitRatio;

	m_CR = 0;
    m_BR = m_property->BlockRatio;
	m_PAbs = 0;
	m_MAbs = 0;
	m_AD = 0;
    m_MA = std::make_pair(0,0);
    m_PA = std::make_pair(0,0);

    m_MaxHP = 0;
    m_MaxMP = 0;

    m_AbsorbMP = 0;

    m_QueueSkill = 0;
    m_QueueTarget = 0;

    m_EncStatus = 0;

    m_WallSkillID = 0;
    m_WallHP = 0;
    m_WallAbs = 0;
    m_WallType = false;
}

NPC::~NPC ()
{
}

uint32_t NPC::get_id () const
{
    return m_property->refID;
}

const std::string& NPC::get_npc_name () const
{
    return m_property->refName;
}

ENTITY_TYPE NPC::type () const
{
    return m_property->type();
}

GENDER NPC::get_gender () const
{
    return static_cast<GENDER>(m_property->Gender);
}

void NPC::heal_hp (const uint32_t amount)
{
	if (amount)
    {
		m_HP += amount;

        if (m_HP > m_MaxHP)
            m_HP = m_MaxHP;
    }
}

void NPC::consume_hp (const uint32_t amount)
{
	if (amount)
	{
		if ( amount > m_HP)
			m_HP = amount;

		m_HP -= amount;

		if (!m_HP)
        {
            if (!signal_dead.empty())
                signal_dead(m_UniqueID,m_CurrentPos.get_zone());

            if (m_AttackState != ATTACK_ACTION_STOP)
            {
                m_AttackState = ATTACK_ACTION_STOP;

                if (!signal_state.empty())
                    signal_state(STATE_ATTACK,m_AttackState);
            }

            m_StatusState = STATUS_ACTION_DEAD;

            if (!signal_state.empty())
                signal_state(STATE_STATUS,m_StatusState);

            m_QueueSkill = 0;
            m_QueueTarget = 0;
            m_CurrentTask = 0;

            m_NextPos = Coord();
		}
	}
}

void NPC::heal_mp (const uint32_t amount)
{
	if (amount)
    {
        m_MP += amount;

        if (m_MP > m_MaxMP)
            m_MP = m_MaxMP;
    }
}

void NPC::consume_mp (const uint32_t amount)
{
	if (amount)
	{
		if ( amount > m_MP)
            m_MP = 0;
        else
            m_MP -= amount;
	}
}

void NPC::inc_aggro (const uint32_t aggro, const uint32_t attacker_id)
{
	m_aggro_list[attacker_id] += aggro;
}

void NPC::receive_damage (const uint32_t damage, const uint32_t attacker_id)
{
	if (damage > m_HP)
		m_damage_list[attacker_id] += m_HP;
	else
		m_damage_list[attacker_id] += damage;

	consume_hp(damage);
}

std::map<uint32_t,uint32_t> NPC::experience_gained () const
{
	std::map<uint32_t,uint32_t> exp_list = m_damage_list;

	float ratio = 0;
    for ( std::map<uint32_t,uint32_t>::iterator i = exp_list.begin(); i != exp_list.end(); ++i)
	{
		ratio = i->second/max_hp();

        i->second = m_property->XP*ratio;
	}

	return exp_list;
}

uint32_t NPC::hp () const
{
	return m_HP;
}

uint32_t NPC::mp () const
{
	return m_MP;
}

uint32_t NPC::max_hp () const
{
	return m_MaxHP;
}

uint32_t NPC::max_mp () const
{
	return m_MaxMP;
}

uint16_t NPC::physical_defense () const
{
	return m_PD;
}

uint16_t NPC::magical_defense () const
{
	return m_MD;
}

float NPC::physical_absorb () const
{
	return m_PAbs;
}

float NPC::magical_absorb () const
{
	return m_MAbs;
}

uint16_t NPC::hit_ratio () const
{
	return m_HR;
}

uint16_t NPC::parry_ratio () const
{
	return m_PR;
}

float NPC::attack_distance () const
{
	return m_AD;
}

NPC::attack_type NPC::physical_attack () const
{
	return m_PA;
}

NPC::attack_type NPC::magical_attack () const
{
	return m_MA;
}

uint16_t NPC::critical_ratio () const
{
	return m_CR;
}

uint16_t NPC::block_ratio () const
{
	return m_BR;
}

float NPC::walking_speed () const
{
	return m_WalkSpeed;
}

float NPC::running_speed () const
{
	return m_RunSpeed;
}

float NPC::berseker_speed () const
{
	return m_BerserkSpeed;
}

float NPC::current_speed () const
{
    return (get_movement_state() == MOVEMENT_ACTION_RUN) ? running_speed() : walking_speed();
}

uint8_t NPC::DamageMP () const
{
    return m_AbsorbMP;
}

bool NPC::select_object (uint32_t id)
{
	m_sel_object = id;
	return true;
}

uint32_t NPC::get_selected_object () const
{
	return m_sel_object;
}

void NPC::insert_imbue (const Imbue &imbue)
{
	m_Imbue = imbue;

	calculate_stats();
}

const Imbue& NPC::GetImbue () const
{
    return m_Imbue;
}

std::pair<uint32_t,uint32_t> NPC::remove_imbue (const uint32_t skillID, bool signal)
{
    std::pair<uint32_t,uint32_t> ret = std::make_pair(0,0);

    if (m_Imbue.skillID == skillID)
    {
        ret = std::make_pair(m_Imbue.taskID,m_Imbue.castID);

        m_Imbue = Imbue();

        if (signal)
            calculate_stats();
    }

    return ret;
}

bool NPC::using_imbue () const
{
    return m_Imbue.skillID != 0;
}

void NPC::insert_status (const Status &status)
{
    m_status_list.insert(std::make_pair(status.ID,status));

    m_EncStatus |= status.ID;
}

uint32_t NPC::GetEncodedStatus () const
{
    return m_EncStatus;
}

const std::map<uint32_t,Status>& NPC::GetStatus () const
{
    return m_status_list;
}

bool NPC::hasStatus (uint32_t mask) const
{
    return m_EncStatus & mask;
}

std::pair<uint32_t,uint32_t> NPC::RemoveStatus (uint32_t statusID)
{
    std::pair<uint32_t,uint32_t> ret = std::make_pair(0,0);
    std::map<uint32_t,Status>::iterator it = m_status_list.find(statusID);

    if (it != m_status_list.end())
    {
        ret = std::make_pair(it->second.taskID,it->second.payloadTaskID);
        m_status_list.erase(it);
        m_EncStatus ^= statusID;
    }

    return ret;
}

void NPC::ClearStatus ()
{
    m_EncStatus = 0;
    m_status_list.clear();
}

void NPC::insert_buff (const Buff &buff)
{
    if (!buff.Effects.empty())
    {
        m_buff_list.insert(std::make_pair(buff.SkillID,buff));

        calculate_stats();
    }
}

std::pair<uint32_t,uint32_t> NPC::remove_buff (const uint32_t SkillID, bool signal)
{
    std::pair<uint32_t,uint32_t> ret = std::make_pair(0,0);
    std::map<uint32_t,Buff>::iterator i = m_buff_list.find(SkillID);

    if (i != m_buff_list.end())
    {
        ret = std::make_pair(i->second.taskID,i->second.CastID);

        m_buff_list.erase(i);

        if (signal)
            calculate_stats();
    }

    return ret;
}

std::vector<std::pair<uint32_t,uint32_t> > NPC::RemoveLesserBuffs (const Buff &buff)
{
    std::vector<std::pair<uint32_t,uint32_t> > ret;

    /// REMOVE ALL BUFF THAT THIS BUFF CANCEL. !!ONLY FROM THE SAME GROUP!!
    for (std::map<uint32_t,Buff>::iterator i = m_buff_list.begin(); i != m_buff_list.end();)
    {
        if (!buff.IsCastable(i->second,false))
        {
            ret.push_back(std::make_pair(i->second.taskID,i->second.CastID));
            m_buff_list.erase(i++);
        }
        else
            ++i;
    }

    return ret;
}

bool NPC::is_buff_stackable (const Buff &buff) const
{
    for (std::map<uint32_t,Buff>::const_iterator i = m_buff_list.begin(); i != m_buff_list.end(); ++i)
    {
        if (!i->second.IsCastable(buff,true) || !i->second.IsEffectStackable(buff))
            return false;
    }

    return true;
}

const std::map<uint32_t,Buff>& NPC::get_buff_list () const
{
    return m_buff_list;
}

uint8_t NPC::WallPhysicalAbsorb () const
{
    return m_WallType ? m_WallAbs : 0;
}

uint8_t NPC::WallMagicalAbsorb () const
{
    return m_WallType ? 0 : m_WallAbs;
}

uint32_t NPC::WallSkillID () const
{
    return m_WallSkillID;
}

bool NPC::hasWall () const
{
    return m_WallHP;
}

void NPC::ConsumeWallHP (uint32_t amount)
{
    if (amount > m_WallHP)
        m_WallHP = 0;
    else
        m_WallHP -= amount;
}

void NPC::SetQueueSkill (const uint32_t skillID, const uint32_t targetID)
{
    m_QueueSkill = skillID;
    m_QueueTarget = targetID;
}

uint32_t NPC::GetQueueSkill () const
{
    return m_QueueSkill;
}

uint32_t NPC::GetQueueTarget () const
{
    return m_QueueTarget;
}

bool NPC::IsSkillAvailable (const uint32_t skillID) const
{
    return m_cooldown_list.find(skillID) == m_cooldown_list.end();
}

void NPC::AddSkillCooldown (const uint32_t taskID, const uint32_t skillID)
{
    m_cooldown_list.insert(std::make_pair(skillID,taskID));
}

void NPC::RemoveSkillCooldown (const uint32_t skillID)
{
    m_cooldown_list.erase(skillID);
}

void NPC::SetCurrentTask (const uint8_t taskID)
{
    m_CurrentTask = taskID;
}

uint8_t NPC::GetCurrentTask () const
{
    return m_CurrentTask;
}

bool NPC::IsTaskInterruptable () const
{
    return true;
}

const Coord& NPC::get_destination () const
{
	return m_NextPos;
}

void NPC::set_destination (const Coord &pos)
{
    if (pos.get_zone())
    {
        float dx = pos.global_x() - m_CurrentPos.global_x();
        float dy = pos.global_y() - m_CurrentPos.global_y();

        m_Angle = atan2(dy,dx)*10430.38208;
    }

    m_NextPos = pos;
}

bool NPC::is_moving () const
{
	return m_NextPos.get_zone();
}

uint32_t NPC::eta_to_destination () const
{
    float speed = 0;

    if (m_MoveState == MOVEMENT_ACTION_RUN)
        speed = m_RunSpeed;
    else
        speed = m_WalkSpeed;

    float dx = m_NextPos.global_x() - m_CurrentPos.global_x();
    float dy = m_NextPos.global_y() - m_CurrentPos.global_y();

    return sqrt(dx*dx+dy*dy)*10000/speed;
}

void NPC::set_speed (float wlk, float run, float bsk, bool signal)
{
	m_WalkSpeed = wlk;
	m_RunSpeed = run;
	m_BerserkSpeed = bsk;

    if (signal && !signal_speed.empty())
        signal_speed();
}

STATE_STATUS_ACTION NPC::get_status_state () const
{
    return m_StatusState;
}

STATE_MOVEMENT_ACTION NPC::get_stance_state () const
{
    return m_StanceState;
}

void NPC::change_stance ()
{
    if (m_StanceState == MOVEMENT_ACTION_SIT)
        m_StanceState = MOVEMENT_ACTION_STAND;
    else
        m_StanceState = MOVEMENT_ACTION_SIT;

    if (!signal_state.empty())
        signal_state(STATE_MOVEMENT,m_StanceState);
}

STATE_MOVEMENT_ACTION NPC::get_movement_state () const
{
    return m_MoveState;
}

void NPC::set_movement_state (STATE_MOVEMENT_ACTION action)
{
    m_MoveState = action;

    if (!signal_state.empty())
        signal_state(STATE_MOVEMENT,m_MoveState);

    if (!signal_speed.empty())
        signal_speed();
}

STATE_ATTRIBUTE_ACTION NPC::get_attribute_state () const
{
    return m_AttributeState;
}

STATE_ATTACK_ACTION NPC::get_attack_state () const
{
    return m_AttackState;
}

void NPC::set_attack_state (STATE_ATTACK_ACTION state)
{
    m_AttackState = state;
}

bool NPC::IsDead () const
{
    return m_StatusState == STATUS_ACTION_DEAD;
}

void NPC::Transform (const boost::shared_ptr<EntityProperty> &property)
{
    m_property = property;
}

void NPC::calculate_status_effects ()
{
    uint8_t ratio = 0;
    float wlkspeed = walking_speed();
    float runspeed = running_speed();
    float bskspeed = berseker_speed();

    std::map<uint32_t,Status>::const_iterator it;

    for (it = m_status_list.begin(); it != m_status_list.end(); ++it)
    {
        switch(it->first)
        {
        case STATUS_SHOCK:
            if (m_PR < it->second.Payload)
                m_PR = 0;
            else
                m_PR -= it->second.Payload;
            break;
        case STATUS_FROST:
        case STATUS_DULL:
            wlkspeed *=0.5;
            runspeed *=0.5;
            bskspeed *=0.5;
            break;
        case STATUS_WEAKEN:
            if (m_MD < it->second.Payload)
                m_MD = 0;
            else
                m_MD -= it->second.Payload;
            break;
        case STATUS_DECAY:
            if (m_PD < it->second.Payload)
                m_PD = 0;
            else
                m_PD -= it->second.Payload;
            break;
        case STATUS_DARKNESS:
            if (m_HR < it->second.Payload)
                m_HR = 0;
            else
                m_HR -= it->second.Payload;
            break;
        case STATUS_IMPOTENT:
            ratio = (1-it->second.Payload/100);
            m_PA.first *= ratio;
            m_PA.second *= ratio;
            m_MA.first *= ratio;
            m_MA.second *= ratio;
            break;
        default:
            break;
        }
    }

    set_speed(wlkspeed,runspeed,bskspeed,false);
}
