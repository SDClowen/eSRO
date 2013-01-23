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

#ifndef SERVER_STATE_GAME_HPP
#define SERVER_STATE_GAME_HPP

#include <coord.hpp>
#include <storage.hpp>

#include <server_connection_state.hpp>

#include <boost/asio.hpp>
#include <boost/shared_array.hpp>

class Player;

class Server;

namespace srv
{
    class Connection;
}

class StateGame : public srv::ConnectionState
{
public:

    StateGame (Server *server, srv::Connection *conn);

    virtual ~StateGame ();

    virtual void Close ();

private:

    /**
     *
     *  Handle player enter world request.
     *
     **/

    int OnLoad (IPacket &packet);

    /**
     *
     *  Handle player leave world request.
     *
     **/

    int OnLogout (IPacket &packet);

    int OnLogoutCancel (IPacket &packet);

    void OnLogoutTimeout (const boost::system::error_code &error);

    /**
     *
     *  Handle player emote request.
     *
     **/

    int OnEmote (IPacket &packet);

    int OnMovement (IPacket &packet);

    int OnSelectTarget (IPacket &packet);

    int OnUseStorage (IPacket &packet);

    /**
     *
     *  Handle player display of the help icon above their head.
     *
     **/

    int OnHelpIcon (IPacket &packet);

    /**
     *
     *  HANDLE PLAYER CONFIGURATION (HOTKEY AND AUTOPOT).
     *
     **/

    int OnSetting (IPacket &packet);

    int OnSetHotkey (IPacket &packet);

    int OnSetAutopot (IPacket &packet);

    /**
     *
     *  Handle player stance (SIT AND STANDUP).
     *
     **/

    int OnStance (IPacket &packet);

    int OnCastBerserk (IPacket &packet);

    int OnTranslate (IPacket &packet);

    int OnRotate (IPacket &packet);

    int OnResurrect (IPacket &packet);

    int OnBlock (IPacket &packet);

    int OnPlayerAction (IPacket &packet);

    int OnPlayerAction_Attack (IPacket &packet);

    int OnPlayerAction_Pickup (IPacket &packet);

    int OnPlayerAction_Trace (IPacket &packet);

    int OnPlayerAction_SkillCast (IPacket &packet);

    int OnPlayerAction_SkillCancel (IPacket &packet);

    /**
     *
     *  HANDLE PLAYER STAT POINTS FUNCTIONS.
     *
     **/

    int OnStatsIntInc (IPacket &packet);

    int OnStatsStrInc (IPacket &packet);

    /**
     *
     *  HANDLE PLAYER SKILLS & MASTERY FUNCTIONS.
     *
     **/

    int OnSkillInc (IPacket &packet);

    int OnSkillEdit (IPacket &packet);

    int OnMasteryInc (IPacket &packet);

    /**
     *
     *  HANDLE PLAYER CHAT FUNCTIONS.
     *
     **/

    int OnMsg (IPacket &packet);

    int OnMsgPublic (IPacket &packet, const uint8_t num_msg, const uint8_t linkCount);

    int OnMsgPrivate (IPacket &packet, const uint8_t num_msgs, const uint8_t linkCount);

    int OnMsgParty (IPacket &packet, const uint8_t num_msgs, const uint8_t linkCount);

    int OnMsgGuild (IPacket &packet, const uint8_t num_msgs, const uint8_t linkCount);

    int OnMsgStall (IPacket &packet, const uint8_t num_msgs, const uint8_t linkCount);

    int OnMsgNotice (IPacket &packet, const uint8_t num_msgs, const uint8_t linkCount);

    int OnMsgAcademy (IPacket &packet, const uint8_t num_msgs, const uint8_t linkCount);

    int OnMsgUnion (IPacket &packet, const uint8_t num_msgs, const uint8_t linkCount);

    /**
     *
     *  HANDLE NPC TALK FUNCTIONS.
     *
     **/

    int OnTalkInitiate (IPacket &packet);

    int OnTalkTerminate (IPacket &packet);

    /**
     *
     *  HANDLE PLAYER STALL FUNCTIONS.
     *
     **/

    int OnStallCreate (IPacket &packet);

    int OnStallAction (IPacket &packet);

    int OnStallEnter (IPacket &packet);

    int OnStallBuy (IPacket &packet);

    int OnStallLeave (IPacket &packet);

    int OnStallClose (IPacket &packet);

    int OnStallNetworkSearch (IPacket &packet);

    int OnStallNetworkPurchase (IPacket &packet);

    /**
     *
     *	STALL EVENT SIGNALS
     *
     **/

