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

#ifndef __SLES_PLAYER_HPP__
#define __SLES_PLAYER_HPP__

#include "npc.hpp"
#include "item.hpp"
#include "mastery_tree.hpp"
#include "job.hpp"
#include "quest.hpp"
#include "storage_constants.hpp"
#include "invitation_type.hpp"
#include "hotkey.hpp"
#include "autopot.hpp"
#include "player_property.hpp"

#include <boost/enable_shared_from_this.hpp>

#define MAX_PLAYER_BERSERK_COUNT 5
#define MAX_PLAYER_BERSERK_TIME 60
#define MAX_PLAYER_NAME_LENTH 12
#define PLAYER_INVUNERABLE_STATE_TIME 5

class Stall;
class Storage;
class ExchangeWnd;
class Skill;
class Guild;
class Party;
class Transport;

enum PLAYER_RANK
{
	RANK_GRUNT,
	RANK_CAPTAIN,
	RANK_COMMANDER
};

/**
 *
 *  @brief Available player task.
 *  Any task after PLAYER_TASK_ITEM_MOVE are not interruptable.
 *
 **/

enum PLAYER_TASK
{
    PLAYER_TASK_IDLE,
    PLAYER_TASK_MOVE,
    PLAYER_TASK_MOVE_TRANSPORT,
    PLAYER_TASK_MOVE_TO_TARGET,
    PLAYER_TASK_MOVE_TO_ITEM,
    PLAYER_TASK_CAST_WAIT,
    PLAYER_TASK_ITEM_MOVE,
    PLAYER_TASK_CAST,
    PLAYER_TASK_STALL,
    PLAYER_TASK_EXCHANGE,
    PLAYER_TASK_ALCHEMY,
    PLAYER_TASK_TELEPORT,
    PLAYER_TASK_NPC_TALK,
    PLAYER_TASK_STUNNED
};

namespace Exception
{

class InvalidGender
{};

class InvalidPosition
{};

class BrokenItem
{};

class InvalidType
{};

}

/**
 *
 *  TODO: FUNCTIONS FOR ->
    uint32_t AcademyID;
    uint32_t AbilityPetID;
    uint32_t AttackPetID;
    uint32_t TransportID;
 *
 **/

