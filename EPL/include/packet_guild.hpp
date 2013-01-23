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

#ifndef __SLES_GAME_SERVER_GUILD_PACKET_HPP__
#define __SLES_GAME_SERVER_GUILD_PACKET_HPP__

#include "guild_error.hpp"
#include "guild_notify.hpp"

/// SOL INCLUDES
#include <guild_constants.hpp>

/// GLOBAL INCLUDES
#include <stdint.h>
#include <string>
#include <boost/shared_array.hpp>

class OPacket;
class Storage;
class Guild;
class GuildMember;

namespace srv_pkt
{

enum GUILD_INVITE_ANSWER
{
    INVITE_GUILD_REJECT = 0x1602,
    INVITE_GUILD_ACCEPT = 0x0101
};

void GuildCreate (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Guild> &guild);

void GuildBegin (const boost::shared_ptr<OPacket> &pkt);

void GuildInfo (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Guild> &guild);

void GuildJoin (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Guild> &guild);

void GuildEnd (const boost::shared_ptr<OPacket> &pkt);

void GuildNotice (const boost::shared_ptr<OPacket> &pkt);

void GuildDisband (const boost::shared_ptr<OPacket> &pkt);

void GuildDonateGP (const boost::shared_ptr<OPacket> &pkt, uint32_t donated_sp);

/**
 *
 *  @brief Construct leave alliance notification packet.
 *
 **/

void GuildUpdate (const boost::shared_ptr<OPacket> &pkt);

void GuildUpdate (const boost::shared_ptr<OPacket> &pkt, const GUILD_NOTIFY_TYPE action);

void GuildUpdate (const boost::shared_ptr<OPacket> &pkt, uint32_t ID, const GUILD_MEMBER_LEAVE_REASON reason);

/**
 *
 *  @brief Construct guild point notify packet.
 *
 **/

void GuildUpdate (const boost::shared_ptr<OPacket> &pkt, const uint32_t gp);

void GuildUpdate (const boost::shared_ptr<OPacket> &pkt, const uint32_t memberID, const uint32_t donated_gp,
    const uint32_t memberGP);

void GuildUpdate (const boost::shared_ptr<OPacket> &pkt, const std::string& title,  const std::string& notice);

void GuildUpdate (const boost::shared_ptr<OPacket> &pkt, const uint8_t lv, const uint32_t gp);

void GuildUpdate (const boost::shared_ptr<OPacket> &pk, const uint32_t masterID, const uint32_t memberID);

void GuildUpdate (const boost::shared_ptr<OPacket> &pkt, const uint32_t memberID, const uint8_t position);

/**
 *
 *  Notify member changing area.
 *
 **/

void GuildNotify_MemberZone (const boost::shared_ptr<OPacket> &pkt, const uint32_t memberID, const int16_t zone);

void GuildMemberState (const boost::shared_ptr<OPacket> &pkt, const uint32_t memberID,
    const GUILD_MEMBER_STATE state);

void GuildNotify_MemberLevel (const boost::shared_ptr<OPacket> &pkt, const uint32_t memberID, const uint8_t level);

void GuildNotify_Authority_V1 (const boost::shared_ptr<OPacket> &pkt, const uint8_t count,
    const boost::shared_array<uint32_t> &memberIDs, const boost::shared_array<uint32_t> &authority);

void GuildNotify_Authority_V2 (const boost::shared_ptr<OPacket> &pkt, const uint8_t count,
    const boost::shared_array<uint32_t> &memberIDs, const boost::shared_array<uint32_t> &authority);

void GuildNotify_UnionJoin (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Guild> &guild);

void GuildNotify_UnionMemberMax (const boost::shared_ptr<OPacket> &pkt, const uint32_t guildID, const uint8_t max);

void GuildNotify_UnionMemberLeader (const boost::shared_ptr<OPacket> &pkt, const uint32_t guildID,
    const std::string &name, const uint32_t refCharID);

void GuildNotify_UnionMemberCount (const boost::shared_ptr<OPacket> &pkt, const uint32_t guildID, const uint8_t count);

/**
 *
 *  @brief Construct guild left alliance notification packet.
 *  @guildID: Guild ID.
 *  @reason: the reason for the guild to leave union (SECEDE OR EXPEL).
 *
 **/

void GuildNotify_UnionRemove (const boost::shared_ptr<OPacket> &pkt, const uint32_t guildID, const uint8_t reason);

void GuildNotify_UnionClear (const boost::shared_ptr<OPacket> &pkt);

void GuildKick (const boost::shared_ptr<OPacket> &pkt);

void GuildNewMember (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<GuildMember> &member);

void GuildGrant (const boost::shared_ptr<OPacket> &pkt, const uint32_t guildID, const uint32_t memberID,
    const std::string &grant);

void GuildPlayer (const boost::shared_ptr<OPacket> &pkt, uint32_t playerID, const std::string &guild,
    const boost::shared_ptr<GuildMember> &member, bool spawning);

void PlayerGrantName (const boost::shared_ptr<OPacket> &pkt, const uint32_t guildID, const std::string &name,
    const std::string &grant);

void GuildPromote (const boost::shared_ptr<OPacket> &pkt);

void GuildPromote (const boost::shared_ptr<OPacket> &pkt, const GUILD_ERROR_TYPE error);

void GuildWarInvite (const boost::shared_ptr<OPacket> &pkt, const GUILD_ERROR_TYPE error);

void GuildLeave (const boost::shared_ptr<OPacket> &pkt);

void GuildClear (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID);

void GuildTransferLeadership (const boost::shared_ptr<OPacket> &pkt);

void GuildStorageOpen (const boost::shared_ptr<OPacket> &pkt);

void GuildStorageOpen (const boost::shared_ptr<OPacket> &pkt, const GUILD_ERROR_TYPE error);

void GuildStorageClose (const boost::shared_ptr<OPacket> &pkt);

void GuildStorageGold (const boost::shared_ptr<OPacket> &pkt, const uint64_t amount);

void GuildStorage (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Storage> &storage);

void GuildStorageEnd (const boost::shared_ptr<OPacket> &pkt);

void GuildInviteDeny (const boost::shared_ptr<OPacket> &pkt, const GUILD_INVITE_ANSWER answer);

void GuildInviteDenied (const boost::shared_ptr<OPacket> &pkt, const GUILD_INVITE_ANSWER answer);

void GuildAuthority (const boost::shared_ptr<OPacket> &pkt);

void GuildPosition (const boost::shared_ptr<OPacket> &pkt, const uint32_t guildID, const uint32_t memberID,
    const uint8_t position);

}

#endif //__SLES_GAME_SERVER_GUILD_PACKET_HPP__
