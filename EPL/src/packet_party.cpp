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

#include "packet_party.hpp"
#include "opcodes_shard_server.hpp"

#include <packet.hpp>

#include <player.hpp>
#include <guild.hpp>

#include <algorithm>
#include <boost/bind.hpp>

namespace srv_pkt
{

void WriteMember (const boost::shared_ptr<OPacket> &packet, const boost::shared_ptr<Player> &player)
{
    packet->Write<uint8_t>(0xFF);
    packet->Write<uint32_t>(player->GetUniqueID());
    packet->Write(player->get_name());
    packet->Write<uint32_t>(player->get_model());
    packet->Write<uint8_t>(player->level());

    uint8_t hp = (player->hp()*10)/player->max_hp();
    uint8_t mp = (player->mp()*10)/player->max_mp();

    packet->Write<uint8_t>(((mp << 4) | hp));

    Coord pos = player->get_position();

    packet->Write<uint16_t>(pos.get_zone());
    packet->Write<uint16_t>(pos.get_x());
    packet->Write<uint16_t>(pos.get_z());
    packet->Write<uint16_t>(pos.get_y());
    packet->Write<uint16_t>(0x0001);
    packet->Write<uint16_t>(0x0001);

    boost::shared_ptr<Guild> guild = player->get_guild();

    std::string name;

    if (guild)
        name = guild->get_name();

    packet->Write(name);
    packet->Write<uint8_t>(0x04);

    std::pair<Mastery,Mastery> masts = player->get_mastery_tree().GetHighestMastery();

    packet->Write<uint32_t>(masts.first.ID());
    packet->Write<uint32_t>(masts.second.ID());
}

void WriteEntry (const boost::shared_ptr<OPacket> &packet, const PartyEntry &entry)
{
    packet->Write<uint8_t>(entry.type());
    packet->Write<uint8_t>(entry.purpose());
    packet->Write<uint8_t>(entry.min_level());
    packet->Write<uint8_t>(entry.max_level());
    packet->Write(entry.description());
}

void WriteParty (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Party> &party)
{
    boost::shared_ptr<Player> master = party->get_master();

    pkt->Write<uint32_t>(party->get_number());
    pkt->Write<uint32_t>(master->GetUniqueID());
    pkt->Write(master->get_name());
    pkt->Write<uint8_t>(master->race());
    pkt->Write<uint8_t>(party->get_member_count());

    WriteEntry(pkt,party->get_entry());
}

void PartyInvitation (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const uint8_t type)
{
    pkt->WriteOpcode(SERVER_SEND_INVITATION);
    pkt->Write<uint8_t>(INVITATION_PARTY);
    pkt->Write<uint32_t>(playerID);
    pkt->Write<uint8_t>(type);
}

void PartyForm (const boost::shared_ptr<OPacket> &pkt, const PartyEntry& entry, const uint32_t partyID)
{
    pkt->WriteOpcode(SERVER_PARTY_FORM,ANSWER_ACCEPT);
    pkt->Write<uint32_t>(entry.number());
    pkt->Write<uint32_t>(partyID);
    pkt->Write<uint8_t>(entry.type());
    pkt->Write<uint8_t>(entry.purpose());
    pkt->Write<uint8_t>(entry.min_level());
    pkt->Write<uint8_t>(entry.max_level());
    pkt->Write(entry.description());
}

void PartyInformation (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Party> &party)
{
    pkt->WriteOpcode(SERVER_PARTY_INFORMATION);
    pkt->Write<uint8_t>(0x01);  /// PARTY MODE
    pkt->Write<uint8_t>(party->get_member_count());

    std::for_each(party->begin(),party->end(),boost::bind(WriteMember,pkt,_1));
}

void PartyDelete (const boost::shared_ptr<OPacket> &pkt, const uint32_t partyID)
{
    pkt->WriteOpcode(SERVER_PARTY_DELETE,ANSWER_ACCEPT);
    pkt->Write<uint32_t>(partyID);
}

void PartyChange (const boost::shared_ptr<OPacket> &pkt, const PartyEntry& entry, const uint32_t partyID)
{
    pkt->WriteOpcode(SERVER_PARTY_CHANGE,ANSWER_ACCEPT);
    pkt->Write<uint32_t>(entry.number());
    pkt->Write<uint32_t>(partyID);
    pkt->Write<uint8_t>(entry.type());
    pkt->Write<uint8_t>(entry.purpose());
    pkt->Write<uint8_t>(entry.min_level());
    pkt->Write<uint8_t>(entry.max_level());
    pkt->Write(entry.description());
}

void PartyList (const boost::shared_ptr<OPacket> &pkt, const uint8_t max_pages, const uint8_t current_page,
    const std::vector<boost::shared_ptr<Party> > &party_list)
{
    pkt->WriteOpcode(SERVER_PARTY_LIST,ANSWER_ACCEPT);
    pkt->Write<uint8_t>(max_pages);
    pkt->Write<uint8_t>(current_page);
    pkt->Write<uint8_t>(party_list.size());

    std::for_each(party_list.begin(),party_list.end(), boost::bind(WriteParty,pkt,_1));
}

void PartyRequest (const boost::shared_ptr<OPacket> &pkt, const uint32_t sessionID, const uint32_t party_number,
    const boost::shared_ptr<Player> &player)
{
    pkt->WriteOpcode(SERVER_PARTY_REQUEST);
    pkt->Write<uint32_t>(sessionID);
    pkt->Write<uint32_t>(player->GetUniqueID());
    pkt->Write<uint32_t>(party_number);

    std::pair<Mastery,Mastery> masts = player->get_mastery_tree().GetHighestMastery();

    pkt->Write<uint32_t>(masts.first.ID());
    pkt->Write<uint32_t>(masts.second.ID());
    pkt->Write<uint8_t>(0x04);

    WriteMember(pkt,player);
}

void PartyInvite (const boost::shared_ptr<OPacket> &pkt, const PARTY_ERROR error)
{
    pkt->WriteOpcode(SERVER_PARTY_INVITE,ANSWER_ERROR,error);
    pkt->Write<uint8_t>(0x2C);
}

void PartyAnswer (const boost::shared_ptr<OPacket> &pkt, const PARTY_ANSWER_TYPE type)
{
    pkt->WriteOpcode(SERVER_PARTY_ANSWER,ANSWER_ACCEPT);
    pkt->Write<uint8_t>(type);
    pkt->Write<uint8_t>(0);
}

void PartyAnswer (const boost::shared_ptr<OPacket> &pkt, const PARTY_ERROR error)
{
    pkt->WriteOpcode(SERVER_PARTY_ANSWER);
    pkt->Write<uint8_t>(ANSWER_ERROR);
    pkt->Write<uint8_t>(error);
    pkt->Write<uint8_t>(0x2C);
}

void PartyAccepted (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID)
{
    pkt->WriteOpcode(SERVER_PARTY_INVITE_ANSWER,ANSWER_ACCEPT);
    pkt->Write<uint32_t>(playerID);
}

void PartyRejected (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_PARTY_INVITE_ANSWER,ANSWER_ERROR,PARTY_ERROR_REJECTED);
    pkt->Write<uint8_t>(0x2C);
}

