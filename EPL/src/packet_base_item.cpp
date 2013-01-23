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

#include "packet_base_item.hpp"

#include <packet.hpp>

namespace srv_pkt
{

void WriteItem (const boost::shared_ptr<OPacket> &pkt, const Item &item, bool append_pos)
{
    if (append_pos)
        pkt->Write<uint8_t>(item.get_position());

    pkt->Write<uint32_t>(0);                /// FOR VERSION 688
    pkt->Write<uint32_t>(item.get_id());

    int itype = item.get_type();

    if ( (itype >= ITEM_CH_SWORD && itype <= ITEM_AVATAR_HAT) ||
         (itype >= ITEM_JOB_TRADER && itype <= ITEM_PVP_CAPE))
    {
        pkt->Write<uint8_t>(item.get_enchant_lv());
        pkt->Write<uint64_t>(item.encoded_mods());
        pkt->Write<uint32_t>(item.get_durability());

        WriteAttr(pkt,item.get_attr_list());

        pkt->Write<uint8_t>(1);
        pkt->Write<uint8_t>(0);
        pkt->Write<uint8_t>(2);
        pkt->Write<uint8_t>(0);
    }
    else if (itype >= ITEM_HP_POT && itype <= ITEM_ALCHEMY_ATTR_STONE)
    {
        pkt->Write<uint16_t>(item.get_quantity());

        if (itype >= ITEM_ALCHEMY_MAGIC_STONE && itype <= ITEM_ALCHEMY_ATTR_STONE)
            pkt->Write<uint8_t>(item.get_assimilation());
    }
    else if (itype == ITEM_COS_PET && itype <= ITEM_COS_GRAB_PET)
    {
        pkt->Write<uint8_t>(3); ///Status
        pkt->Write<uint32_t>(6106); ///npc id !!! depends on lv and type for pets
        pkt->Write(item.get_pet_name());
        pkt->Write<uint8_t>(0);
    }
}

void WriteAttr (const boost::shared_ptr<OPacket> &pkt, const std::vector< ::Item::attribute> &attr)
{
    pkt->Write<uint8_t>(attr.size());

    for (size_t i = 0; i < attr.size(); ++i )
    {
        pkt->Write<uint32_t>(attr[i].first);
        pkt->Write<uint32_t>(attr[i].second);
    }
}

}
