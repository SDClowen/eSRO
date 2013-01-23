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

#include "packet_stall.hpp"
#include "packet_base_item.hpp"
#include "opcodes_shard_server.hpp"

#include <packet.hpp>

#include <stall.hpp>

namespace srv_pkt
{

void WriteStallItems (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Stall>& stall)
{
    if (stall)
    {
        for (Stall::const_iterator i = stall->begin(); i != stall->end(); ++i)
        {
            pkt->Write<uint8_t>(i->second.get_position());
            WriteItem(pkt,i->second,false);
            pkt->Write<uint8_t>(i->second.get_alt_position());
            pkt->Write<uint16_t>(i->second.get_stall_quantity());
            pkt->Write<uint64_t>(i->second.get_stall_price());
        }
    }

    pkt->Write<uint8_t>(0xFF);
}

void StallSpawn (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID, const std::string &name,
    const uint32_t avatar)
{
    pkt->WriteOpcode(SERVER_STALL_SPAWN);
    pkt->Write<uint32_t>(playerID);
    pkt->Write(name);
    pkt->Write<uint32_t>(avatar);
}

void StallDespawn (const boost::shared_ptr<OPacket> &pkt, const uint32_t playerID)
{
    pkt->WriteOpcode(SERVER_STALL_DESPAWN);
    pkt->Write<uint32_t>(playerID);
    pkt->Write<uint8_t>(0x17);
    pkt->Write<uint8_t>(0x3C);
}

void StallOpen (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_STALL_OPEN);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
}

void StallAction (const boost::shared_ptr<OPacket> &pkt, uint8_t action)
{
    pkt->WriteOpcode(SERVER_STALL_ACTION);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
    pkt->Write<uint8_t>(action);
}

void StallAction (const boost::shared_ptr<OPacket> &pkt, uint16_t status)
{
    pkt->WriteOpcode(SERVER_STALL_ACTION);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
    pkt->Write<uint8_t>(0x05);
    pkt->Write<uint16_t>(status);
    pkt->Write<uint8_t>(0);
}

void StallAction (const boost::shared_ptr<OPacket> &pkt, const std::string& greetings)
{
    pkt->WriteOpcode(SERVER_STALL_ACTION);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
    pkt->Write<uint8_t>(6);
    pkt->Write(greetings);
}

void StallAction (const boost::shared_ptr<OPacket> &pkt, const uint8_t pos, const uint16_t quantity,
    const uint64_t price)
{
    pkt->WriteOpcode(SERVER_STALL_ACTION);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
    pkt->Write<uint8_t>(0x01);
    pkt->Write<uint8_t>(pos);
    pkt->Write<uint16_t>(quantity);
    pkt->Write<uint64_t>(price);
    pkt->Write<uint8_t>(0);
}

void StallItem (const boost::shared_ptr<OPacket> &pkt, const boost::shared_ptr<Stall> &stall,
    const STALL_OPERATION type)
{
    pkt->WriteOpcode(SERVER_STALL_ACTION);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
    pkt->Write<uint16_t>(type);
    pkt->Write<uint8_t>(0);

    if (stall)
        WriteStallItems(pkt,stall);
}

void StallName (const boost::shared_ptr<OPacket> &pkt, const std::string& name, const uint32_t playerID)
{
    pkt->WriteOpcode(SERVER_STALL_NAME);
    pkt->Write<uint32_t>(playerID);
    pkt->Write(name);
}

void StallEnter (const boost::shared_ptr<OPacket> &pkt, const uint32_t splayerID, const uint32_t nplayerID,
    const boost::shared_ptr<Stall> &stall)
{
    pkt->WriteOpcode(SERVER_STALL_ENTER);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
    pkt->Write<uint32_t>(splayerID);
    pkt->Write(stall->get_greeting());

    if (stall->is_open())
        pkt->Write<uint16_t>(0x0101);	//TODO:!!! STALL STATUS
    else
        pkt->Write<uint16_t>(0);

    if (stall)
    {
        WriteStallItems(pkt,stall);

        std::vector<uint32_t> ids = stall->client_list();

        /// REMOVE SELF FROM LIST
        ids.erase(std::remove(ids.begin(),ids.end(),nplayerID));

        pkt->Write<uint8_t>(ids.size());
        for (size_t i = 0; i < ids.size(); ++i)
            pkt->Write<uint32_t>(ids[i]);
    }
}

void StallNotify (const boost::shared_ptr<OPacket> &pkt, const STALL_ACTION action, const uint32_t ID)
{
    pkt->WriteOpcode(SERVER_STALL_NOTIFY);
    pkt->Write<uint8_t>(action);
    pkt->Write<uint32_t>(ID);
}

void StallNotify (const boost::shared_ptr<OPacket> &pkt, const std::string& name,
    const boost::shared_ptr<Stall> &stall)
{
    pkt->WriteOpcode(SERVER_STALL_NOTIFY);
    pkt->Write<uint8_t>(STALL_ACTION_BUY);
    pkt->Write<uint8_t>(0x01);
    pkt->Write(name);

    if (stall)
        WriteStallItems(pkt,stall);
}

void StallLeave (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_STALL_LEAVE);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
}

void StallBuy (const boost::shared_ptr<OPacket> &pkt, const uint8_t pos)
{
    pkt->WriteOpcode(SERVER_STALL_BUY);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
    pkt->Write<uint8_t>(pos);
}

void StallBuy (const boost::shared_ptr<OPacket> &pkt, const STALL_ERROR error)
{
    pkt->WriteOpcode(SERVER_STALL_BUY,ANSWER_ERROR,error);
    pkt->Write<uint8_t>(0x3C);
}

void StallClose (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_STALL_CLOSE);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
}

}
