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

#ifndef __SLES_WORLD_MANAGER_HPP__
#define __SLES_WORLD_MANAGER_HPP__

#include "task_background.hpp"

/// SRLF INCLUDES
#include <drop.hpp>
#include <coord.hpp>
#include <teleport.hpp>
#include <spawn_point.hpp>
#include <level_property.hpp>
#include <magicoption_property.hpp>

/// GLOBAL INCLUDES
#include <vector>
#include <deque>
#include <boost/asio.hpp>
#include <boost/unordered_map.hpp>
#include <boost/thread/thread.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <boost/shared_array.hpp>

enum
{
	TIGERWOMAN,
	URUCHI,
	ISYUTARU,
	CERBERUS,
	IVY,
	MAX_UNIQUES
};

class Server;

class FortressManager;
class EventManager;
class EntityManager;
class WeatherManager;
class ScriptManager;
class GuildManager;
class UnionManager;
class AcademyManager;
class PartyManager;
class StallManager;
class TradeManager;
class SkillManager;
class MapManager;
class ModelManager;
class FriendManager;

class NPC;
class Mob;
class Player;
class Skill;
class DropableItem;
class TeleportBuilding;

class ItemFactory;
class CharacterFactory;

class Zone;

class World : public boost::enable_shared_from_this<World>
{
public:

    typedef boost::unordered_map<uint32_t,boost::asio::deadline_timer*>::iterator timer_iterator;
    typedef boost::unordered_map<uint32_t,boost::asio::deadline_timer*>::const_iterator timer_const_iterator;

    typedef boost::unordered_map<int16_t,boost::shared_ptr<Zone> >::iterator zone_iterator;
    typedef boost::unordered_map<int16_t,boost::shared_ptr<Zone> >::const_iterator zone_const_iterator;

    typedef boost::unordered_map<uint16_t,Teleport>::iterator teleport_iterator;
    typedef boost::unordered_map<uint16_t,Teleport>::const_iterator teleport_const_iterator;

    typedef boost::unordered_map<uint32_t,boost::shared_ptr<Player> >::iterator player_iterator;
    typedef boost::unordered_map<uint32_t,boost::shared_ptr<Player> >::const_iterator player_const_iterator;

    World (Server *server);

    ~World ();

    bool Initialize ();

    void Release ();

    void Run ();

    void Stop ();

    const std::string& GetLastError () const;

    void Configure (const std::map<std::string,std::string> &config_entries);

    boost::shared_ptr<MapManager> GetMapManager ();

    boost::shared_ptr<EventManager> GetEventManager ();

    boost::shared_ptr<WeatherManager> GetWeatherManager ();

    boost::shared_ptr<FortressManager> GetFortressManager ();

    boost::shared_ptr<ScriptManager> GetScriptManager ();

    boost::shared_ptr<GuildManager> GetGuildManager ();

    boost::shared_ptr<UnionManager> GetUnionManager ();

    boost::shared_ptr<AcademyManager> GetAcademyManager ();

    boost::shared_ptr<PartyManager> GetPartyManager ();

    boost::shared_ptr<StallManager> GetStallManager ();

    boost::shared_ptr<TradeManager> GetTradeManager ();

    boost::shared_ptr<SkillManager> GetSkillManager ();

    boost::shared_ptr<FriendManager> GetFriendManager ();

    boost::shared_ptr<ItemFactory> GetItemFactory ();

    boost::shared_ptr<CharacterFactory> GetCharacterFactory ();

    bool IsInsideTown (const Coord &position) const;

    bool resolveMotion (const Coord &src, const Coord &dest, Coord *end) const;

    /**
     *
     *  NPC MANAGEMENT FUNCTIONS.
     *
     **/

    void InsertNPC (const boost::shared_ptr<NPC> &npc);

    void InsertNPC (const std::vector<boost::shared_ptr<NPC> > &list, const Coord &pos);

    void RemoveNPC (uint32_t npcID, const Coord &pos);

