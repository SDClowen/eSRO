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

#include "server.hpp"
#include "server_connection.hpp"
#include "server_state_game.hpp"

/// SRNL INCLUDES
#include <packet_tutorial.hpp>

int StateGame::OnTutorialRequest (IPacket &packet)
{
    uint64_t ID = packet.Read<uint64_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::Tutorial(pkt,ID);
//    m_connection->Send(&pkt);

    return MSG_SUCCESS;
}

int StateGame::OnEventCalendar (IPacket &packet)
{
    uint8_t unk = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

//    OPacket p(0xB009,ANSWER_ACCEPT);
//    p.Write<uint8_t>(unk);
//
//    send(p);

    return MSG_SUCCESS;
}
