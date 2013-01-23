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

#include "packet_guild.hpp"
#include "packet_base_item.hpp"
#include "opcodes_shard_server.hpp"

#include <packet.hpp>

/// SRLF INCLUDES
#include <storage.hpp>
#include <guild.hpp>
#include <guild_member.hpp>

/// GLOBAL INCLUDES
#include <algorithm>
#include <boost/bind.hpp>

namespace srv_pkt
{

void WriteGuildMember (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<GuildMember> &member)
{
    pkt->Write<uint32_t>(member->ID);
    pkt->Write(member->Name);

    if (member->Master)
        pkt->Write<uint8_t>(0);
    else
        pkt->Write<uint8_t>(0x0A);

    pkt->Write<uint8_t>(member->Level);
    pkt->Write<uint32_t>(member->GP);

    if (member->Master)
        pkt->Write<uint32_t>(-1);
    else
        pkt->Write<uint32_t>(0);

    pkt->Write<uint32_t>(member->CombatPoints);
    pkt->Write<uint32_t>(0);
    pkt->Write<uint32_t>(0);
    pkt->Write(member->GrantName);
    pkt->Write<uint32_t>(member->refCharID);
    pkt->Write<uint8_t>(0/*member->Position*/);
    pkt->Write<uint16_t>(member->Zone); /// LAST ZONE
    pkt->Write<uint32_t>(0x0264414A);    /// time (LAST LOG) * BINARY
    pkt->Write<uint32_t>(0x0264214A);    /// time (LAST LEVEL) * BINARY

    if (!member->Status)
        pkt->Write<uint8_t>(0x00);
    else
        pkt->Write<uint8_t>(0x01);
}

void WriteGuild (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Guild> &guild)
{
    pkt->Write<uint32_t>(guild->get_id());
    pkt->Write(guild->get_name());
    pkt->Write<uint8_t>(guild->get_level());
    pkt->Write<uint32_t>(guild->get_gp());
    pkt->Write(guild->get_notice_title());
    pkt->Write(guild->get_notice_msg());
    pkt->Write<uint32_t>(guild->get_emblem_index());
    pkt->Write<uint8_t>(0);
    pkt->Write<uint8_t>(guild->size());

    for (Guild::const_iterator i = guild->begin(); i != guild->end(); ++i)
        WriteGuildMember(pkt,i->second);

    pkt->Write<uint8_t>(0);
}

void GuildCreate (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Guild> &guild)
{
    pkt->WriteOpcode(SERVER_GUILD_CREATE);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);

    WriteGuild(pkt,guild);
}

void GuildBegin (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_GUILD_INFO_BEGIN);
}

void GuildInfo (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Guild> &guild)
{
    pkt->WriteOpcode(SERVER_GUILD_INFO);
    WriteGuild(pkt,guild);
}

void GuildJoin (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Guild> &guild)
{
    pkt->WriteOpcode(SERVER_GUILD_JOIN);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
    WriteGuild(pkt,guild);
}

void GuildEnd(const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_GUILD_INFO_END);
}

void GuildNotice (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_GUILD_NOTICE);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
}

void GuildDisband (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_GUILD_DISBAND);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
}

void GuildDonateGP (const boost::shared_ptr<OPacket> &pkt, uint32_t donated_sp)
{
    pkt->WriteOpcode(SERVER_GUILD_DONATE_GP);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
    pkt->Write<uint32_t>(donated_sp);
}

void GuildUpdate (const boost::shared_ptr<OPacket> &pkt, const GUILD_NOTIFY_TYPE action)
{
    pkt->WriteOpcode(SERVER_GUILD_NOTIFY);
    pkt->Write<uint8_t>(action);
}

void GuildNewMember (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<GuildMember> &member)
{
    pkt->WriteOpcode(SERVER_GUILD_NOTIFY);
    pkt->Write<uint8_t>(GUILD_NOTIFY_MEMBER_JOIN);
    WriteGuildMember(pkt,member);
}

void GuildUpdate (const boost::shared_ptr<OPacket> &pkt, uint32_t ID, const GUILD_MEMBER_LEAVE_REASON reason)
{
    pkt->WriteOpcode(SERVER_GUILD_NOTIFY);
    pkt->Write<uint8_t>(GUILD_NOTIFY_MEMBER_LEAVE);
    pkt->Write<uint32_t>(ID);
    pkt->Write<uint8_t>(reason);
}

void GuildUpdate (const boost::shared_ptr<OPacket> &pkt, const uint32_t GP)
{
    pkt->WriteOpcode(SERVER_GUILD_NOTIFY);
    pkt->Write<uint8_t>(GUILD_NOTIFY_GUILD);
    pkt->Write<uint8_t>(GUILD_NOTIFY_GUILD_GP);
    pkt->Write<uint32_t>(GP);
}