    void OnStallEvent_Greeting (const std::string& greeting);

    void OnStallEvent_State (const bool is_open);

    /**
     *
     *	@brief This event is triggered when a new player enter to the stall.
     *	@playerID: PlayerID that entered the stall.
     *
     **/

    void OnStallEvent_Enter (const uint32_t playerID, const std::string &name);

    /**
     *
     *	@brief This event is triggered when a player leave the stall is currently in.
     *	@player_id: PlayerID that left stall.
     *
     **/

    void OnStallEvent_Leave (const uint32_t player_id);

    void OnStallEvent_Buy (const uint8_t pos,const std::string &name);

    void OnStallEvent_Price (const uint8_t pos, const uint16_t quantity, const uint64_t price);

    void OnStallEvent_Close ();

    /**
     *
     *	@brief This event is triggered when a player sends message to the stall is currently in.
     *	@name: Senders name.
     *	@msg: Message sent.
     *
     **/

    void OnStallEvent_Msg (const std::string &name, const std::string &msg);

    uint32_t OnStallEvent_Insert (const Item& item, const STORAGE_TYPE type);

    void OnStallEvent_Remove (const uint32_t itemID);

    /**
     *
     *  HANDLE PLAYER EXCHANGE FUNCTIONS.
     *
     **/

    int OnExchangeRequest (IPacket &packet);

    int OnExchangeConfirm (IPacket &packet);

    int OnExchangeApprove (IPacket &packet);

    int OnExchangeClose (IPacket &packet);

    /**
     *
     *  HANDLE PLAYER ITEM FUNCTIONS.
     *
     **/

    int OnItemMove (IPacket &packet);

    /**
     *
     *  MOVE AN ITEM INSIDE THE CHARACTER INVENTORY.
     *
     **/

    int OnItemMove_FromInvToInv (IPacket &packet);

    int OnItemMove_FromInvToInv_Async (const uint8_t src, const uint8_t dest, const uint16_t quantity,
        Item *isrc, Item *idest, const boost::system::error_code &error);

    /**
     *
     *  MOVE AN ITEM INSIDE THE CHARACTER STORAGE.
     *
     **/

    int OnItemMove_FromStToSt (IPacket &packet);

    int OnItemMove_FromInvToSt (IPacket &packet);

    int OnItemMove_FromStToInv (IPacket &packet);

    int OnItemMove_St_Deposit_Gold (IPacket &packet);

    int OnItemMove_St_Withdraw_Gold (IPacket &packet);

    /**
     *
     *  MOVE AN ITEM FROM THE CHARACTER INVENTORY TO THE EXCHANGE WINDOW.
     *
     **/

    int OnItemMove_FromInvToEx (IPacket &packet);

    int OnItemMove_FromExToInv (IPacket &packet);

    int OnItemMove_Exchange_Gold (IPacket &packet);

    /**
     *
     *  MOVE AN ITEM FROM THE CHARACTER INVENTORY TO THE FLOOR.
     *
     **/

    int OnItemMove_FromInvToFloor_I (IPacket &packet);

    /**
     *
     *  MOVE GOLD FROM THE CHARACTER INVENTORY TO THE FLOOR.
     *
     **/

    int OnItemMove_FromInvToFloor_G (IPacket &packet);

    int OnItemMove_FromInvToNpc (IPacket &packet);

    int OnItemMove_FromTransportToTransport (IPacket &packet);

    int OnItemMove_FromNpcToTransport (IPacket &packet);

    int OnItemMove_FromTransportToNpc (IPacket &packet);

    int OnItemMove_FromNpcToInv (IPacket &packet);

    int OnItemMove_FromMallToInv (IPacket &packet);

    /**
     *
     *  MOVE AN ITEM FROM THE CHARACTER INVENTORY TO THE GUILD STORAGE.
     *
     **/

    int OnItemMove_FromGuildStToGuildSt (IPacket &packet);

    int OnItemMove_FromInvToGuildSt (IPacket &packet);

    int OnItemMove_FromGuildStToInv (IPacket &packet);

    int OnItemMove_GuildSt_Deposit_Gold (IPacket &packet);

    int OnItemMove_GuildSt_Withdraw_Gold (IPacket &packet);

    /**
     *
     *  MOVE AN ITEM FROM THE CHARACTER INVENTORY TO THE AVATAR INVENTORY.
     *
     **/

    int OnItemMove_FromInvToAva (IPacket &packet);

    int OnItemMove_FromAvaToInv (IPacket &packet);

    int OnItemBuyBack (IPacket &packet);

    int OnItemUse (IPacket &packet);