    boost::shared_ptr<NPC> FindNPC_CloseToPosition (uint32_t npcID, const Coord &pos) const;

    /**
     *
     *  ITEM MANAGEMENT FUNCTIONS.
     *
     **/

    void InsertItem (const boost::shared_ptr<DropableItem> &item);

    void RemoveItem (const uint32_t itemID, const Coord &pos);

    boost::shared_ptr<DropableItem> FindItem_CloseToPosition (uint32_t itemID, const Coord &pos) const;

    /**
     *
     *  TELEPORT BUILDINGS MANAGEMENT FUNCTIONS.
     *
     **/

    void InsertBuilding (const boost::shared_ptr<TeleportBuilding> &building);

    void RemoveBuilding (const uint32_t buildingID, const Coord &pos);

    boost::shared_ptr<TeleportBuilding> FindBuilding_CloseToPosition (const Coord &pos);

    boost::shared_ptr<TeleportBuilding> FindBuilding_CloseToPosition (uint32_t buildingID, const Coord &pos);

    /**
     *
     *  @brief Insert a new player into the world, and start listening to events in adjacent zones.
     *  @return success if its the player is listening to adjacent zones, fail otherwise.
     *
     **/

    bool InsertPlayer (const boost::shared_ptr<Player> &player);

    /**
     *
     *  @brief Remove a player from the world.
     *
     **/

    void RemovePlayer (const uint32_t playerID);

    boost::shared_ptr<Player> FindPlayer_ByID (uint32_t playerID) const;

    boost::shared_ptr<Player> FindPlayer_ByName (const std::string &name) const;

    boost::shared_ptr<Player> FindPlayer_CloseToPosition (uint32_t playerID, const Coord &pos) const;

    /**
     *
     *  FOREGROUND TASKS FUNCTIONS FOR PLAYER
     *
     **/

    bool InitTaskIdle (const uint32_t playerID);

    bool InitTaskTeleport (const uint32_t playerID, const Coord &pos, const uint32_t time);

    bool InitTaskMove (const uint32_t playerID);

    bool InitTaskMoveTarget (const uint32_t playerID, const uint32_t targetID, const uint32_t skillID);

    bool InitTaskMovePickup (const uint32_t playerID, const uint32_t objectID);

    bool InitTaskCastSkill (const uint32_t playerID, const uint32_t targetID, const uint32_t skillID,
        const Coord &pos);

    bool InitTaskExecSkill (const uint32_t playerID, const uint32_t targetID, const uint32_t skillID,
        const Coord &pos);

    bool InitTaskWaitSkill (const uint32_t playerID, const uint32_t targetID, const uint32_t msDelay,
        const uint32_t skillID);

    bool InitTaskPickup (const uint32_t playerID, const uint32_t objectID);

    void InitTaskPickupExec (const uint32_t playerID, const uint32_t objectID);

    bool InitTaskMoveItem (const uint32_t playerID, const uint8_t src, const uint8_t dest, const uint16_t quantity,
        const uint8_t time);

    /**
     *
     *  @brief Initiate alchemy operation.
     *  @param playerID player requesting alchemy operation.
     *  @param ipos Item position in player inventory (required).
     *  @param epos Elixir position in player inventory (required).
     *  @param lpos Lucky powder position in player inventory (optional).
     *  @param advanced Indicates if the elixir used is advanced(100% success) or normal one.
     *  @return If the operation started or failed.
     *
     **/

    bool InitTaskAlchemy (const uint32_t playerID, uint8_t ipos, uint8_t epos, uint8_t lpos, bool advanced);

    /**
     *
     *  FOREGROUND TASKS FUNCTIONS FOR NPC
     *
     **/

    void InitTaskAI (const boost::shared_ptr<NPC> &npc);

    void InitTaskIdleNPC (const boost::shared_ptr<NPC> &npc);

    void InitTaskCastNPC (const boost::shared_ptr<NPC> &npc);

    void InitTaskExecCastNPC (const boost::shared_ptr<NPC> &npc, const uint32_t skillID = 0);