void GuildUpdate (const boost::shared_ptr<OPacket> &pkt, const uint8_t level, const uint32_t GP)
{
    pkt->WriteOpcode(SERVER_GUILD_NOTIFY);
    pkt->Write<uint8_t>(GUILD_NOTIFY_GUILD);
    pkt->Write<uint8_t>(GUILD_NOTIFY_GUILD_PROMOTE);
    pkt->Write<uint8_t>(level);
    pkt->Write<uint32_t>(GP);
}

void GuildUpdate (const boost::shared_ptr<OPacket> &pkt, const std::string& title, const std::string& msg)
{
    pkt->WriteOpcode(SERVER_GUILD_NOTIFY);
    pkt->Write<uint8_t>(GUILD_NOTIFY_GUILD);
    pkt->Write<uint8_t>(GUILD_NOTIFY_GUILD_NOTICE);
    pkt->Write(title);
    pkt->Write(msg);
}

void GuildNotify_MemberLevel (const boost::shared_ptr<OPacket> &pkt, const uint32_t memberID, const uint8_t level)
{
    pkt->WriteOpcode(SERVER_GUILD_NOTIFY);
    pkt->Write<uint8_t>(GUILD_NOTIFY_MEMBER);
    pkt->Write<uint32_t>(memberID);
    pkt->Write<uint8_t>(GUILD_NOTIFY_MEMBER_LEVEL);
    pkt->Write<uint8_t>(level);
}

void GuildMemberState (const boost::shared_ptr<OPacket> &pkt, const uint32_t memberID, const GUILD_MEMBER_STATE state)
{
    pkt->WriteOpcode(SERVER_GUILD_NOTIFY);
    pkt->Write<uint8_t>(GUILD_NOTIFY_MEMBER);
    pkt->Write<uint32_t>(memberID);
    pkt->Write<uint8_t>(GUILD_NOTIFY_MEMBER_STATE);
    pkt->Write<uint8_t>(state);
}

void GuildUpdate (const boost::shared_ptr<OPacket> &pkt, const uint32_t memberID, const uint32_t donatedGP,
    const uint32_t actualGP)
{
    pkt->WriteOpcode(SERVER_GUILD_NOTIFY);
    pkt->Write<uint8_t>(GUILD_NOTIFY_MEMBER);
    pkt->Write<uint32_t>(memberID);
    pkt->Write<uint8_t>(GUILD_NOTIFY_MEMBER_GP);
    pkt->Write<uint32_t>(donatedGP+actualGP);
}

void GuildUpdate (const boost::shared_ptr<OPacket> &pkt, const uint32_t memberID, const uint8_t position)
{
    pkt->WriteOpcode(SERVER_GUILD_NOTIFY);
    pkt->Write<uint8_t>(GUILD_NOTIFY_MEMBER);
    pkt->Write<uint32_t>(memberID);
    pkt->Write<uint8_t>(GUILD_NOTIFY_MEMBER_POSITION);
    pkt->Write<uint8_t>(position);
}

void GuildNotify_MemberZone (const boost::shared_ptr<OPacket> &pkt, const uint32_t memberID, const int16_t zone)
{
    pkt->WriteOpcode(SERVER_GUILD_NOTIFY);
    pkt->Write<uint8_t>(GUILD_NOTIFY_MEMBER);
    pkt->Write<uint32_t>(memberID);
    pkt->Write<uint8_t>(GUILD_NOTIFY_MEMBER_ZONE);
    pkt->Write<int16_t>(zone);
}

void GuildNotify_UnionJoin (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Guild> &guild)
{
    pkt->WriteOpcode(SERVER_GUILD_NOTIFY);
    pkt->Write<uint8_t>(GUILD_NOTIFY_UNION_MEMBER);
    pkt->Write<uint8_t>(GUILD_NOTIFY_UNION_JOIN);
    pkt->Write<uint32_t>(guild->get_id());

    if (guild)
    {
        pkt->Write(guild->get_name());
        pkt->Write<uint8_t>(guild->get_level());

        boost::shared_ptr<GuildMember> member = guild->get_master();

        if (member)
        {
            pkt->Write(member->Name);
            pkt->Write<uint32_t>(member->refCharID);
            pkt->Write<uint8_t>(guild->size());
        }
    }
}

void GuildNotify_UnionMemberMax (const boost::shared_ptr<OPacket> &pkt, const uint32_t guildID, const uint8_t max)
{
    pkt->WriteOpcode(SERVER_GUILD_NOTIFY);
    pkt->Write<uint8_t>(GUILD_NOTIFY_UNION_MEMBER);
    pkt->Write<uint8_t>(GUILD_NOTIFY_UNION_MEMBER_MAX);
    pkt->Write<uint32_t>(guildID);
    pkt->Write<uint8_t>(max);
}

