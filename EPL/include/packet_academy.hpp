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

#ifndef __SLES_GAME_SERVER_ACADEMY_PACKETS_HPP__
#define __SLES_GAME_SERVER_ACADEMY_PACKETS_HPP__

#include <academy.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

class OPacket;

namespace srv_pkt
{

enum ACADEMY_NOTIFY
{
    ACADEMY_DISBAND = 0x01,
    ACADEMY_STATUS = 0x05,
    ACADEMY_NOTICE = 0x07
};

enum ACADEMY_STATE
{
    ACADEMY_LOGIN = 0x01,
    ACADEMY_LOGOFF = 0x02
};

enum ACADEMY_ERROR
{
    ERROR_ACADEMY_APPROVE_DEAD = 0x07,
    ERROR_ACADEMY_ESTABLISH_LEVEL = 0x09,
    ERROR_ACADEMY_NEEDED = 0x24,
    ERROR_ACADEMY_ESTABLISHED = 0x25
};

void AcademyInfo (const boost::shared_ptr<OPacket> &pkt, const Academy &academy);

void AcademyList (const boost::shared_ptr<OPacket> &pkt, const uint8_t max_pages, const uint8_t page,
    const std::vector<Academy> &list);

void AcademyNotice (const boost::shared_ptr<OPacket> &pkt);

void AcademyRemove (const boost::shared_ptr<OPacket> &pkt, const uint32_t ID);

void AcademyForm (const boost::shared_ptr<OPacket> &pkt, const uint32_t number, const uint32_t ID, const uint8_t type,
    const std::string &name, const uint32_t rank);

void AcademyNotify (const boost::shared_ptr<OPacket> &pkt, const ACADEMY_NOTIFY type);

void AcademyNotify (const boost::shared_ptr<OPacket> &pkt, const uint32_t ID, const ACADEMY_STATE type);

void AcademyNotify (const boost::shared_ptr<OPacket> &pkt, const std::string &title, const std::string &notice);

void AcademyError (const boost::shared_ptr<OPacket> &pkt, const ACADEMY_ERROR type);

void AcademyDisband (const boost::shared_ptr<OPacket> &pkt);

}

#endif //__SLES_GAME_SERVER_ACADEMY_PACKETS_HPP__