    int OnItemUse_Return (const uint8_t pos, IPacket &packet);

    int OnItemUse_ReverseReturn (const uint8_t pos, IPacket &packet);

    int OnItemUse_ResurrectScroll (const uint8_t pos, IPacket &packet);

    int OnItemUse_Global (const uint8_t pos, IPacket &packet);

    int OnItemUse_BuffScroll (const uint8_t pos, IPacket &packet);

    int OnItemUse_HPPots (const uint8_t pos, IPacket &packet);

    int OnItemUse_MPPots (const uint8_t pos, IPacket &packet);

    int OnItemUse_VigorPots (const uint8_t pos, IPacket &packet);

    int OnItemUse_PurificationPill (const uint8_t pos, IPacket &packet);

    int OnItemUse_CureAllPill (const uint8_t pos, IPacket &packet);

    int OnItemUse_TransportScroll (const uint8_t pos, IPacket &packet);

    int OnItemUse_TransportPot (const uint8_t pos, const IPacket &packet);

    int OnItemUse_TransportCureAll (const uint8_t pos, const IPacket &packet);

    int OnItemUse_AttackPet (const uint8_t pos, IPacket &packet);

    int OnItemUse_StallAvatar (const uint8_t pos, IPacket &packet);

    int OnItemUse_ZerkPotion (const uint8_t pos, IPacket &packet);

    int OnItemUse_RepairHammer (const uint8_t pos, IPacket &packet);

    int OnItemUse_StorageSpaceExpansion (uint8_t pos, IPacket &packet);

    int OnItemUse_SkinChange (uint8_t pos, IPacket &packet);

    int OnItemUse_RegisterEmblem (uint8_t pos, IPacket &packet);

    int OnItemUse_RemoveEmblem (uint8_t pos, IPacket &packet);

    int OnItemRepair (IPacket &packet);

    int OnItemRepair_Single (IPacket &packet);

    int OnItemRepair_All (IPacket &packet);

    int OnItemAvatarMagicOption (IPacket &packet);

    /**
     *
     *  STORAGE SIGNALS
     *
     **/

    uint32_t OnStorageEvent_Insert (const Item &item, const STORAGE_TYPE &type);

    void OnStorageEvent_Position (const uint32_t itemID, const uint8_t pos);

    void OnStorageEvent_Remove (const uint32_t itemID);

    void OnStorageEvent_Quantity (const uint32_t itemID, const uint16_t quantity);

    /**
     *
     *  TRANSPORT FUNCTIONS
     *
     **/

    int OnTransportMovement (IPacket &packet);

    int OnTransportAction (IPacket &packet);

    int OnTransportTerminate (IPacket &packet);

    /**
     *
     *  ALCHEMY FUNCTIONS
     *
     **/

    int OnAlchemyDisjoint (IPacket &packet);

    int OnAlchemyDismantle (IPacket &packet);

    int OnAlchemyManufacture (IPacket &packet);

    int OnAlchemyEnchant (IPacket &packet);

    int OnAlchemyEnchant_Beginner (IPacket &packet);

    int OnAlchemyEnchant_Advanced (IPacket &packet);

    int OnAlchemyEnchant_Cancel (IPacket &packet);

    void OnAlchemyEnchant_BeginnerTimeout (uint8_t ipos, uint8_t epos, uint8_t lpos,
        const boost::system::error_code &error);

    void OnAlchemyEnchant_AdvancedTimeout (uint8_t ipos, uint8_t epos, const boost::system::error_code &error);

    int OnAlchemyMagicOption (IPacket &packet);

    /**
     *
     *  HANDLE PLAYER PARTY FUNCTIONS.
     *
     **/

    int OnPartyCreate (IPacket &packet);

    int OnPartyAnswer (IPacket &packet);

    int OnPartyLeave (IPacket &packet);

    int OnPartyKick (IPacket &packet);

    /**
     *
     *  @brief Transfer party leadership to another member.
     *
     **/

    int OnPartyLeader (IPacket &packet);

    int OnPartySearch (IPacket &packet);

    int OnPartyInformation (IPacket &packet);

    int OnPartyForm (IPacket &packet);

    int OnPartyChange (IPacket &packet);

    int OnPartyRemove (IPacket &packet);

    int OnPartyJoin (IPacket &packet);

    int OnPartyInvite (IPacket &packet);

    /**
     *
     *	PARTY SIGNALS
     *
     **/

    void OnPartyEvent_Join (const boost::shared_ptr<Player> player);

    void OnPartyEvent_Leave (const uint32_t player_id);

    void OnPartyEvent_Kick (const uint32_t player_id);

