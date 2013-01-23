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

#include "server.hpp"
#include "server_state_game.hpp"
#include "server_connection.hpp"
#include "opcodes_shard_client.hpp"

#include "world.hpp"
#include "manager/friend_manager.hpp"
#include "manager/party_manager.hpp"

/// SRLF INCLUDES
#include <player.hpp>
#include <transport.hpp>
#include <guild.hpp>
#include <party.hpp>

/// SRNL INCLUDES
#include <packet_misc.hpp>
#include <packet_friend.hpp>
#include <packet_spawn.hpp>

/// SRDL INCLUDES
#include <player_query.hpp>
#include <academy_query.hpp>

StateGame::StateGame(Server *srv, srv::Connection *conn)
    : srv::ConnectionState(conn), m_server(srv), m_logout_timer(srv->GetService()), m_hooked(false)
{
    using boost::bind;

    m_opcode_table[CLIENT_GAME_LOGIN] = bind(&StateGame::OnLoad,this,_1);
    m_opcode_table[CLIENT_GAME_LOGOUT] = bind(&StateGame::OnLogout,this,_1);
    m_opcode_table[CLIENT_GAME_LOGOUT_CANCEL] = bind(&StateGame::OnLogoutCancel,this,_1);

    m_opcode_table[CLIENT_PLAYER_EMOTE] = bind(&StateGame::OnEmote,this,_1);
    m_opcode_table[CLIENT_PLAYER_HELP_ICON] = bind(&StateGame::OnHelpIcon,this,_1);
    m_opcode_table[CLIENT_PLAYER_SETTING] = bind(&StateGame::OnSetting,this,_1);
    m_opcode_table[CLIENT_PLAYER_CHAT] = bind(&StateGame::OnMsg,this,_1);
    m_opcode_table[CLIENT_PLAYER_ROTATE] = bind(&StateGame::OnRotate,this,_1);
    m_opcode_table[CLIENT_PLAYER_TRANSLATE] = bind(&StateGame::OnTranslate,this,_1);
    m_opcode_table[CLIENT_PLAYER_CAST_BERSERK] = bind(&StateGame::OnCastBerserk,this,_1);
    m_opcode_table[CLIENT_PLAYER_BLOCK] = bind(&StateGame::OnBlock,this,_1);
    m_opcode_table[CLIENT_PLAYER_MOVEMENT] = bind(&StateGame::OnMovement,this,_1);
    m_opcode_table[CLIENT_PLAYER_MOVEMENT_STATE] = bind(&StateGame::OnStance,this,_1);
    m_opcode_table[CLIENT_PLAYER_SELECT_TARGET] = bind(&StateGame::OnSelectTarget,this,_1);
    m_opcode_table[CLIENT_PLAYER_USE_STORAGE] = bind(&StateGame::OnUseStorage,this,_1);
    m_opcode_table[CLIENT_PLAYER_RESURRECT] = bind(&StateGame::OnResurrect,this,_1);
    m_opcode_table[CLIENT_PLAYER_ACTION] = bind(&StateGame::OnPlayerAction,this,_1);

    m_opcode_table[CLIENT_STATS_INT_INC] = bind(&StateGame::OnStatsIntInc,this,_1);
    m_opcode_table[CLIENT_STATS_STR_INC] = bind(&StateGame::OnStatsStrInc,this,_1);

    m_opcode_table[CLIENT_MASTERY_INC] = bind(&StateGame::OnMasteryInc,this,_1);
    m_opcode_table[CLIENT_SKILL_INC] = bind(&StateGame::OnSkillInc,this,_1);
    m_opcode_table[CLIENT_SKILL_MASTERY_EDIT] = bind(&StateGame::OnSkillEdit,this,_1);

    m_opcode_table[CLIENT_ITEM_MOVE] = bind(&StateGame::OnItemMove,this,_1);
    m_opcode_table[CLIENT_ITEM_BUY_BACK] = bind(&StateGame::OnItemBuyBack,this,_1);
    m_opcode_table[CLIENT_ITEM_USE] = bind(&StateGame::OnItemUse,this,_1);
    m_opcode_table[CLIENT_ITEM_REPAIR] = bind(&StateGame::OnItemRepair,this,_1);
    m_opcode_table[CLIENT_ITEM_AVATAR_MAGICOPTION] = bind(&StateGame::OnItemAvatarMagicOption,this,_1);

    m_opcode_table[CLIENT_TRANSPORT_MOVEMENT] = bind(&StateGame::OnTransportMovement,this,_1);
    m_opcode_table[CLIENT_TRANSPORT_ACTION] = bind(&StateGame::OnTransportAction,this,_1);
    m_opcode_table[CLIENT_TRANSPORT_TERMINATE] = bind(&StateGame::OnTransportTerminate,this,_1);

    m_opcode_table[CLIENT_ALCHEMY_DISJOINT] = bind(&StateGame::OnAlchemyDisjoint,this,_1);
    m_opcode_table[CLIENT_ALCHEMY_DISMANTLE] = bind(&StateGame::OnAlchemyDismantle,this,_1);
    m_opcode_table[CLIENT_ALCHEMY_ENCHANT] = bind(&StateGame::OnAlchemyEnchant,this,_1);
    m_opcode_table[CLIENT_ALCHEMY_MAGICOPTS] = bind(&StateGame::OnAlchemyMagicOption,this,_1);

    m_opcode_table[CLIENT_TALK_INITIATE] = bind(&StateGame::OnTalkInitiate,this,_1);
    m_opcode_table[CLIENT_TALK_TERMINATE] = bind(&StateGame::OnTalkTerminate,this,_1);

    m_opcode_table[CLIENT_TELEPORT] = bind(&StateGame::OnTeleport,this,_1);
    m_opcode_table[CLIENT_TELEPORT_DESIGNATE] = bind(&StateGame::OnTeleportDesignate,this,_1);
    m_opcode_table[CLIENT_TELEPORT_REQUEST] = bind(&StateGame::OnTeleportToDestination,this,_1);
    m_opcode_table[CLIENT_TELEPORT_CANCEL] = bind(&StateGame::OnTeleportCancel,this,_1);

    m_opcode_table[CLIENT_GUILD_CREATE] = bind(&StateGame::OnGuildCreate,this,_1);
    m_opcode_table[CLIENT_GUILD_INVITE] = bind(&StateGame::OnGuildInvite,this,_1);
    m_opcode_table[CLIENT_GUILD_GRANTNAME] = bind(&StateGame::OnGuildGrantName,this,_1);
    m_opcode_table[CLIENT_GUILD_DONATE_GP] = bind(&StateGame::OnGuildDonate,this,_1);
    m_opcode_table[CLIENT_GUILD_NOTICE] = bind(&StateGame::OnGuildNotice,this,_1);
    m_opcode_table[CLIENT_GUILD_POSITION_ALLOCATION] = bind(&StateGame::OnGuildPosition,this,_1);
    m_opcode_table[CLIENT_GUILD_AUTHORITY] = bind(&StateGame::OnGuildAuthority,this,_1);
    m_opcode_table[CLIENT_GUILD_WITHDRAW] = bind(&StateGame::OnGuildWithdraw,this,_1);
    m_opcode_table[CLIENT_GUILD_PROMOTE] = bind(&StateGame::OnGuildPromote,this,_1);
    m_opcode_table[CLIENT_GUILD_LEADER_VOTE] = bind(&StateGame::OnGuildLeaderVote,this,_1);
    m_opcode_table[CLIENT_GUILD_LEADER_TRANSFER] = bind(&StateGame::OnGuildTransferLeadership,this,_1);
    m_opcode_table[CLIENT_GUILD_STORAGE_OPEN] = bind(&StateGame::OnGuildStorageOpen,this,_1);
    m_opcode_table[CLIENT_GUILD_STORAGE_INFO] = bind(&StateGame::OnGuildStorageInfo,this,_1);
    m_opcode_table[CLIENT_GUILD_STORAGE_CLOSE] = bind(&StateGame::OnGuildStorageClose,this,_1);
    m_opcode_table[CLIENT_GUILD_WAR_PROPOSE] = bind(&StateGame::OnGuildWarPropose,this,_1);
    m_opcode_table[CLIENT_GUILD_WAR_REWARD_GOLD] = bind(&StateGame::OnGuildWarRewardGold,this,_1);
    m_opcode_table[CLIENT_GUILD_LEAVE] = bind(&StateGame::OnGuildLeave,this,_1);
    m_opcode_table[CLIENT_GUILD_DISBAND] = bind(&StateGame::OnGuildDisband,this,_1);

    m_opcode_table[CLIENT_ALLIANCE_INVITE] = bind(&StateGame::OnAllianceInvite,this,_1);
    m_opcode_table[CLIENT_ALLIANCE_SECEDE] = bind(&StateGame::OnAllianceSecede,this,_1);
    m_opcode_table[CLIENT_ALLIANCE_EXPEL] = bind(&StateGame::OnAllianceExpel,this,_1);

    m_opcode_table[CLIENT_FORTRESS_APPLY] = bind(&StateGame::OnFortressApply,this,_1);
    m_opcode_table[CLIENT_FORTRESS_RETURN] = bind(&StateGame::OnFortressReturn,this,_1);

    m_opcode_table[CLIENT_GM_COMMAND] = bind(&StateGame::OnGMCommand,this,_1);

    m_opcode_table[CLIENT_FRIEND_REQUEST] = bind(&StateGame::OnFriendRequest,this,_1);
    m_opcode_table[CLIENT_FRIEND_ANSWER] = bind(&StateGame::OnFriendAnswer,this,_1);
    m_opcode_table[CLIENT_FRIEND_DELETE] = bind(&StateGame::OnFriendDelete,this,_1);
    m_opcode_table[CLIENT_FRIEND_GROUP_CREATE] = bind(&StateGame::OnFriendGroupCreate,this,_1);
    m_opcode_table[CLIENT_FRIEND_GROUP_DELETE] = bind(&StateGame::OnFriendGroupDelete,this,_1);
    m_opcode_table[CLIENT_FRIEND_GROUP_ENTRY_ADD] = bind(&StateGame::OnFriendGroupEntryMove,this,_1);

    m_opcode_table[CLIENT_STALL_CREATE] = bind(&StateGame::OnStallCreate,this,_1);
    m_opcode_table[CLIENT_STALL_ACTION] = bind(&StateGame::OnStallAction,this,_1);
    m_opcode_table[CLIENT_STALL_CLOSE] = bind(&StateGame::OnStallClose,this,_1);
    m_opcode_table[CLIENT_STALL_ENTER] = bind(&StateGame::OnStallEnter,this,_1);
    m_opcode_table[CLIENT_STALL_LEAVE] = bind(&StateGame::OnStallLeave,this,_1);
    m_opcode_table[CLIENT_STALL_BUY] = bind(&StateGame::OnStallBuy,this,_1);
    m_opcode_table[CLIENT_STALL_NETWORK_SEARCH] = bind(&StateGame::OnStallNetworkSearch,this,_1);
    m_opcode_table[CLIENT_STALL_NETWORK_PURCHASE] = bind(&StateGame::OnStallNetworkPurchase,this,_1);

    m_opcode_table[CLIENT_EXCHANGE_REQUEST] = bind(&StateGame::OnExchangeRequest,this,_1);
    m_opcode_table[CLIENT_EXCHANGE_CONFIRM] = bind(&StateGame::OnExchangeConfirm,this,_1);
    m_opcode_table[CLIENT_EXCHANGE_APPROVE] = bind(&StateGame::OnExchangeApprove,this,_1);
    m_opcode_table[CLIENT_EXCHANGE_CLOSE] = bind(&StateGame::OnExchangeClose,this,_1);

    m_opcode_table[CLIENT_JOB_JOIN] = bind(&StateGame::OnJobJoin,this,_1);
    m_opcode_table[CLIENT_JOB_ALIAS] = bind(&StateGame::OnJobAlias,this,_1);
    m_opcode_table[CLIENT_JOB_PAST_INFO] = bind(&StateGame::OnJobPastInfo,this,_1);
    m_opcode_table[CLIENT_JOB_RANKING] = bind(&StateGame::OnJobRanking,this,_1);
    m_opcode_table[CLIENT_JOB_PAYMENT] = bind(&StateGame::OnJobPayment,this,_1);
    m_opcode_table[CLIENT_JOB_LEAVE] = bind(&StateGame::OnJobLeave,this,_1);
    m_opcode_table[CLIENT_JOB_PRODUCT_INFORMATION] = bind(&StateGame::OnJobProductInformation,this,_1);

    m_opcode_table[CLIENT_TUTORIAL_REQUEST] = bind(&StateGame::OnTutorialRequest,this,_1);
    m_opcode_table[CLIENT_EVENT_CALENDAR] = bind(&StateGame::OnEventCalendar,this,_1);

    m_opcode_table[CLIENT_PARTY_CREATE] = bind(&StateGame::OnPartyCreate,this,_1);
    m_opcode_table[CLIENT_PARTY_ANSWER] = bind(&StateGame::OnPartyAnswer,this,_1);
    m_opcode_table[CLIENT_PARTY_LEAVE] = bind(&StateGame::OnPartyLeave,this,_1);
    m_opcode_table[CLIENT_PARTY_KICK] = bind(&StateGame::OnPartyKick,this,_1);
    m_opcode_table[CLIENT_PARTY_LEADER] = bind(&StateGame::OnPartyLeader,this,_1);

    m_opcode_table[CLIENT_PARTY_SEARCH] = bind(&StateGame::OnPartySearch,this,_1);
    m_opcode_table[CLIENT_PARTY_INFORMATION] = bind(&StateGame::OnPartyInformation,this,_1);
    m_opcode_table[CLIENT_PARTY_FORM] = bind(&StateGame::OnPartyForm,this,_1);
    m_opcode_table[CLIENT_PARTY_CHANGE] = bind(&StateGame::OnPartyChange,this,_1);
    m_opcode_table[CLIENT_PARTY_REMOVE] = bind(&StateGame::OnPartyRemove,this,_1);
    m_opcode_table[CLIENT_PARTY_JOIN] = bind(&StateGame::OnPartyJoin,this,_1);
    m_opcode_table[CLIENT_PARTY_INVITE] = bind(&StateGame::OnPartyInvite,this,_1);

    m_opcode_table[CLIENT_PLAYER_INVITE_ANSWER] = bind(&StateGame::OnInvite,this,_1);

    m_opcode_table[CLIENT_ACADEMY_LIST] = bind(&StateGame::OnAcademyList,this,_1);
    m_opcode_table[CLIENT_ACADEMY_RANK] = bind(&StateGame::OnAcademyRank,this,_1);

    /*
    insert_handler<handler::AcademyDisband>(CLIENT_ACADEMY_DISBAND);

    insert_handler<handler::MessageSend>(CLIENT_MESSAGE_SEND);
    insert_handler<handler::MessageBroadcast>(CLIENT_MESSAGE_BROADCAST);
    insert_handler<handler::MessageRead>(CLIENT_MESSAGE_READ);
    insert_handler<handler::MessageDelete>(CLIENT_MESSAGE_DELETE);
    */
}