void GuildNotify_UnionMemberLeader (const boost::shared_ptr<OPacket> &pkt, const uint32_t guildID,
    const std::string &name, const uint32_t refCharID)
{
    pkt->WriteOpcode(SERVER_GUILD_NOTIFY);
    pkt->Write<uint8_t>(GUILD_NOTIFY_UNION_MEMBER);
    pkt->Write<uint8_t>(GUILD_NOTIFY_UNION_MEMBER_LEADER);
    pkt->Write<uint32_t>(guildID);
    pkt->Write(name);
    pkt->Write<uint32_t>(refCharID);
}

void GuildNotify_UnionMemberCount (const boost::shared_ptr<OPacket> &pkt, const uint32_t guildID, const uint8_t count)
{
    pkt->WriteOpcode(SERVER_GUILD_NOTIFY);
    pkt->Write<uint8_t>(GUILD_NOTIFY_UNION_MEMBER);
    pkt->Write<uint8_t>(GUILD_NOTIFY_UNION_MEMBER_COUNT);
    pkt->Write<uint32_t>(guildID);
    pkt->Write<uint8_t>(count);
}

void GuildNotify_UnionRemove (const boost::shared_ptr<OPacket> &pkt, const uint32_t guildID, const uint8_t reason)
{
    pkt->WriteOpcode(SERVER_GUILD_NOTIFY);
    pkt->Write<uint8_t>(GUILD_NOTIFY_UNION_MEMBER_LEAVE);
    pkt->Write<uint8_t>(reason);
    pkt->Write<uint32_t>(guildID);
}

void GuildNotify_UnionClear (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_GUILD_NOTIFY);
    pkt->Write<uint8_t>(GUILD_NOTIFY_UNION_MEMBER_LEAVE);
    pkt->Write<uint8_t>(GUILD_NOTIFY_UNION_MEMBER_REASON_DISBAND);
}

void GuildNotify_Authority_V2 (const boost::shared_ptr<OPacket> &pkt, const uint8_t count,
    const boost::shared_array<uint32_t> &memberIDs, const boost::shared_array<uint32_t> &authority)
{
    pkt->WriteOpcode(SERVER_GUILD_NOTIFY);
    pkt->Write<uint8_t>(0x14);
    pkt->Write<uint8_t>(count);

    for (int i = 0; i < count; ++i)
    {
        pkt->Write<uint32_t>(memberIDs[i]);
        pkt->Write<uint32_t>(authority[i]);
    }
}

void GuildNotify_Authority_V1 (const boost::shared_ptr<OPacket> &pkt, const uint8_t count,
    const boost::shared_array<uint32_t> &memberIDs, const boost::shared_array<uint32_t> &authority)
{
    pkt->WriteOpcode(SERVER_GUILD_NOTIFY);
    pkt->Write<uint8_t>(GUILD_NOTIFY_MEMBER_AUTHORITY);
    pkt->Write<uint8_t>(count);
    pkt->Write<uint8_t>(GUILD_NOTIFY_MEMBER_AUTHORITY_SETTING);

    for (int i = 0; i < count; ++i)
    {
        pkt->Write<uint32_t>(memberIDs[i]);
        pkt->Write<uint32_t>(authority[i]);
    }
}

void GuildUpdate (const boost::shared_ptr<OPacket> &pkt, const uint32_t masterID, const uint32_t memberID)
{
    pkt->WriteOpcode(SERVER_GUILD_NOTIFY);
    pkt->Write<uint8_t>(GUILD_NOTIFY_MEMBER_AUTHORITY);
    pkt->Write<uint8_t>(0x02);
    pkt->Write<uint8_t>(GUILD_NOTIFY_MEMBER_AUTHORITY_LEADER_TRANSFER);
    pkt->Write<uint32_t>(masterID);
    pkt->Write<uint8_t>(0x00);
    pkt->Write<uint32_t>(-1);
    pkt->Write<uint8_t>(0x01);
    pkt->Write<uint32_t>(memberID);
    pkt->Write<uint8_t>(0x0A);
    pkt->Write<uint32_t>(0);
    pkt->Write<uint8_t>(0);
}

void GuildKick (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_GUILD_KICK);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
}

void GuildGrant (const boost::shared_ptr<OPacket> &pkt, const uint32_t guildID, const uint32_t memberID,
    const std::string &grant)
{
    pkt->WriteOpcode(SERVER_GUILD_GRANT);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
    pkt->Write<uint32_t>(guildID);
    pkt->Write<uint32_t>(memberID);
    pkt->Write(grant);
}

