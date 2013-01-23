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

#include "packet_attack.hpp"
#include "packet_error_global.hpp"
#include "opcodes_shard_server.hpp"

#include <packet.hpp>

#include <player.hpp>

namespace srv_pkt
{

void WriteCastInfo (const boost::shared_ptr<OPacket> &pkt, const CastInfo &info)
{
    pkt->Write<uint32_t>(info.attackID);
    pkt->Write<uint32_t>(info.targetID);

    bool hasDmg = info.dmgInfo.size();
    bool hasDst = info.dest.get_zone();

    pkt->Write<uint8_t>(hasDmg | (hasDst << 3));    /// THERES ANOTHER FLAG 0x02!!!

    if (!info.dmgInfo.empty())
    {
        pkt->Write<uint8_t>(1);	//NUMBER ATTACKS PER HIT!!  DEPENDS ON THE OBJECT!!
        pkt->Write<uint8_t>(info.dmgInfo.size());

        for (std::vector<DamageInfo>::const_iterator it = info.dmgInfo.begin(); it != info.dmgInfo.end(); ++it)
        {
            pkt->Write<uint32_t>(it->targetID);
            pkt->Write<uint8_t>(it->AttackState);

            if (!(it->AttackState & ATTACK_STATE_BLOCK))
            {
                pkt->Write<uint8_t>(it->DmgState);
                pkt->Write<uint32_t>(it->damage);

                uint32_t EncStatus = 0;

                for (size_t i = 0; i < it->StatusList.size(); ++i)
                    EncStatus |= it->StatusList[i].ID;

                for (int i = 0; i < 3; ++i)
                    pkt->Write<uint8_t>((EncStatus >> (2*i+2)) & 0xFF);

                if (it->AttackState & ATTACK_STATE_POSITION)
                {
                    pkt->Write<int16_t>(it->dest.get_zone());
                    pkt->Write<int32_t>(it->dest.get_x());
                    pkt->Write<int32_t>(it->dest.get_z());
                    pkt->Write<int32_t>(it->dest.get_y());
                }
            }
        }
    }

    if (hasDst)
    {
        pkt->Write<int16_t>(info.dest.get_zone());
        pkt->Write<int32_t>(info.dest.get_x());
        pkt->Write<int32_t>(info.dest.get_z());
        pkt->Write<int32_t>(info.dest.get_y());
    }
}

void SkillCast_1st (const boost::shared_ptr<OPacket> &pkt, const SKILL_ACTION_TYPE type, const CastInfo &info)
{
    pkt->WriteOpcode(SERVER_ACTION_INFORMATION);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);
    pkt->Write<uint8_t>(type);
    pkt->Write<uint8_t>(0x30);
    pkt->Write<uint32_t>(info.skillID);
    pkt->Write<uint32_t>(info.casterID);

    WriteCastInfo(pkt,info);
}

void SkillCast_1st (const boost::shared_ptr<OPacket> &pkt, const pkt::SKCAST_ERROR_CODE error)
{
    pkt->WriteOpcode(SERVER_ACTION_INFORMATION);
    pkt->Write<uint8_t>(ANSWER_ERROR);
    pkt->Write<uint8_t>(error);
    pkt->Write<uint8_t>(pkt::GLOBAL_ERROR_SKILL_CAST);
}

void SkillCast_2nd (const boost::shared_ptr<OPacket> &pkt, const CastInfo &info)
{
    pkt->WriteOpcode(SERVER_DELAY_ACTION_INFORMATION);
    pkt->Write<uint8_t>(ANSWER_ACCEPT);

    WriteCastInfo(pkt,info);
}

}
