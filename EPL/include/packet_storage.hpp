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

#ifndef __SRNL_PACKET_STORAGE_HPP__
#define __SRNL_PACKET_STORAGE_HPP__

#include <stdint.h>
#include <boost/shared_ptr.hpp>

class OPacket;
class Storage;

namespace srv_pkt
{

void StorageGold (const boost::shared_ptr<OPacket> &pkt, const uint64_t amount);

void StorageItems (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Storage> storage);

void StorageEnter (const boost::shared_ptr<OPacket> &pkt);

}

#endif