StateGame::~StateGame()
{
    Close();
}

void StateGame::Close()
{
    World *world = m_server->GetWorld();
    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (player)
    {
        disconnect_player_signals();

        world->TerminateTask(player->GetUniqueID());
        world->TerminateStateTask(player->GetUniqueID());

        m_server->GetWorld()->RemovePlayer(player->GetUniqueID());

        boost::shared_ptr<Transport> transport = player->m_transport;

        if (transport)
        {
            /// KILL TRANSPORT SO IT GETS REMOVED FROM GAME AND DROP ALL ITS ITEM IF IT HAS ANY
            transport->consume_hp(transport->hp());
//            world->RemoveNPC(transport->GetUniqueID(),transport->get_position());
            player->m_transport.reset();
        }

        boost::shared_ptr<Guild> guild = player->get_guild();

        if (guild)
        {
            disconnect_guild_signals();
            guild->member_state(player->get_name(),true);
        }

        if (player->is_stalling())
        {
            disconnect_stall_signals();
            player->close_stall();
        }

        boost::shared_ptr<Party> party = player->get_party();

        if (party)
        {
            disconnect_party_signals();
            player->leave_party();

            if (!party->get_member_count() && party->get_number())
                m_server->GetWorld()->GetPartyManager()->remove_party(party->get_number());
        }

        FriendBook book = world->GetFriendManager()->findFriendbook(player->ID());

        boost::shared_ptr<OPacket> pkt(new OPacket);
        boost::shared_ptr<Player> fplayer;

        for(FriendBook::friend_iterator i = book.friends.begin(); i != book.friends.end(); ++i)
        {
            if (i->status)
            {
                fplayer = world->FindPlayer_ByName(i->Name);

                if (fplayer)
                {
                    srv_pkt::FriendNotify(pkt,player->ID(),srv_pkt::FRIEND_ONLINE);
                    m_server->Send(pkt,player->ConnectionID());

                    pkt.reset(new OPacket);
                }
            }
        }

        /// SAVE PLAYER CURRENT INFO

        DB::PLAYER::Position pos_query;
        pos_query(m_server->DBConnection(),player->ID(),player->get_position());

        DB::PLAYER::HPMP hp_query;
        hp_query(m_server->DBConnection(),player->ID(),player->hp(),player->mp());

        DB::PLAYER::Exp xp_query;
        xp_query(m_server->DBConnection(),player);

        DB::PLAYER::Lvl lv_query;
        lv_query(m_server->DBConnection(),player);

        DB::PLAYER::Bersek brk_query;
        brk_query(m_server->DBConnection(),player->ID(),player->get_berserk_count());

        DB::PLAYER::State st_query;
        st_query(m_server->DBConnection(),player->ID(),false);

        DB::PLAYER::Buffs buff_query;
        std::map<uint32_t,Buff> buff_list = player->get_buff_list();

        for (std::map<uint32_t,Buff>::const_iterator it = buff_list.begin(); it != buff_list.end(); ++it)
        {
            if (it->second.persistent)
            {
                uint32_t time = world->GetBackgroundTaskTime(it->second.taskID);

                buff_query(m_server->DBConnection(),player->ID(),it->first,time);
            }

            world->TerminateBackgroundTask(it->second.taskID);
        }
    }
}
