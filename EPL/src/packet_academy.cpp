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

#include "packet_academy.hpp"
#include "opcodes_shard_server.hpp"

#include <packet.hpp>

namespace srv_pkt
{

void AcademyList (const boost::shared_ptr<OPacket> &pkt, const uint8_t max_page, const uint8_t page,
    const std::vector<Academy> &list)
{
    pkt->WriteOpcode(SERVER_ACADEMY_LIST);
}

void AcademyNotice (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_ACADEMY_NOTICE);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
}

void AcademyRemove (const boost::shared_ptr<OPacket> &pkt, const uint32_t ID)
{
    pkt->WriteOpcode(SERVER_ACADEMY_REMOVE);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
    pkt->Write<uint32_t>(ID);
}

void AcademyForm (const boost::shared_ptr<OPacket> &pkt, const uint32_t number, const uint32_t ID, const uint8_t type,
    const std::string &name, const uint32_t rank)
{
    pkt->WriteOpcode(SERVER_ACADEMY_FORM);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
    pkt->Write<uint32_t>(number);
    pkt->Write<uint32_t>(ID);
    pkt->Write<uint8_t>(type);
    pkt->Write(name,OPacket::STRING_UNICODE);
    pkt->Write<uint32_t>(rank);
}

void AcademyNotify (const boost::shared_ptr<OPacket> &pkt, const ACADEMY_NOTIFY type)
{
    pkt->WriteOpcode(SERVER_ACADEMY_NOTIFY);
    pkt->Write<uint8_t>(type);
}

void AcademyNotify (const boost::shared_ptr<OPacket> &pkt, const std::string &title, const std::string &notice)
{
    pkt->WriteOpcode(SERVER_ACADEMY_NOTIFY);
    pkt->Write<uint8_t>(ACADEMY_NOTICE);
    pkt->Write(title);
    pkt->Write(notice);
}

void AcademyError (const boost::shared_ptr<OPacket> &pkt, const ACADEMY_ERROR error)
{
    pkt->WriteOpcode(SERVER_ACADEMY_ERROR);
    pkt->Write<uint8_t>(ANSWER_ERROR);
    pkt->Write<uint8_t>(error);
}

void AcademyDisband (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_ACADEMY_DISBAND);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
}

}
