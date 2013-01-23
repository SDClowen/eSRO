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

#include "packet_gateway.hpp"

/// SRNL INCLUDES
#include <packet.hpp>
#include <opcodes_manager_gateway_client.hpp>

namespace clt_pkt
{

void WriteConfig (const boost::shared_ptr<OPacket> &p)
{
    p->WriteOpcode(GW_CL_CONFIG);
}

void WriteQueue (const boost::shared_ptr<OPacket> &p, const uint32_t clientID, const std::string &usr,
    const std::string &pw, const uint32_t shardID, const std::string &IP)
{
    p->WriteOpcode(GW_CL_LOGIN_QUEUE);
    p->Write<uint32_t>(clientID);
    p->Write<uint32_t>(shardID);
    p->Write(usr);
    p->Write(pw);
    p->Write(IP);
}

}
