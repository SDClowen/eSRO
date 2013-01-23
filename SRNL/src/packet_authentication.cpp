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

#include "packet_authentication.hpp"
#include "opcodes_global_server.hpp"

namespace srv_pkt
{

void WriteHandshake (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_AUTHENTICATION);
    pkt->Write<uint8_t>(HANDSHAKE_CLEAN);
}

void WriteHandshake (const boost::shared_ptr<OPacket> &pkt, const uint32_t public_key[2], const uint32_t count_seed,
    const uint32_t crc_seed, const uint32_t base_key[2], const uint32_t G, const uint32_t P, const uint32_t A)
{
    pkt->WriteOpcode(SERVER_AUTHENTICATION);
    pkt->Write<uint8_t>(HANDSHAKE_EXCHANGE);

    for (int i = 0; i < 2; ++i)
        pkt->Write<uint32_t>(public_key[i]);

    ///SEED COUNT
    pkt->Write<uint32_t>(count_seed);

    ///SEED CRC
    pkt->Write<uint32_t>(crc_seed);

    for (int i = 0; i < 2; ++i)
        pkt->Write<uint32_t>(base_key[i]);

    pkt->Write<uint32_t>(G);
    pkt->Write<uint32_t>(P);
    pkt->Write<uint32_t>(A);
}

void WriteHandshake (const boost::shared_ptr<OPacket> &pkt, const uint32_t signature[2])
{
    pkt->WriteOpcode(SERVER_AUTHENTICATION);
    pkt->Write<uint8_t>(HANDSHAKE_SIGNATURE);
    pkt->Write<uint32_t>(signature[0]);
    pkt->Write<uint32_t>(signature[1]);
}

void WriteWhoIs (const boost::shared_ptr<OPacket> &pkt, const std::string &ID)
{
    pkt->WriteOpcode(SERVER_WHOIS);
	pkt->Write(ID);
	pkt->Write<uint8_t>(0);
}

void WriteVersionA (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_INFORMATION);
    pkt->Write<uint16_t>(0x0101);
	pkt->Write<uint16_t>(0x0500);
	pkt->Write<uint8_t>(0x20);
}

void WriteVersionB (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_INFORMATION);
    pkt->Write<uint16_t>(0x0100);
	pkt->Write<uint16_t>(0x0100);
	pkt->Write<uint16_t>(0x0C69);   //Client Version ??? 1.189 = 0x095C, 1.190 = 0x02CF KSRO_VERSION=0x0C69
	pkt->Write<uint32_t>(0x05);
	pkt->Write<uint8_t>(0x02);
}

void WriteVersionC (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_INFORMATION);
    pkt->Write<uint16_t>(0x0101);
	pkt->Write<uint16_t>(0x0500);
	pkt->Write<uint8_t>(0x60);
}

void WriteVersionD (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_INFORMATION);
    pkt->Write<uint16_t>(0x0300);
	pkt->Write<uint16_t>(0x0200);
	pkt->Write<uint16_t>(0x0200);
}

void WriteVersionE (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_INFORMATION);
    pkt->Write<uint32_t>(0x0101);    ///ISRO AND KSRO <= 654 0x0101, KSRO VERSION 657 = 0x0B01
	pkt->Write<uint8_t>(0xA1);
}

void WriteVersionF (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_INFORMATION);
    pkt->Write<uint16_t>(0x0100);
}

void WriteVersionG (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_INFORMATION);
    pkt->Write<uint32_t>(0x04000101);
	pkt->Write<uint8_t>(0xA1);
}

}
