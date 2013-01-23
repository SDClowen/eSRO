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

#include "packet_alliance.hpp"
#include "opcodes_shard_server.hpp"

#include <packet.hpp>

/// SRLF INCLUDES
#include <union.hpp>
#include <guild.hpp>
#include <guild_member.hpp>

namespace srv_pkt
{

void WriteGuildEntry (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Guild> &guild)
{
    pkt->Write<uint32_t>(guild->get_id());
    pkt->Write(guild->get_name());
    pkt->Write<uint8_t>(guild->get_level());

    boost::shared_ptr<GuildMember> member = guild->get_master();

    pkt->Write(member->Name);
    pkt->Write<uint32_t>(member->refCharID);
    pkt->Write<uint8_t>(guild->size());
}

void AllianceList (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Union> &ally)
{
    pkt->WriteOpcode(SERVER_ALLIANCE_LIST);
    pkt->Write<uint32_t>(ally->id());
    pkt->Write<uint32_t>(ally->emblem_index());
    pkt->Write<uint32_t>(ally->head_guild_id());

    std::vector<boost::shared_ptr<Guild> > guilds = ally->guild_list();

    pkt->Write<uint8_t>(guilds.size());
    for (Union::const_iterator it = guilds.begin(); it != guilds.end(); ++it)
        WriteGuildEntry(pkt,*it);
}

void AllianceInvite (const boost::shared_ptr<OPacket> &pkt, const ALLIANCE_ERROR error)
{
    pkt->WriteOpcode(SERVER_GUILD_UNION_INVITE);
    pkt->Write<uint8_t>(ANSWER_ERROR);
    pkt->Write<uint8_t>(error);
    pkt->Write<uint8_t>(0x4C);
}

void AllianceExpel (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_ALLIANCE_EXPEL);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
}

void AllianceSecede (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_ALLIANCE_SECEDE);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
}

}