class Player :
	public NPC,
	public boost::enable_shared_from_this<Player>
{
public:

    boost::signals2::signal<void ()> signal_stats;

    boost::signals2::signal<void ()> m_gold_changed;

    boost::signals2::signal<void ()> m_sp_changed;

    boost::signals2::signal<void ()> signal_connect_party;

public:

	typedef NPC::attack_type attack_type;

    Player (const boost::shared_ptr<EntityProperty>& character_property);

    Player(const boost::shared_ptr<EntityProperty>& character_property, const PlayerProperty& player_property);

	/**
	 *
	 *  @param npc_prop player basic properties defined in characterdata.txt
	 *  @param ilist initial items.
	 *
	 */

    Player (const boost::shared_ptr<EntityProperty> &property, const std::vector<Item>& ilist,
		const std::string& name, uint8_t volume);

	virtual ~Player();

	/**
	 *
	 *  Set Character UniqueID (only used for DB operations).
	 *
	 **/

	void ID (const uint32_t newID);

    /**
     *
     *  Get Character UniqueID (only used for DB operations).
     *
     **/

    uint32_t ID () const;

    void ConnectionID (const uint32_t connID);

    uint32_t ConnectionID () const;

	void set_name (const std::string& name);

	std::string get_name () const;

	void set_model (uint32_t modelid);

	uint32_t get_model () const;

	void set_volume (uint8_t volume);

	uint8_t get_volume () const;

	/****************************************************************
	 *                                                              *
	 *                  PK AND MURDER FUNCTIONS                     *
	 *                                                              *
	 ****************************************************************/

	void set_pk (uint8_t pk);

	uint8_t get_pk () const;

	void set_murderlv (uint32_t lvl);

	uint32_t get_murderlv () const;

	/****************************************************************
	 *                                                              *
	 *                         GM FUNCTIONS                         *
	 *                                                              *
	 ****************************************************************/

	void set_gm (bool flag);

	bool is_gm() const;

	/**
	 *
	 *  @brief Set player rank.
	 *
	 **/

	void set_rank (PLAYER_RANK rank);

	/**
	 *
	 *  @brief Return player current rank.
	 *
	 **/

	int get_rank () const;


	void set_job (const Job &job);

	Job get_job () const;

	RACE race () const;

	/**
	 *
	 *  @brief Player current level.
	 *
	 **/

	virtual uint8_t level () const;

	/**
	 *
	 *  @brief Player maximum level achieved.
	 *
	 **/

	uint8_t get_maxlv () const;

	/****************************************************************
	 *                                                              *
	 *                      STATS FUNCTIONS                         *
	 *                                                              *
	 ****************************************************************/

    uint16_t get_stat () const;

	uint16_t get_str () const;

	/**
	 *
	 *  @brief Increase a player strength permanently
	 *  by one point.
	 *
	 **/

    void inc_str ();

	uint16_t get_int () const;

	/**
     *
     *  @brief Increase player intelligence permanently
     *  by one point.
     *
     **/

    void inc_int ();


	/****************************************************************
	 *                                                              *
	 *                   XP/SP/SPEXP FUNCTIONS                      *
	 *                                                              *
	 ****************************************************************/

	uint64_t get_exp () const;

	uint32_t get_sp () const;

	uint16_t get_spexp () const;

    /**
     *
     *  @brief Increase player experience points.
     *  @exp Amount of experience gained.
     *  @return if the player gained a new level.
     *
     **/

    void inc_exp (uint64_t exp);

    /**
     *
     *  @brief Increase player skill experience points.
     *  @exp Amount of skill experience points gained.
     *
     **/

    void inc_spexp (uint32_t exp);

    /**
     *
     *	@brief Decrease the player skill points amount.
     *	@sp Skill Points to spend.
     *	@return if the operation is allowed or denied.
     *
     **/

    bool spend_sp (uint32_t sp);

    /****************************************************************
	 *                                                              *
	 *                  ERASE & RESTORE FUNCTIONS                   *
	 *                                                              *
	 ****************************************************************/

	void erase ();

	void restore();

	uint16_t get_timeleft () const;

	void set_timeleft (uint16_t minutes);

    /**
	 *
	 *  @return erase state. True = being erased.
	 *
	 */

	bool is_erased () const;

	/**
	 *
	 *  @brief Obtain the player inventory storage where it keep all items
	 *  that belongs to him equipped or not.
	 *  @type The type of storage you want to retrieve.
	 *
	 **/

	boost::shared_ptr<Storage> get_storage (const STORAGE_TYPE type);

    /**
     *
     *  @brief: Check if the inventory was expanded or not.
     *
     **/

    bool IsInvExpanded() const;

    /**
     *
     *  @brief: Check if the storage was expanded or not.
     *
     **/

    bool IsStorageExpanded () const;

    void IncInventorySlots (const uint8_t nslots);

    void IncStorageSlots (const uint8_t nslots);

	/****************************************************************
	 *                                                              *
	 *                        QUEST FUNCTIONS                       *
	 *                                                              *
	 ****************************************************************/

	std::vector<Quest> get_completed_quest_list () const;

	std::vector<Quest> get_pending_quest_list () const;

	uint32_t aggro () const;

	bool check_gender (const Item& item) const;

	bool check_mastery (const Item& item) const;

	/**
	 *
	 *  MASTERY FUNCTIONS
	 *
	 **/

	void set_mastery_tree (const MasteryTree& tree);

    MasteryTree& get_mastery_tree ();

	const MasteryTree& get_mastery_tree () const;

	/**
	 *
	 *	@brief Decrease player mastery by one level.
	 *	@mastery_id: Mastery to decrease.
	 *	@return: if the operation is allowed or denied.
	 *
	 **/

	bool downgrade_mastery (const uint32_t mastery_id);

    uint8_t GetMasteryBonus (const uint32_t masteryID) const;

	/****************************************************************
	 *                                                              *
	 *                       SKILL FUNCTIONS                        *
	 *                                                              *
	 ****************************************************************/

	void set_skill_list (const std::set<uint32_t> &list);

	const std::set<uint32_t>& get_skill_list () const;

	void skill_insert (const uint32_t ID);

	void skill_remove (const uint32_t ID);

	bool skill_exists (const uint32_t ID) const;

	/**
	 *
	 *	@brief Downgrade player skill.
	 *	@skill: Skill to unlearn.
	 *	@return: if the operation is allowed or denied.
	 *
	 **/

	bool downgrade_skill (const boost::shared_ptr<Skill> skill);

	uint32_t get_base_attack () const;

    /**
     *
     *  GOLD FUNCTIONS
     *
     **/

    uint64_t get_gold () const;

    /**
     *
     *  @brief Increase the player current gold amount.
     *  @amount Gold amount gained.
     *
     **/

    void gain_gold (uint64_t amount);

    /**
     *
     *  @brief Decrease the player current gold amount.
     *  @amount Gold amount lost.
     *  @return if the operation is allowed or denied.
     *
     **/

    bool spend_gold (uint64_t amount);

    bool deposit_gold (const uint64_t amount);

    bool withdraw_gold (const uint64_t amount);

    uint64_t get_stored_gold () const;


	void help_mark (bool mark);

	bool help_mark () const;

	void berserk (bool activate);

	bool IsBerserk () const;

	/**
	 *
	 *  @brief Set berserk counter.
	 *
	 **/

	void set_berserk_count(uint8_t counter);

	/**
	 *
	 *  @brief Returns player current berserk count.
	 *
	 **/

	uint8_t get_berserk_count () const;

	void set_return_point (const uint32_t ID);

	uint32_t get_return_point () const;

	/**
	 *
	 *  @brief Return if the player is using a stall or not, this includes participating
	 *	or running a stall.
	 *
	 **/

	bool is_stalling () const;

	void create_stall (const std::string& name);

	void close_stall ();

	void join_stall (boost::shared_ptr<Stall> stall);

	void leave_stall ();

	boost::shared_ptr<Stall> get_stall () const;

	void set_stall_avatar (const uint32_t avatarID);

	uint32_t get_stall_avatar () const;

	/**
	 *
	 *	@brief Set the type of invitation sent to the player.
	 *
	 **/

	void set_invitation (const INVITATION_TYPE type);

	INVITATION_TYPE get_invitation () const;

	/**
	 *
	 *  @brief Set the player that your exchanging with.
	 *
	 **/

	void set_invitation_player (const uint32_t ID);

	/**
	 *
	 *  @brief Return the player your exchanging with.
	 *
	 **/

	uint32_t get_invitation_player () const;

	boost::shared_ptr<ExchangeWnd> get_exchange_window () const;

	void set_exchange_approval (bool approval);

	bool is_exchange_approved () const;

	void close_exchange ();

	/**
     *
     *  @brief: Level the player, increase available stats and str,int points.
     *  @return: if the player is allowed to level up or not.
     *
     **/

	bool levelup ();

	void create_party ();

	bool in_party () const;

	boost::shared_ptr<Party> get_party () const;

	void join_party (boost::shared_ptr<Party> party);

	void leave_party ();

	void clear_party ();

	/****************************************************************
	 *                                                              *
	 *                        GUILD FUNCTIONS                       *
	 *                                                              *
	 ****************************************************************/

    uint32_t get_guild_id () const;

	void set_guild (boost::shared_ptr<Guild> guild);

	boost::shared_ptr<Guild> get_guild () const;

	void guild_clear ();

    /****************************************************************
     *                                                              *
     *                    TRANSPORT FUNCTIONS                       *
     *                                                              *
     ****************************************************************/

    uint32_t TransportID () const;

    void SetTransport (const boost::shared_ptr<Transport> &transport);

    boost::shared_ptr<Transport> GetTransport () const;

    void TransportTerminate ();

	/**
	 *
	 *  @brief: Resurrect a dead player.
	 *  @set_invulnerable: indicates is player is invunerable after resurrecting.
	 *  @recover_xp: indicates if the player recovered a part of the XP/SPEXP lost.
	 *  @recover_percent: the percent of the lost XP/SPEXP recovered.
	 *  @max_recovery: indicates if the HP and MP must be fully recovered.
	 *
	 **/

    void resurrect (bool set_invulnerable, bool recover_xp, uint8_t recover_percent, bool max_recovery);

	void clear_invulnerable ();

	bool IsJobbing () const;

    bool IsFreePvP () const;

	void change_skin (const uint32_t refCharID, const uint8_t volume);

    void EstablishRecallPoint ();

    const Coord& GetRecallPoint () const;

    void EstablishDeathPoint ();

    const Coord& GetDeathPoint () const;

    virtual bool IsTaskInterruptable () const;

    /**
     *
     *  @brief Indicates is a item group is usable by the player or is on cooldown.
     *  @type: Item group.
     *
     **/

    bool IsItemAvailable (const uint32_t itemType) const;

    /**
     *
     *  @brief Insert an item group into the cooldown list.
     *  @taskID: associated taskID in the world queue.
     *  @itemType: Item group thats being added into cooldown.
     *
     **/

    void AddItemCooldown (const uint32_t taskID, const uint32_t itemType);

    /**
     *
     *  @brief Remove an item group from the cooldown list.
     *  @itemType: Item group.
     *
     **/

    void RemoveItemCooldown (const uint32_t itemType);

    /**
     *
     *  @brief Obtain current item group cooldown list.
     *
     **/

    const std::map<uint32_t,uint32_t>& GetItemCooldownList() const;

    std::vector<std::pair<uint32_t,uint32_t> > RemoveInvalidatedBuffs ();

    void RecalculateStats ();

protected:

	bool check_item_combination (const Item& item, const Item& new_item) const;

	virtual void calculate_stats();

protected:

    uint32_t m_ConnectionID;

	PlayerProperty m_BaseChar;  ///< Player properties.

	boost::shared_ptr<Storage> m_InvStore;
	boost::shared_ptr<Storage> m_AvaStore;
	boost::shared_ptr<Storage> m_MainStore;

	boost::shared_ptr<Guild> m_Guild;

	MasteryTree m_mastery_tree;
	std::set<uint32_t> m_SkillList;

	std::vector<Quest> m_CompletedQuests;
	std::vector<Quest> m_PendingQuests;

	Job m_Job;
	boost::shared_ptr<Stall> m_Stall;
	boost::shared_ptr<Party> m_Party;
	boost::shared_ptr<ExchangeWnd> m_ExchangeWnd;

	bool m_exchange_approve;
	bool m_help_mark;

	INVITATION_TYPE m_Invitation;

	uint32_t m_invitation_player;

	uint32_t m_TransportID;

	uint16_t m_Str;
	uint16_t m_Int;

	uint16_t m_HPRgn;	//HP RENEGERATION
	uint16_t m_MPRng;	//MP REGENERATION

	uint8_t m_IGD;		//IGNORE NPC DEFENSE
	uint8_t m_CPR;		//CRITICAL PARRY RATIO
	uint8_t m_GoldDrop;
	uint8_t m_MOL;		//MAGIC OPTION LUCK
	uint8_t m_ETL;		//ENCHANT LUCK

	uint8_t m_MPDec;
	uint8_t m_HealHP;	//HEAL HP INCREASE BONUS
	uint8_t m_HealMP;	//HEAL MP INCREASE BONUS

    Coord m_RecallPoint;    /// LAST TELEPORT RETURN POINT.
    Coord m_DeathPoint;     /// LAST DEATH POSITION.

    std::map<uint32_t,uint32_t> m_cooldown_itemgroup;

public:

    boost::shared_ptr<Transport> m_transport;

    AutopotSetting m_Autopot;
    std::vector<Hotkey> m_Hotkeys;
    std::vector<std::string> m_BlockList;
};

/**
 *
 *  @brief Check if two player can interact with other in combat.
 *  While jobbing the traders with hunter and thief with thief.
 *
 *  @p1 First player being checked.
 *  @p2 Second player being checked.
 *
 **/

bool CanPlayerInteract (const boost::shared_ptr<Player> &p1, const boost::shared_ptr<Player> &p2);

#endif //__SLES_PLAYER_HPP__
