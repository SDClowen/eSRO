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

#ifndef __SLES_GAME_SERVER_PARTY_PACKETS_HPP__
#define __SLES_GAME_SERVER_PARTY_PACKETS_HPP__

#include <party.hpp>
#include <vector>
#include <boost/shared_ptr.hpp>

class OPacket;

namespace srv_pkt
{

enum PARTY_ANSWER_TYPE
{
    PARTY_ANSWER_DENY,
    PARTY_ANSWER_ACCEPT,
    PARTY_ANSWER_IGNORE
};

enum PARTY_ERROR
{
    PARTY_ERROR_PLAYER_BUSY = 0x06,
    PARTY_ERROR_MEMBER_FULL = 0x08,
    PARTY_ERROR_INSUFFICIENT_LEVEL = 0x0A,
    PARTY_ERROR_INVALID_APPLICANT = 0x0E,
    PARTY_ERROR_INVALID_TARGET = 0x0F,
    PARTY_ERROR_REJECTED = 0x10,
    PARTY_ERROR_LEADER_DISMISSED = 0x11,
    PARTY_ERROR_REQUEST_DENY = 0x17,
    PARTY_ERROR_TARGET_IN_PARTY = 0x18,
    PARTY_ERROR_INVALID_PARTY = 0x1C,
    PARTY_ERROR_INVALID_LEVEL = 0x1E,
    PARTY_ERROR_PENDING_INVITATION = 0x1F,
    PARTY_ERROR_OPPOSING_ROLES = 0x23
};

enum PARTY_REASON_TYPE
{
    PT_DISCONNECTED = 0x01,
    PT_LEFT = 0x02,
    PT_KICK = 0x04
};

enum PARTY_NOTIFY
{
    PARTY_NOTIFY_DISMISS = 1,
    PARTY_NOTIFY_MEMBER_JOIN,
    PARTY_NOTIFY_MEMBER_STATE,
    PARTY_NOTIFY_MEMBER_INFO = 6,
    PARTY_NOTIFY_LEADER = 9
};

enum PARTY_NOTIFY_MEMBER
{
    PARTY_NOTIFY_MEMBER_LEVEL = 0x02,
    PARTY_NOTIFY_MEMBER_HEALTH = 0x04,
    PARTY_NOTIFY_MEMBER_MASTERY = 0x08,
    PARTY_NOTIFY_MEMBER_POSITION = 0x20,
    PARTY_NOTIFY_MEMBER_GUILD = 0x40
};

void PartyInvitation (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const uint8_t type);

void PartyForm (const boost::shared_ptr<OPacket> &pkt, const PartyEntry& entry, const uint32_t party_id);

/**
 *
 *  @brief Send descriptive information about a party listed in the party match.
 *  @pkt: Packet to where write the information.
 *  @party: Party which data is gonna be displayed.
 *
 **/

void PartyInformation (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Party> &party);

/**
 *
 *	brief Delete party from party match.
 *
 **/

void PartyDelete (const boost::shared_ptr<OPacket> &pkt, const uint32_t partyID);

/**
 *
 *	brief Change party entry in the party match.
 *
 **/

void PartyChange (const boost::shared_ptr<OPacket> &pkt, const PartyEntry& entry, const uint32_t partyID);

void PartyList (const boost::shared_ptr<OPacket> &pkt, const uint8_t max_page, const uint8_t current_page,
    const std::vector<boost::shared_ptr<Party> > &party_list);

void PartyRequest (const boost::shared_ptr<OPacket> &pkt, const uint32_t sessionID, const uint32_t party_number,
    const boost::shared_ptr<Player> &player);

void PartyInvite (const boost::shared_ptr<OPacket> &pkt, const PARTY_ERROR error);

void PartyAnswer (const boost::shared_ptr<OPacket> &pkt, const PARTY_ANSWER_TYPE type);

void PartyAnswer (const boost::shared_ptr<OPacket> &pkt, const PARTY_ERROR error);

void PartyAccepted (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID);

void PartyRejected (const boost::shared_ptr<OPacket> &pkt);

void PartyInfo (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Party> &party);

void PartyUpdate (const boost::shared_ptr<OPacket> &pkt);

void PartyUpdate (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Player> &player);

void PartyUpdate (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID);

/**
 *
 *	@brief Player left party.
 *
 **/

void PartyUpdate (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const PARTY_REASON_TYPE type);

/**
 *
 *	@brief Player level.
 *
 **/

void PartyUpdate (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const uint8_t level);

/**
 *
 *	@brief Player Health.
 *
 **/

void PartyUpdate (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const uint32_t HP,
    const uint32_t maxHP, const uint32_t MP, const uint32_t maxMP);

/**
 *
 *	@brief Player Mastery.
 *
 **/

void PartyUpdate (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const uint32_t fMast,
    const uint32_t sMast);

/**
 *
 *	@brief Player position.
 *
 **/

void PartyUpdate (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const Coord &position);

/**
 *
 *	@brief Player Guild changed.
 *
 **/

void PartyUpdate (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const std::string &guild);

}

#endif //__SLES_GAME_SERVER_PARTY_PACKETS_HPP__
