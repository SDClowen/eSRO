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

#include "packet_manager_shard.hpp"
#include "packet.hpp"
#include "opcodes_manager_gateway_client.hpp"

namespace srv_pkt
{

void WriteLoginQueue (const boost::shared_ptr<OPacket> &p, const uint32_t ID, const std::string &usr,
    const std::string &pw, const std::string &IP, const uint32_t accountID, const uint8_t acc, const uint8_t sec,
    const uint32_t silk)
{
    p->WriteOpcode(GM_SR_LOGIN_QUEUE);
    p->Write<uint32_t>(ID);
    p->Write(usr);
    p->Write(pw);
    p->Write(IP);
    p->Write(accountID);
    p->Write(acc);
    p->Write(sec);
    p->Write(silk);
}

}