void GuildPlayer (const boost::shared_ptr<OPacket> &pkt, uint32_t playerID, const std::string &guild,
    const boost::shared_ptr<GuildMember> &member, bool spawning)
{
    pkt->WriteOpcode(SERVER_PLAYER_GUILD);
    pkt->Write<uint32_t>(playerID);
    pkt->Write<uint32_t>(member->GuildID);
    pkt->Write(guild);
    pkt->Write(member->GrantName);
    pkt->Write<uint32_t>(0);
    pkt->Write<uint32_t>(0);
    pkt->Write<uint32_t>(0);
    pkt->Write<uint8_t>(0);
    pkt->Write<uint8_t>(1);
}

void PlayerGrantName (const boost::shared_ptr<OPacket> &pkt, const uint32_t guildID, const std::string &Name,
    const std::string &GrantName)
{
    pkt->WriteOpcode(SERVER_PLAYER_GUILD_GRANTNAME);
    pkt->Write<uint32_t>(guildID);
    pkt->Write(Name);
    pkt->Write(GrantName);
}

void GuildPromote (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_GUILD_PROMOTE);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
}

void GuildPromote (const boost::shared_ptr<OPacket> &pkt, const GUILD_ERROR_TYPE error)
{
    pkt->WriteOpcode(SERVER_GUILD_PROMOTE);
    pkt->Write<uint8_t>(ANSWER_ERROR);
    pkt->Write<uint8_t>(error);
    pkt->Write<uint8_t>(0x4C);
}

void GuildWarInvite (const boost::shared_ptr<OPacket> &pkt, const GUILD_ERROR_TYPE error)
{
    pkt->WriteOpcode(SERVER_GUILD_WAR_INVITE);
    pkt->Write<uint8_t>(ANSWER_ERROR);
    pkt->Write<uint8_t>(error);
    pkt->Write<uint8_t>(0x4C);
}

void GuildLeave (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_GUILD_LEAVE);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
}

void GuildClear (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID)
{
    pkt->WriteOpcode(SERVER_GUILD_CLEAR);
    pkt->Write<uint32_t>(playerID);
}

void GuildTransferLeadership (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_GUILD_TRANSFER_LEADERSHIP,ANSWER_ACCEPT);
}

void GuildStorageOpen (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_GUILD_STORAGE_OPEN,ANSWER_ACCEPT);
}

void GuildStorageOpen (const boost::shared_ptr<OPacket> &pkt, const GUILD_ERROR_TYPE error)
{
    pkt->WriteOpcode(SERVER_GUILD_STORAGE_OPEN,ANSWER_ERROR,error);
    pkt->Write<uint8_t>(0x4C);
}

void GuildStorageClose (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_GUILD_STORAGE_CLOSE,ANSWER_ACCEPT);
}

void GuildStorageGold (const boost::shared_ptr<OPacket> &pkt, const uint64_t amount)
{
    pkt->WriteOpcode(SERVER_GUILD_STORAGE_GOLD);
    pkt->Write<uint64_t>(amount);
}

void GuildStorage (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Storage> &storage)
{
    pkt->WriteOpcode(SERVER_GUILD_STORAGE_ITEM);
    pkt->Write<uint8_t>(storage->GetCapacity());
    pkt->Write<uint8_t>(storage->size());

    Storage::const_iterator iter;
    for ( iter = storage->begin(); iter != storage->end(); ++iter)
        WriteItem(pkt,iter->second);
}

void GuildStorageEnd (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_GUILD_STORAGE_END);
}

void GuildInviteDeny (const boost::shared_ptr<OPacket> &pkt, const GUILD_INVITE_ANSWER answer)
{
    pkt->WriteOpcode(SERVER_GUILD_INVITE_DENY);
    pkt->Write<uint16_t>(answer);
}

void GuildInviteDenied (const boost::shared_ptr<OPacket> &pkt, const GUILD_INVITE_ANSWER answer)
{
    pkt->WriteOpcode(SERVER_GUILD_INVITE_DENIED);
    pkt->Write<uint16_t>(answer);

    if (answer == INVITE_GUILD_REJECT)
        pkt->Write<uint8_t>(0x4C);
}

void GuildAuthority (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_GUILD_AUTHORITY,ANSWER_ACCEPT);
}

void GuildPosition (const boost::shared_ptr<OPacket> &pkt, const uint32_t guildID, const uint32_t memberID,
    const uint8_t position)
{
    pkt->WriteOpcode(SERVER_GUILD_POSITION,ANSWER_ACCEPT);
    pkt->Write<uint32_t>(guildID);
    pkt->Write<uint32_t>(memberID);
    pkt->Write<uint8_t>(position);
}

}
