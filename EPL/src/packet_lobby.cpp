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

#include "packet_lobby.hpp"
#include "opcodes_shard_server.hpp"

#include <packet.hpp>

/// SRLF INCLUDES
#include <player.hpp>
#include <storage.hpp>

namespace srv_pkt
{

void Lobby (const boost::shared_ptr<OPacket> &pkt, const LOBBY_ACTION action)
{
    pkt->WriteOpcode(SERVER_LOBBY);
    pkt->Write<uint8_t>(action);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
}

void Lobby (const boost::shared_ptr<OPacket> &pkt, const std::vector<boost::shared_ptr<Player> > &player_list)
{
    pkt->WriteOpcode(SERVER_LOBBY);
    pkt->Write<uint8_t>(LOBBY_ACTION_LIST);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
    pkt->Write<uint8_t>(player_list.size());

    boost::shared_ptr<Player> player;
    std::vector<boost::shared_ptr<Player> >::const_iterator i;

    for (i = player_list.begin(); i < player_list.end(); ++i)
    {
        player = *i;

        pkt->Write<uint32_t>(player->get_model());
        pkt->Write(player->get_name());
        pkt->Write<uint16_t>(0);     ///KSRO ONLY I THINK
        pkt->Write<uint8_t>(player->get_volume());
        pkt->Write<uint8_t>(player->level());
        pkt->Write<uint64_t>(player->get_exp());
        pkt->Write<uint16_t>(player->get_str());
        pkt->Write<uint16_t>(player->get_int());
        pkt->Write<uint16_t>(player->get_stat());
        pkt->Write<uint32_t>(player->hp());
        pkt->Write<uint32_t>(player->mp());

        if (player->is_erased())
        {
            pkt->Write<uint8_t>(1);
            pkt->Write<uint16_t>(player->get_timeleft());
            pkt->Write<uint32_t>(0);
            pkt->Write<uint8_t>(0);
        }
        else
            pkt->Write<uint32_t>(0);

        std::vector<Item> eq;
        boost::shared_ptr<Storage> il = player->get_storage(STORAGE_INVENTORY);

        Storage::const_iterator iter;
        for (iter = il->begin(); iter != il->end(); ++iter)
        {
            if (iter->first < Item::EARRING_SLOT)
                eq.push_back(iter->second);
        }

        pkt->Write<uint8_t>(eq.size());

        for (size_t j=0; j < eq.size(); ++j)
        {
            pkt->Write<uint32_t>(eq[j].get_id());
            pkt->Write<uint8_t>(eq[j].get_enchant_lv());
        }

        il = player->get_storage(STORAGE_AVATAR);
        pkt->Write<uint8_t>(il->size());

        for (iter = il->begin(); iter != il->end(); ++iter)
        {
            pkt->Write<uint32_t>(iter->second.get_id());
            pkt->Write<uint8_t>(0);
        }
    }
}

void Lobby (const boost::shared_ptr<OPacket> &pkt, const LOBBY_ACTION action, const LOBBY_ERROR error)
{
    pkt->WriteOpcode(SERVER_LOBBY);
    pkt->Write<uint8_t>(action);
    pkt->Write<uint8_t>(ANSWER_ERROR);
    pkt->Write<uint8_t>(error);
    pkt->Write<uint8_t>(0x04);
}

void RequestIngame (const boost::shared_ptr<OPacket> &pkt)
{
    pkt->WriteOpcode(SERVER_LOBBY_SELECT);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
}

}
