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

#include "packet_exchange.hpp"
#include "packet_base_item.hpp"
#include "opcodes_shard_server.hpp"

#include <packet.hpp>

#include <exchange_window.hpp>

namespace srv_pkt
{

void ExchangeItem (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID,
    const boost::shared_ptr<ExchangeWnd> exchange, bool add_inv_pos)
{
    pkt->WriteOpcode(SERVER_EXCHANGE_ITEM);
    pkt->Write<uint32_t>(playerID);
    pkt->Write<uint8_t>(exchange->size());

    for ( ExchangeWnd::const_iterator iter = exchange->begin(); iter != exchange->end(); ++iter)
    {
        if (add_inv_pos)
            pkt->Write<uint8_t>(iter->second.get_alt_position());

        WriteItem(pkt,iter->second);
    }
}

void ExchangeGold (const boost::shared_ptr<OPacket> &pkt, const uint64_t amount)
{
    pkt->WriteOpcode(SERVER_EXCHANGE_GOLD);
    pkt->Write<uint8_t>(2);
    pkt->Write<uint64_t>(amount);
}

void ExchangeConfirm (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_EXCHANGE_CONFIRM);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
}

void ExchangePlayerConfirm (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_EXCHANGE_PLAYER_CONFIRMED);
}

void ExchangeApprove (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_EXCHANGE_APPROVE);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
}

void ExchangeCompleted (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_EXCHANGE_COMPLETED);
}

void ExchangeClose (const boost::shared_ptr<OPacket> &pkt, const bool is_closed)
{
    if (!is_closed)
    {
        pkt->WriteOpcode(SERVER_EXCHANGE_INIT_CLOSE);
        pkt->Write<uint8_t>(0x2C);
        pkt->Write<uint8_t>(0x18);
    }
    else
    {
        pkt->WriteOpcode(SERVER_EXCHANGE_END_CLOSE);
        pkt->Write<uint16_t>(0x1B02);
    }
}

void ExchangeAccept (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID)
{
    pkt->WriteOpcode(SERVER_EXCHANGE_ACCEPT);
    pkt->Write<uint32_t>(playerID);
}

void ExchangeAccepted (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID)
{
    pkt->WriteOpcode(SERVER_EXCHANGE_ACCEPTED);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
    pkt->Write<uint32_t>(playerID);
}

void ExchangeReject (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_EXCHANGE_INIT_CLOSE);
    pkt->Write<uint8_t>(0x28);
    pkt->Write<uint8_t>(0x18);
}

}
