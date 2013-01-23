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

#ifndef __SLES_NPCS_HPP__
#define __SLES_NPCS_HPP__

#include "spawnobject.hpp"
#include "entity_state.hpp"
#include "entity_constants.hpp"
#include "skill_status.hpp"
#include "skill_buff.hpp"
#include "skill_imbue.hpp"

#include <set>
#include <utility>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

class EntityProperty;

class NPC : public SpawnObject
{
public:

    boost::signals2::signal<void (uint8_t,uint8_t)> signal_state;

    boost::signals2::signal<void ()> signal_speed;

    boost::signals2::signal<void (uint32_t,int16_t)> signal_dead;

public:

	typedef std::pair<uint32_t,uint32_t> attack_type;

	typedef std::map<SKILL_BUFF,Buff>::iterator buff_iterator;

    NPC(const boost::shared_ptr<EntityProperty> &property);

	virtual ~NPC();

	uint32_t get_id () const;

	const std::string& get_npc_name () const;

	ENTITY_TYPE type () const;

	virtual uint8_t level () const = 0;

	GENDER get_gender () const;

	void heal_hp (const uint32_t amount);

	void heal_mp (const uint32_t amount);

	void consume_hp (const uint32_t amount);

	void consume_mp (const uint32_t amount);

	void inc_aggro (const uint32_t aggro, const uint32_t attacker_id);

	void receive_damage (const uint32_t damage, const uint32_t attacker_id);

	std::map<uint32_t,uint32_t> experience_gained () const;

    /**
     *
     *  @brief: Current Hit Points.
     *
     **/

	uint32_t hp () const;

	/**
	 *
	 *  @brief: Current Mana Points.
	 *
	 **/

	uint32_t mp () const;

	uint32_t max_hp () const;

	uint32_t max_mp () const;

	uint16_t physical_defense () const;

	uint16_t magical_defense () const;

    float physical_absorb () const;

    float magical_absorb () const;

	uint16_t hit_ratio () const;

	uint16_t parry_ratio () const;

	float attack_distance () const;

	attack_type physical_attack () const;

	attack_type magical_attack () const;

	uint16_t critical_ratio () const;

	uint16_t block_ratio () const;

	float walking_speed () const;

	float running_speed () const;

	float berseker_speed () const;

    float current_speed () const;

    /**
     *
     *  @brief MP Absorb percent.
     *  @return Convertion percent from damage to MP. (0 - 100)
     *
     **/

    uint8_t DamageMP () const;

	virtual void calculate_stats () = 0;

	/**
	 *
	 *  @brief Set current target. The object must be in the observer list.
	 *  @id Object ID to set as target.
	 *
	 **/

	bool select_object (uint32_t objid);

	uint32_t get_selected_object () const;

	void insert_imbue (const Imbue &imbue);

    const Imbue& GetImbue () const;

    /**
     *
     *  @brief Remove a imbue from entity list.
     *  @SkillID: Originated skill.
     *  @signal: Flag to update and signal stats.
     *  @return Pair associated taskID, associated castID.
     *
     **/

    std::pair<uint32_t,uint32_t> remove_imbue (const uint32_t skillID, bool signal = true);

	bool using_imbue () const;

	void insert_status (const Status &status);

    /**
     *
     *  @brief Return all the status packed into a 32bit integer.
     *
     **/

    uint32_t GetEncodedStatus () const;

    const std::map<uint32_t,Status>& GetStatus () const;

    /**
     *
     *  @brief Check if the selected status are activated.
     *
     **/

    bool hasStatus (uint32_t mask) const;

    std::pair<uint32_t,uint32_t> RemoveStatus (uint32_t statusID);

    /**
     *
     *  @brief Remove all the status of the player.
     *
     **/

    void ClearStatus ();

    void insert_buff (const Buff &buff);

    /**
     *
     *  @brief Remove a buff from entity list.
     *  @SkillID: Originated skill.
     *  @signal: Flag to update and signal stats.
     *  @return Pair associated taskID, associated castID.
     *
     **/

    std::pair<uint32_t,uint32_t> remove_buff (const uint32_t SkillID, bool signal = true);

    std::vector<std::pair<uint32_t,uint32_t> > RemoveLesserBuffs (const Buff &buff);

    bool is_buff_stackable (const Buff &buff) const;

    const std::map<uint32_t,Buff>& get_buff_list () const;

    uint8_t WallPhysicalAbsorb () const;

    uint8_t WallMagicalAbsorb () const;

    uint32_t WallSkillID () const;

    bool hasWall () const;

    void ConsumeWallHP (uint32_t amount);

    /**
     *
     *  @brief Add a skill into the skill queue for later casting.
     *
     **/

    void SetQueueSkill (const uint32_t skillID, const uint32_t targetID);

    /**
     *
     *  @brief Get queue skill for casting.
     *
     **/

    uint32_t GetQueueSkill () const;

    uint32_t GetQueueTarget () const;

    /**
     *
     *  @brief Indicates if a skill is available for use by the player or not.
     *  @param skillID ID to lookup.
     *  @return active status.
     *
     **/

    bool IsSkillAvailable (const uint32_t skillID) const;

