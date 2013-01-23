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

#ifndef __SLES_PACKET_AUTHENTICATION_HPP__
#define __SLES_PACKET_AUTHENTICATION_HPP__

#include "packet.hpp"

namespace srv_pkt
{

enum HANDSHAKE_TYPE
{
	HANDSHAKE_CLEAN = 0x01,
	HANDSHAKE_EXCHANGE = 0x0E,
	HANDSHAKE_SIGNATURE = 0x10
};

/**
 *
 *  @brief: Construct the handshake packet with no encryption flag set up.
 *
 **/

void WriteHandshake (const boost::shared_ptr<OPacket> &pkt);

/**
 *
 *  @brief: Construct the handshake packet with initial seed values and data for Diffie-Hellman key exchange.
 *
 **/

void WriteHandshake (const boost::shared_ptr<OPacket> &pkt, const uint32_t public_key[2], const uint32_t count_seed,
    const uint32_t crc_seed, const uint32_t base_key[2], const uint32_t G, const uint32_t P, const uint32_t A);

/**
 *
 *  @brief: Construct the handshake packet with server signature for later client validation.
 *
 **/

void WriteHandshake (const boost::shared_ptr<OPacket> &pkt, const uint32_t signature[2]);

/**
 *
 *	@brief: Build server identification packet.
 *
 **/

void WriteWhoIs (const boost::shared_ptr<OPacket> &pkt, const std::string &ID);

void WriteVersionA (const boost::shared_ptr<OPacket> &pkt);

void WriteVersionB (const boost::shared_ptr<OPacket> &pkt);

void WriteVersionC (const boost::shared_ptr<OPacket> &pkt);

void WriteVersionD (const boost::shared_ptr<OPacket> &pkt);

void WriteVersionE (const boost::shared_ptr<OPacket> &pkt);

void WriteVersionF (const boost::shared_ptr<OPacket> &pkt);

void WriteVersionG (const boost::shared_ptr<OPacket> &pkt);

}

#endif