void PartyInfo (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Party> &party)
{
    pkt->WriteOpcode(SERVER_PARTY_INFO);
    pkt->Write<uint8_t>(0xFF);
    pkt->Write<uint32_t>(party->get_master()->GetUniqueID());
    pkt->Write<uint8_t>(01); //TODO: TYPE!!
    pkt->Write<uint8_t>(party->get_member_count());

    std::for_each(party->begin(),party->end(),boost::bind(WriteMember,pkt,_1));
}

void PartyUpdate (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_PARTY_UPDATE);
    pkt->Write<uint8_t>(PARTY_NOTIFY_DISMISS);
    pkt->Write<uint16_t>(0x0B);
}

void PartyUpdate (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Player> &player)
{
    pkt->WriteOpcode(SERVER_PARTY_UPDATE);
    pkt->Write<uint8_t>(PARTY_NOTIFY_MEMBER_JOIN);
    WriteMember(pkt,player);
}

void PartyUpdate (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID)
{
    pkt->WriteOpcode(SERVER_PARTY_UPDATE);
    pkt->Write<uint8_t>(PARTY_NOTIFY_LEADER);
    pkt->Write<uint32_t>(playerID);
}

void PartyUpdate (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const PARTY_REASON_TYPE type)
{
    pkt->WriteOpcode(SERVER_PARTY_UPDATE);
    pkt->Write<uint8_t>(PARTY_NOTIFY_MEMBER_STATE);
    pkt->Write<uint32_t>(playerID);
    pkt->Write<uint8_t>(type);
}

void PartyUpdate (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const uint8_t level)
{
    pkt->WriteOpcode(SERVER_PARTY_UPDATE);
    pkt->Write<uint8_t>(PARTY_NOTIFY_MEMBER_INFO);
    pkt->Write<uint32_t>(playerID);
    pkt->Write<uint8_t>(PARTY_NOTIFY_MEMBER_LEVEL);
    pkt->Write<uint8_t>(level);
}

void PartyUpdate (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const uint32_t HP,
    const uint32_t maxHP, const uint32_t MP, const uint32_t maxMP)
{
    pkt->WriteOpcode(SERVER_PARTY_UPDATE);
    pkt->Write<uint8_t>(PARTY_NOTIFY_MEMBER_INFO);
    pkt->Write<uint32_t>(playerID);
    pkt->Write<uint8_t>(PARTY_NOTIFY_MEMBER_HEALTH);

    uint8_t hp = (HP*10)/maxHP;
    uint8_t mp = (MP*10)/maxMP;

    pkt->Write<uint8_t>(((mp << 4) | hp));
}

void PartyUpdate (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const uint32_t fMast,
    const uint32_t sMast)
{
    pkt->WriteOpcode(SERVER_PARTY_UPDATE);
    pkt->Write<uint8_t>(PARTY_NOTIFY_MEMBER_INFO);
    pkt->Write<uint32_t>(playerID);
    pkt->Write<uint8_t>(PARTY_NOTIFY_MEMBER_MASTERY);
    pkt->Write<uint32_t>(fMast);
    pkt->Write<uint32_t>(sMast);
}

void PartyUpdate (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const Coord &pos)
{
    pkt->WriteOpcode(SERVER_PARTY_UPDATE);
    pkt->Write<uint8_t>(PARTY_NOTIFY_MEMBER_INFO);
    pkt->Write<uint32_t>(playerID);
    pkt->Write<uint8_t>(PARTY_NOTIFY_MEMBER_POSITION);
    pkt->Write<uint16_t>(pos.get_zone());
    pkt->Write<uint16_t>(pos.get_x());
    pkt->Write<uint16_t>(pos.get_z());
    pkt->Write<uint16_t>(pos.get_y());
    pkt->Write<uint16_t>(0x0001);
    pkt->Write<uint16_t>(0x0001);
}

void PartyUpdate (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const std::string &guild)
{
    pkt->WriteOpcode(SERVER_PARTY_UPDATE);
    pkt->Write<uint8_t>(PARTY_NOTIFY_MEMBER_INFO);
    pkt->Write<uint32_t>(playerID);
    pkt->Write<uint8_t>(PARTY_NOTIFY_MEMBER_GUILD);
    pkt->Write(guild);
}

}
