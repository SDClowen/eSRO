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

#include "packet_fortress.hpp"
#include "opcodes_shard_server.hpp"

#include <packet.hpp>

#include <boost/bind.hpp>
#include <algorithm>

namespace srv_pkt
{

void WriteFortress (const boost::shared_ptr<OPacket> &pkt, const Fortress &fort)
{
    /*
    [<Unlabeled 1423158>][S -> C][3887][73 bytes]

    00 02 01 00 00 00 08 00 6C 6D 6D 6F 72 74 61 6C   ........lmmortal

    AC 11 00 00 0B 00 00 00 F4 05 00 00 01 00 00 00   ................

    00 00 06 00 00 00 0A 00 4C 61 74 69 6E 46 6F 72   ........LatinFor

    63 65 73 0E 00 00 08 00 00 00 F4 05 00 00 01 00   ces.............

    00 00 00 00 02 00 00 00 00                        ................
    */
    pkt->Write<uint32_t>(fort.ID);
    pkt->Write(fort.Owner);
    pkt->Write<uint32_t>(0);
    pkt->Write<uint32_t>(0);
    pkt->Write<uint32_t>(0x05F4);
    pkt->Write<uint32_t>(1);
    pkt->Write<uint16_t>(0);
}

void FortressInfo (boost::shared_ptr<OPacket> &pkt, const std::vector<Fortress> &list)
{
    pkt->WriteOpcode(SERVER_FORTRESS_INFO);
    pkt->Write<uint8_t>(0);
    pkt->Write<uint8_t>(list.size());

    for_each(list.begin(),list.end(),boost::bind(WriteFortress,pkt,_1));

    pkt->Write<uint8_t>(0);
}

}