    void InitTaskWaitCastNPC (const boost::shared_ptr<NPC> &npc, const uint32_t msDelay,
        const uint32_t skillID);

    void InitTaskMoveNPC (const boost::shared_ptr<NPC> &npc);

    void InitTaskTraceNPC (const boost::shared_ptr<NPC> &npc, const uint32_t playerID);

    void InitTaskTraceMoveNPC (boost::shared_ptr<NPC> &npc, const uint32_t playerID);

    /**
     *
     *  FOREGROUND TASKS FUNCTIONS FOR ITEMS
     *
     **/

    bool InitTask_ItemDespawn (const uint32_t itemID, const int16_t zone, const uint32_t time);

    /**
     *
     *  @brief Cancel a running task.
     *
     **/

    uint32_t TerminateTask (const uint32_t playerID);

    /**
     *
     *  BACKGROUND TASKS FUNCTIONS
     *
     **/

    uint32_t InitBackgroundTask (BTask &tasks, bool changeID = true);

    uint32_t GetBackgroundTaskTime (const uint32_t taskID);

    void TerminateBackgroundTask (const uint32_t taskID);

    void InitStateTask (const STask &task);

    void TerminateStateTask (const uint32_t playerID);

    /**
     *
     *  SEARCH THE TELEPORT INFO BY ID.
     *  return if succedded returns the teleport spawn point else returns a teleport with ID = 0.
     *
     **/

    Teleport FindTeleport (uint16_t ID) const;

    TeleportOptional FindTeleportOptional (uint16_t ID) const;

    TeleportLink FindTeleportLink (uint16_t ID, uint32_t dst) const;

    uint8_t GetMaxCap () const;

    int32_t GetRequiredSP (const uint8_t lv) const;

    uint16_t GetShopItemStack (const uint32_t itemID) const;

    LevelProperty GetXPInformation (const uint8_t level) const;

    MagicOptionProperty GetMagicOptionProperty (const std::string &name, const uint8_t degree) const;

    bool IsEuropeanAllowed () const;

    bool IsChineseAllowed () const;

protected:

    void CreateTimer (const uint32_t timerID);

    void RemoveTimer (const uint32_t timerID);

    bool OnMovePlayer (boost::shared_ptr<Player> &player, uint32_t dt, bool with_transport);

    bool OnMoveInRange (boost::shared_ptr<Player> &caster, boost::shared_ptr<NPC> &target, uint32_t skillID,
        float range);

    bool OnMoveNPC (boost::shared_ptr<NPC> &npc, uint32_t dt);

    void OnAttackTarget (const boost::shared_ptr<NPC> &attacker, const boost::shared_ptr<Skill> &skill,
        const boost::shared_ptr<NPC> &target, const uint32_t attackID, const uint8_t bonus = 0);

    void OnBuffTarget (const boost::shared_ptr<NPC> &caster, const boost::shared_ptr<Skill> &skill,
        const boost::shared_ptr<NPC> &target, const uint32_t attackID, const uint32_t targetID);

    boost::shared_ptr<NPC> GetTarget (const uint32_t targetID, const Coord &pos) const;

    void OnJoinArea (const uint32_t playerID, const uint32_t connectionID, const uint8_t min_x, const uint8_t max_x,
        const uint8_t min_y, const uint8_t max_y);

    void OnLeaveArea (const uint32_t playerID, const uint32_t connectionID, const uint8_t min_x, const uint8_t max_x,
        const uint8_t min_y, const uint8_t max_y);

    void OnJoinArea (const boost::shared_ptr<NPC> &npc, const uint8_t min_x, const uint8_t max_x,
        const uint8_t min_y, const uint8_t max_y);

    void OnLeaveArea (const uint32_t npcID, const uint8_t min_x, const uint8_t max_x,
        const uint8_t min_y, const uint8_t max_y);

    /**
     *
     *  @brief Spawn a player in the selected zone.
     *  @playerID: player ID to spawn.
     *  @zone: ID of the area to notify.
     *  @priv: if the broadcast is for players only in the area or not.
     *
     **/