    /**
     *
     *  @brief Insert a skill into cooldown list.
     *  @param taskID Associated task ID in the world queue.
     *  @param skillID skill to add.
     *
     **/

    void AddSkillCooldown (const uint32_t taskID, const uint32_t skillID);

    /**
     *
     *  @brief Remove a skill from the cooldown list.
     *  @param skillID skill to remove.
     *
     **/

    void RemoveSkillCooldown (const uint32_t skillID);

    void SetCurrentTask (const uint8_t taskID);

    uint8_t GetCurrentTask () const;

    virtual bool IsTaskInterruptable () const;

public:

    /// FUNCTIONS FROM MOVEABLE OBJECT
    const Coord& get_destination () const;

    /**
	 *
	 *	@brief Set object destination position.
	 *
	 **/

    void set_destination (const Coord &pos);

	bool is_moving () const;

	uint32_t eta_to_destination () const;

    void set_speed (float wlk, float run, float bsrk, bool signal = true);

    /**
     *
     *  @brief: Indicates object current state.
     *  @return: if the object is alive or dead.
     *
     **/

	STATE_STATUS_ACTION get_status_state () const;

	/**
	 *
	 *  @brief: Indicate the object current stance state.
	 *  @return: if the object is standing or sitting.
	 *
	 **/

    STATE_MOVEMENT_ACTION get_stance_state () const;

    void change_stance ();

    /**
     *
     *  @brief: Indicate the object current move state.
     *  @return: if the object is running or walking.
     *
     **/

    STATE_MOVEMENT_ACTION get_movement_state () const;

    void set_movement_state (const STATE_MOVEMENT_ACTION action);

    /**
     *
     *  @brief: Indicate the object current attribute state.
     *  @return: if the object is berserking, invunerable or normal.
     *
     **/

    STATE_ATTRIBUTE_ACTION get_attribute_state () const;

    /**
     *
     *  @brief: Indicate the object current attack state.
     *  @return: if the object is attacking or not.
     *
     **/

    STATE_ATTACK_ACTION get_attack_state () const;

    void set_attack_state (STATE_ATTACK_ACTION state);

    /**
     *
     *  @brief: check the object death state.
     *  @return: true if is dead else returns false.
     *
     **/

    bool IsDead () const;

    void Transform (const boost::shared_ptr<EntityProperty> &property);

protected:

    void calculate_status_effects ();

protected:

    boost::shared_ptr<EntityProperty> m_property;

	uint32_t m_sel_object;
	std::pair<uint32_t,uint32_t> m_BasePhysicalAttack;
	std::pair<uint32_t,uint32_t> m_BaseMagicalAttack;

	float m_AD;				//ATTACK DISTANCE
	attack_type m_PA;		//PHYSICAL ATTACK
	attack_type m_MA;		//MAGICAL ATTACK
	uint32_t m_HP;				//HIT POINTS
	uint32_t m_MaxHP;			//MAX HIT POINTS
	uint32_t m_MP;				//MANA POINTS
	uint32_t m_MaxMP;			//MAX MANA POINTS
	uint16_t m_PD;				//PHYSICAL DEFENSE
	uint16_t m_MD;				//MAGICAL DEFENSE
	uint16_t m_HR;				//HIT RATIO
	uint16_t m_PR;				//PARRY RATIO
	uint16_t m_CR;				//CRITICAL RATIO
	uint16_t m_BR;				//BLOCK RATIO
    float m_PAbs;               //PHYSICAL ABSORB (VALUES FROM 0 TO 100)
    float m_MAbs;               //MAGICAL ABSORB (VALUES FROM 0 TO 100)
	float m_WalkSpeed;
	float m_RunSpeed;
	float m_BerserkSpeed;

    uint8_t m_AbsorbMP;        //CONVERTION RATE FROM DAMAGE 2 MP

    bool m_WallType;     /// TRUE PHYSICAL ELSE MAGICAL
    uint16_t m_WallHP;
    uint8_t m_WallAbs;
    uint32_t m_WallSkillID;

    uint32_t m_AttackTaskID;            /// taskID where attack state timeout is being tracked

	STATE_STATUS_ACTION m_StatusState;
    STATE_ATTRIBUTE_ACTION m_AttributeState;
    STATE_ATTACK_ACTION m_AttackState;
    STATE_MOVEMENT_ACTION m_MoveState;      ///WALK OR RUN
    STATE_MOVEMENT_ACTION m_StanceState;    ///STAND OR SIT

	std::map<uint32_t,uint32_t> m_aggro_list;
	std::map<uint32_t,uint32_t> m_damage_list;

	uint8_t m_Level;

	Imbue m_Imbue;

    uint32_t m_EncStatus;
    std::map<uint32_t,Status> m_status_list;
    std::map<uint32_t,Buff> m_buff_list;

    uint32_t m_QueueSkill;
    uint32_t m_QueueTarget;
    std::map<uint32_t,uint32_t> m_cooldown_list;

    /// MOVEABLE OBJECT MEMBER
	Coord m_NextPos;

    uint8_t m_CurrentTask;
};

#endif //__SLES_NPCS_HPP__
