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

#ifndef __SLES_GAME_SERVER_EXCHANGE_PACKETS_HPP__
#define __SLES_GAME_SERVER_EXCHANGE_PACKETS_HPP__

#include <stdint.h>
#include <boost/shared_ptr.hpp>

class OPacket;
class ExchangeWnd;

namespace srv_pkt
{

void ExchangeItem (const boost::shared_ptr<OPacket> &pkt, const uint32_t player_id,
    const boost::shared_ptr<ExchangeWnd> exchange, bool add_inv_pos);

void ExchangeGold (const boost::shared_ptr<OPacket> &pkt, const uint64_t amount);

void ExchangeConfirm (const boost::shared_ptr<OPacket> &pkt);

void ExchangePlayerConfirm (const boost::shared_ptr<OPacket> &pkt);

void ExchangeApprove (const boost::shared_ptr<OPacket> &pkt);

void ExchangeCompleted (const boost::shared_ptr<OPacket> &pkt);

void ExchangeClose (const boost::shared_ptr<OPacket> &pkt, const bool is_closed);

void ExchangeAccept (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID);

void ExchangeAccepted (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID);

void ExchangeReject (const boost::shared_ptr<OPacket> &pkt);

}

#endif //__SLES_GAME_SERVER_EXCHANGE_PACKETS_HPP__