    void OnSpawnPlayer (const uint32_t playerID, const int16_t zone, bool priv);

    void OnTaskTeleport (const uint32_t playerID, const Coord &pos, const boost::system::error_code &error);

    void OnTaskMove (const uint32_t playerID, uint32_t dt, const boost::system::error_code &error);

    void OnTaskMoveItem (const uint32_t playerID, const uint8_t src, const uint8_t dest, const uint16_t quantity,
        const uint8_t time, const boost::system::error_code &error);

    void OnTaskMoveTarget (const uint32_t playerID, const uint32_t targetID, const uint32_t skillID,
        uint32_t dt, const boost::system::error_code &error);

    void OnTaskMovePickup (const uint32_t playerID, const uint32_t objectID, uint32_t dt,
        const boost::system::error_code &error);

    void OnTaskExecSkill(const uint32_t playerID, const uint32_t targetID, const uint32_t skillID,
        const uint32_t attackID, const Coord &pos, const boost::system::error_code &error);

    void OnTaskWaitSkill(const uint32_t playerID, const uint32_t targetID, const uint32_t skillID,
        const boost::system::error_code &error);

    void OnTaskAlchemy (const uint32_t playerID, uint8_t ipos, uint8_t epos, uint8_t lpos, bool advanced,
        const boost::system::error_code &error);

    void OnTaskIdle (const uint32_t playerID, const boost::system::error_code &error);

    /**
     *
     *  NPC AND ITEM TASK HANDLERS
     *
     **/

    void OnTaskIdleNPC (boost::shared_ptr<NPC> &npc, const boost::system::error_code &error);

    void OnTaskExecCastNPC (boost::shared_ptr<NPC> &npc, const uint32_t attackID,
        const boost::system::error_code &error);

    void OnTaskWaitCastNPC (boost::shared_ptr<NPC> &npc, const uint32_t skillID,
        const boost::system::error_code &error);

    void OnTaskMoveNPC (boost::shared_ptr<NPC> &npc, uint32_t dt, const boost::system::error_code &error);

    void OnTaskTraceNPC (boost::shared_ptr<NPC> &npc, const uint32_t playerID,
        const boost::system::error_code &error);

    void OnTaskTraceMoveNPC (boost::shared_ptr<NPC> &npc, const uint32_t playerID, uint32_t dt,
        const boost::system::error_code &error);

    void OnTaskDespawn (const uint32_t objID, const int16_t zone, bool IsNPC, const boost::system::error_code &error);

    void OnBackgroundTask (const boost::system::error_code &error);

    void OnBackgroundTaskExec (const BTask &task);

    void OnBackgroundTaskRemove (const uint32_t taskID);

    void OnStateTask (const boost::system::error_code &error);

    void OnStateTaskRemove (const uint32_t ID);

    void OnStateTaskExec (const STask &task);

    void OnRespawnMob (const boost::system::error_code &error);

    /**
     *
     *  ZONE AI (DE)ACTIVATION HANDLERS
     *
     **/

    void OnActivateZoneAI (const int16_t zoneID);

    void OnDeactivateZoneAI (const int16_t zoneID);

    /**
     *
     *  NPC SIGNALS HANDLING
     *
     **/

    void OnSignalMobDead (const uint32_t entityID, const int16_t zoneID);

    void OnSignalTransportDead (const uint32_t transportID, const int16_t zoneID);

    void OnSignalHPMP (const uint32_t npcID, const int16_t zoneID);

    //////

    void SpawnUniqueRandomPosition (const uint32_t unique_id);

    void OnSpawnUnique (uint8_t Index, uint32_t refCharID, const boost::system::error_code& error);

    std::vector<boost::shared_ptr<DropableItem> > GetDropList (const uint32_t refCharID, uint8_t lv,
        int MobQuality) const;

private:

    Server *m_server;