    void OnPartyEvent_Dismiss ();

    void OnPartyEvent_Leader (const uint32_t player_id);

    void OnPartyEvent_Move (const uint32_t player_id, const Coord &position);

    void OnPartyEvent_Mastery (const uint32_t playerID, const uint32_t fMastID, const uint32_t sMastID);

    void OnPartyEvent_Msg (const std::string &name, const std::string &msg);

    void OnPartyEvent_Guild (const uint32_t player_id, const std::string &guild);

    void OnPartyEvent_Level (const uint32_t player_id, const uint8_t level);

    void OnPartyEvent_HPMP (const uint32_t playerID, const uint32_t HP, const uint32_t maxHP, const uint32_t MP,
        const uint32_t maxMP);

    /**
     *
     *  HANDLE PLAYER GUILD FUNCTIONS.
     *
     **/

    int OnGuildCreate (IPacket &packet);

    int OnGuildDonate (IPacket &packet);

    int OnGuildNotice (IPacket &packet);

    int OnGuildGrantName (IPacket &packet);

    int OnGuildPosition (IPacket &packet);

    int OnGuildAuthority (IPacket &packet);

    int OnGuildPromote (IPacket &packet);

    int OnGuildInvite (IPacket &packet);

    int OnGuildWithdraw (IPacket &packet);

    int OnGuildLeave (IPacket &packet);

    int OnGuildLeaderVote (IPacket &packet);

    int OnGuildTransferLeadership (IPacket &packet);

    int OnGuildWarPropose (IPacket &packet);

    int OnGuildWarRewardGold (IPacket &packet);

    int OnGuildStorageOpen (IPacket &packet);

    int OnGuildStorageInfo (IPacket &packet);

    int OnGuildStorageClose (IPacket &packet);

    int OnGuildDisband (IPacket &packet);

    /**
     *
     *	GUILD SIGNALS
     *
     **/

    void OnGuildEvent_Notice (const std::string &title, const std::string &msg);

    void OnGuildEvent_GP (const uint32_t GP);

    void OnGuildEvent_Promote (const uint8_t lvl, const uint32_t GP);

    void OnGuildEvent_Disband ();

    void OnGuildEvent_Alliance (const uint32_t allianceID);

    void OnGuildEvent_MemberGP (const uint32_t ID, const uint32_t GP);

    void OnGuildEvent_MemberState (const uint32_t ID, const uint8_t state);

    void OnGuildEvent_MemberGrant (const uint32_t guildID, const std::string &name, const std::string &grant);

    void OnGuildEvent_MemberJoin (const uint32_t guildID, const std::string &memberName);

    void OnGuildEvent_MemberWithdraw (const std::string &name, const uint32_t memberID, const uint8_t reason);

    void OnGuildEvent_MemberPosition (const uint32_t memberID, const uint8_t position);

    void OnGuildEvent_MemberZone (const uint32_t memberID, const int16_t zoneID);

    void OnGuildEvent_MemberLevel (const uint32_t memberID, const uint8_t level);

    void OnGuildEvent_MemberLeader (const uint32_t currentMasterID, const uint32_t nextMasterID);

    void OnGuildEvent_MemberAuthority (const uint8_t count, const boost::shared_array<uint32_t> &memberIDs,
        const boost::shared_array<uint32_t> &authority);

    /**
     *
     *  HANDLE PLAYER ALLIANCE FUNCTIONS.
     *
     **/

    int OnAllianceInvite (IPacket &packet);

    int OnAllianceSecede (IPacket &packet);

    int OnAllianceExpel (IPacket &packet);

    /**
     *
     *	ALLIANCE SIGNALS
     *
     **/


    void ConnectAllianceSignals ();

    void DisconnectAllianceSignals ();

    void OnAllianceEvent_GuildInsert (const uint32_t guildID);

    void OnAllianceEvent_GuildMember (const uint32_t guildID, const uint8_t count);

    void OnAllianceEvent_GuildLeader (const uint32_t guildID);

    void OnAllianceEvent_GuildLevel (const uint32_t guildID, const uint8_t level);

    void OnAllianceEvent_Msg (const std::string &name, const std::string &msg);

    void OnAllianceEvent_GuildRemove (const uint32_t guildID, const uint8_t reason);

    void OnAllianceEvent_Disband (const uint32_t guildID);

    /**
     *
     *  HANDLE PLAYER INVITATIONS FUNCTION
     *
     **/

    int OnInvite (IPacket &packet);

    int OnInvite_Exchange (IPacket &packet, const boost::shared_ptr<Player> &player);

    int OnInvite_Party (IPacket &packet, const boost::shared_ptr<Player> &player);

