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

#include "packet_transport.hpp"
#include "packet_base_item.hpp"
#include "opcodes_shard_server.hpp"

#include <packet.hpp>

#include <player.hpp>
#include <transport.hpp>
#include <storage.hpp>

namespace srv_pkt
{

void TransportStats (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Transport> &transport,
    const uint32_t playerID)
{
    pkt->WriteOpcode(SERVER_TRANSPORT_STATS);
    pkt->Write<uint32_t>(transport->GetUniqueID());
    pkt->Write<uint32_t>(transport->get_id());
    pkt->Write<uint32_t>(transport->hp());
    pkt->Write<uint32_t>(transport->max_hp());

    if (transport->type() == ET_COS_TRANSPORT)
    {
        boost::shared_ptr<Storage> store = transport->GetStorage();

        if (store)
        {
            boost::shared_ptr<Player> owner = transport->GetOwner();

            if (owner)
            {
                std::string alias = owner->get_job().get_alias();

                pkt->Write<uint8_t>(store->GetCapacity());
                pkt->Write<uint8_t>(store->size());

                for (Storage::iterator it = store->begin(); it != store->end(); ++it)
                {
                    WriteItem(pkt,it->second,true);
                    pkt->Write(alias);
                }
            }
        }

        pkt->Write<uint32_t>(playerID);
    }
    else
        pkt->Write<uint8_t>(0);
}


void TransportStatsRemove (const boost::shared_ptr<OPacket> &pkt, const uint32_t transportID)
{
    pkt->WriteOpcode(SERVER_TRANSPORT_STATS_REMOVE);
    pkt->Write<uint32_t>(transportID);
    pkt->Write<uint8_t>(1);
}

void TransportAction (const boost::shared_ptr<OPacket> &pkt, const MSG_ERROR_TRANSPORT &error)
{
    pkt->WriteOpcode(SERVER_TRANSPORT_ACTION);
    pkt->Write<uint8_t>(ANSWER_ERROR);
    pkt->Write<uint8_t>(error);
    pkt->Write<uint8_t>(0x00);
}

void TransportAction (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const TRANSPORT_ACTION action,
    const uint32_t transportID)
{
    pkt->WriteOpcode(SERVER_TRANSPORT_ACTION);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
    pkt->Write<uint32_t>(playerID);
    pkt->Write<uint8_t>(action);
    pkt->Write<uint32_t>(transportID);
}

void TransportTerminate (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_TRANSPORT_TERMINATE);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
}

void TransportOperation (const boost::shared_ptr<OPacket> &pkt, const TRANSPORT_OPERATION op)
{
    pkt->WriteOpcode(SERVER_NOTIFY_TRANSPORT_OPERATION);
    pkt->Write<uint8_t>(op);
}

}