    boost::shared_ptr<EventManager> m_EventMgr;
    boost::shared_ptr<MapManager> m_MapMgr;
    boost::shared_ptr<ModelManager> m_ModelMgr;
    boost::shared_ptr<WeatherManager> m_WeatherMgr;
    boost::shared_ptr<FortressManager> m_FortressMgr;
    boost::shared_ptr<ScriptManager> m_ScriptMgr;
    boost::shared_ptr<GuildManager> m_GuildMgr;
    boost::shared_ptr<UnionManager> m_UnionMgr;
    boost::shared_ptr<AcademyManager> m_AcademyMgr;
    boost::shared_ptr<PartyManager> m_PartyMgr;
    boost::shared_ptr<StallManager> m_StallMgr;
    boost::shared_ptr<TradeManager> m_TradeMgr;
    boost::shared_ptr<SkillManager> m_SkillMgr;
    boost::shared_ptr<FriendManager> m_FriendMgr;

    boost::shared_ptr<ItemFactory> m_ItemFactory;
    boost::shared_ptr<CharacterFactory> m_CharacterFactory;

    boost::unordered_map<uint16_t,Teleport> m_TeleportChart;
    boost::unordered_map<uint16_t,TeleportOptional> m_TeleportOptionalChart;
    std::multimap<uint16_t,TeleportLink> m_TeleportLinkChart;

    boost::unordered_map<uint8_t,LevelProperty> m_XPChart;
    boost::unordered_map<uint8_t,std::pair<uint16_t,uint16_t> > m_GoldChart;

    boost::unordered_map<uint32_t,uint16_t> m_ShopItemStacks;

    std::vector<MagicOptionProperty> m_MagicOptionChart;

    boost::unordered_multimap<uint32_t,Drop> m_DropChart;
    boost::unordered_multimap<uint32_t,DropWeight> m_DropWeightChart;

    std::multimap<uint32_t,SpawnPoint> m_UniqueSpawnPoints;

    mutable boost::mutex m_player_mutex;
    boost::unordered_map<uint32_t,boost::shared_ptr<Player> > m_PlayerTable;

    bool m_stop_flag;
    boost::asio::io_service m_io_service;

    boost::asio::deadline_timer m_time_tg;
    boost::asio::deadline_timer m_time_uru;
    boost::asio::deadline_timer m_time_isy;
    boost::asio::deadline_timer m_time_cerb;
    boost::asio::deadline_timer m_time_ivy;

    boost::asio::deadline_timer m_world_timer;

    bool m_unique_spawned[MAX_UNIQUES];

    std::string m_last_error;

    std::string m_db;
    std::string m_db_host;
    std::string m_db_usr;
    std::string m_db_pw;
    std::string m_path_media;   /// ITEM,SKILL,CHARACTER,ETC DATA
    std::string m_path_data;    /// DUNGEON,BSR DATA
    int m_thread_count;
    int m_spawn_count;
    uint8_t m_max_cap;
    bool m_market;
    bool m_allow_ch;
    bool m_allow_eu;

    /// FOREGROUND TASKS
    boost::mutex m_task_mutex;
    boost::unordered_map<uint32_t,boost::asio::deadline_timer*> m_tasks;

    /// BACKGROUND TASKS
    boost::mutex m_bg_task_mutex;
    uint32_t m_bg_counter;
    boost::unordered_map<uint32_t,BTask> m_bg_tasks;
    boost::asio::deadline_timer m_bg_task_timer;

    /// STATE TASKS
    boost::mutex m_state_task_mutex;
    boost::unordered_map<uint32_t,STask> m_state_tasks;
    boost::asio::deadline_timer m_state_task_timer;

    /// SPAWN POINT // RESPAWN
    boost::mutex m_spawn_mutex;
    std::vector<SpawnPoint> m_spawn_list;
    std::deque<std::pair<uint32_t,size_t> > m_spawn_deque;
    boost::asio::deadline_timer m_spawn_timer;

    boost::asio::io_service::work m_work;
    boost::thread_group m_threads;
};

#endif //__SLES_WORLD_MANAGER_HPP__