    int OnInvite_Academy (IPacket &packet, const boost::shared_ptr<Player> &player);

    int OnInvite_Guild (IPacket &packet, const boost::shared_ptr<Player> &player);

    int OnInvite_Alliance (IPacket &packet, const boost::shared_ptr<Player> &player);

    /**
     *
     *  HANDLE PLAYER FORTRESS FUNCTIONS.
     *
     **/

    int OnFortressApply (IPacket &packet);

    int OnFortressReturn (IPacket &packet);

    /**
     *
     *  HANDLE PLAYER ACADEMY FUNCTIONS.
     *
     **/

    /**
     *
     *  HANDLE PLAYER GM FUNCTIONS.
     *
     **/

    int OnGMCommand (IPacket &packet);

    int OnGMCommand_LoadMonster (IPacket &packet);

    int OnGMCommand_CreateItem (IPacket &packet);

    int OnGMCommand_MoveToUser (IPacket &packet);

    int OnGMCommand_BanUser (IPacket &packet);

    int OnGMCommand_RecallUser (IPacket &packet);

    int OnGMCommand_RecallGuild (IPacket &packet);

    int OnGMCommand_Kill (IPacket &packet);

    int OnGMCommand_BlockLogout (IPacket &packet);

    int OnGMCommand_AllowLogout (IPacket &packet);

    int OnGMCommand_LieName (IPacket &packet);

    int OnGMCommand_SpawnAllUniques (IPacket &packet);

    int OnGMCommand_ClearInventory (IPacket &packet);


    /**
     *
     *  HANDLE TELEPORT FUNCTIONS.
     *
     **/

    int OnTeleport (IPacket &packet);

    int OnTeleportDesignate (IPacket &packet);

    int OnTeleportToDestination (IPacket &packet);

    int OnTeleportCancel (IPacket &packet);

    /**
     *
     *  HANDLE TUTORIAL, QUEST AND EVENT FUNCTIONS.
     *
     **/

    int OnTutorialRequest (IPacket &packet);

    int OnEventCalendar (IPacket &packet);

    /**
     *
     *  HANDLE PLAYER FRIEND FUNCTIONS.
     *
     **/

    int OnFriendRequest (IPacket &packet);

    int OnFriendAnswer (IPacket &packet);

    int OnFriendDelete (IPacket &packet);

    int OnFriendGroupCreate (IPacket &packet);

    int OnFriendGroupDelete (IPacket &packet);

    int OnFriendGroupEntryMove (IPacket &packet);

    /**
     *
     *  HANDLE PLAYER JOB FUNCTIONS.
     *
     **/

    int OnJobJoin (IPacket &packet);

    int OnJobAlias (IPacket &packet);

    int OnJobPastInfo (IPacket &packet);

    int OnJobPayment (IPacket &packet);

    int OnJobRanking (IPacket &packet);

    int OnJobLeave (IPacket &packet);

    int OnJobProductInformation (IPacket &packet);

    /**
     *
     *  HANDLE ACADEMY FUNCTIONS.
     *
     **/

    int OnAcademyList (IPacket &packet);

    int OnAcademyRank (IPacket &packet);

    /**
     *
     *	PLAYER SIGNALS
     *
     **/

    void OnPlayerState (uint8_t type, uint8_t action);

    void OnPlayerGold ();

    void OnPlayerSP ();

    void OnPlayerStats ();

    void OnPlayerSpeed ();

    void OnPlayerDead ();

    void connect_player_signals();

    void disconnect_player_signals ();

    void connect_storage_signals (boost::shared_ptr<Storage> storage);

    void disconnect_storage_signals (boost::shared_ptr<Storage> storage);

    void connect_party_signals ();

    void connect_stall_signals ();

    void connect_guild_signals ();

    void disconnect_party_signals ();

    void disconnect_stall_signals ();

    void disconnect_guild_signals ();

private:

    boost::shared_ptr<Player> get_invitation_player (const boost::shared_ptr<Player> &player) const;

    void use_item (std::map<uint8_t,Item>::iterator &iiter,const uint16_t item_type);

    bool valid_item (std::map<uint8_t,Item>::iterator &iiter, const uint16_t item_type, const uint8_t level);

    bool CheckWeaponShieldSlots (const Item &item, const boost::shared_ptr<Player> &player,
        const boost::shared_ptr<Storage> &store);

private:

    Server *m_server;

    boost::asio::deadline_timer m_logout_timer;

    bool m_hooked;
    Coord m_TeleportPosition;
};

#endif // SERVER_STATE_GAME_HPP
