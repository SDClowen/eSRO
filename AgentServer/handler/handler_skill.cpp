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

#include "world.hpp"
#include "skill_manager.hpp"

/// SRLF INCLUDES
#include <player.hpp>
#include <skill.hpp>
#include <party.hpp>

/// SRDL INCLUDES
#include <skill_query.hpp>
#include <mastery_query.hpp>

/// SRNL INCLUDES
#include <packet_skill.hpp>
#include <packet_player.hpp>
#include <packet_mastery.hpp>

#if defined DEBUG
#include <packet_util_functions.hpp>
#endif

int StateGame::OnSkillInc (IPacket &packet)
{
    uint32_t skillID = packet.Read<uint32_t>();

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    boost::shared_ptr<SkillManager> smgr = m_server->GetWorld()->GetSkillManager();

    boost::shared_ptr<Skill> skill = smgr->find(skillID);

    if (!skill || player->skill_exists(skillID))
        return MSG_SUCCESS;

    /// CHECK THAT BELONGS TO THE PLAYER MASTERY TREE.
    MasteryTree tree = player->get_mastery_tree();

    MasteryTree::const_iterator i = tree.find(skill->required_mastery());

    if (i == tree.end())
        return MSG_ERROR_ORDER;

    /// CHECK IF THE PLAYER HAS THE REQUIRED MINIMUM MASTERY LEVEL TO LEVEL UP THIS SKILL
    if (i->second.Level() < skill->required_mastery_lv())
        return MSG_ERROR_ORDER;

    /// CHECK IF THE PLAYER HAS THE REQUIRED SKILLS TO LEVEL UP THIS SKILL.
    if (skill->required_group_skill1())
    {
        boost::shared_ptr<Skill> req;

        req = smgr->find_by_group_and_level(skill->required_group_skill1(), skill->required_group_skill1_lv());

        if (!req)
        {
#if defined DEBUG
            syslog(LOG_INFO,"Unable to find Required Skill1 %i for %i",skill->required_group_skill1(),skillID);
#endif
            return MSG_SUCCESS;
        }

        if (!player->skill_exists(req->id()))
        {
#if defined DEBUG
            syslog(LOG_INFO,"Skill requirements not complete.");
#endif
            return MSG_ERROR;
        }
    }

    if (skill->required_group_skill2())
    {
        boost::shared_ptr<Skill> req;

        req = smgr->find_by_group_and_level(skill->required_group_skill2(), skill->required_group_skill2_lv());

        if (!req)
        {
#if defined DEBUG
            syslog(LOG_INFO,"Unable to find Required Skill2 %i for %i",skill->required_group_skill2(),skillID);
#endif
            return MSG_SUCCESS;
        }

        if (!player->skill_exists(req->id()))
        {
#if defined DEBUG
            syslog(LOG_INFO,"Skill requirements not complete.");
#endif
            return MSG_ERROR;
        }
    }

    /// CHECK IF THE PLAYER HAS ENOUGH SP TO LEVEL UP THIS SKILL.
    if (!player->spend_sp(skill->required_sp()))
    {
#if defined DEBUG
        syslog(LOG_INFO,"OnSkillInc - Insufficient SP");
#endif
        return MSG_ERROR;
    }

    player->skill_insert(skillID);

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::IncSkill(pkt,skillID);
    m_connection->Send(pkt);

    if (skill->type() == SKT_PASSIVE)
    {
        Buff buff;
        buff.passive = true;
        buff.persistent = true;
        buff.Level = skill->level();
        buff.GroupID = skill->group_id();
        buff.Effects = skill->buff_list();

        buff.ReqShield = skill->require_shield();
        buff.ReqDevil = skill->require_devil();
        buff.ReqWeapon = skill->required_weapons();

        player->RemoveLesserBuffs(buff);
        player->insert_buff(buff);
    }

    pkt.reset(new OPacket);

    player->RecalculateStats();

    DB::SKILL::Create query;
    query(m_server->DBConnection(),player->ID(),skillID);

    return MSG_SUCCESS;
}

int StateGame::OnMasteryInc (IPacket &packet)
{
    uint32_t mastID = packet.Read<uint32_t>();

    uint8_t mastlv = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    if (mastlv != 1)
        return MSG_ERROR_ARG;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    MasteryTree &tree = player->get_mastery_tree();

    MasteryTree::iterator i = tree.find(mastID);

    if (i == tree.end())
        return MSG_ERROR_ORDER;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (tree.GetMasteryTotal() + 1 > tree.GetMasteryLimit())
    {
        srv_pkt::MasteryInc(pkt,srv_pkt::MASTERY_ERROR_LIMIT);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    if (i->second.Level() == m_server->GetWorld()->GetMaxCap())
    {
#if defined DEBUG
        syslog(LOG_INFO,"Mastery capped.");
#endif
        return MSG_SUCCESS;
    }

    int32_t reqsp = 0;

    if (i->second.Level())
        reqsp = m_server->GetWorld()->GetRequiredSP(i->second.Level());

    if (reqsp == -1)
    {
#if defined DEBUG
        syslog(LOG_INFO,"Required SP for Level %i not found.",i->second.Level()+1);
#endif
        return MSG_ERROR;
    }

    if (!player->spend_sp(reqsp))
    {
#if defined DEBUG
        syslog(LOG_INFO,"OnMasteryInc() - Insuficient SP");
#endif
        return MSG_ERROR_ORDER;
    }

    ++i->second;

    srv_pkt::MasteryInc(pkt,i->second);
    m_connection->Send(pkt);

    pkt.reset(new OPacket);

    srv_pkt::PlayerStats(pkt,player);
    m_connection->Send(pkt);

    boost::shared_ptr<Party> party = player->get_party();

    if (party)
    {
        std::pair<Mastery,Mastery> masts = tree.GetHighestMastery();
        party->SendMasteryNotification(player->GetUniqueID(),masts.first.ID(),masts.second.ID());
    }

    DB::MASTERY::Update query;
    query(m_server->DBConnection(),player->ID(),i->second);

    return MSG_SUCCESS;
}

int StateGame::OnSkillEdit (IPacket &packet)
{
    /*
    uint32_t refItemID = packet.Read<uint32_t>();

    uint32_t masteryID = packet.Read<uint32_t>();

    uint8_t level = packet.Read<uint8_t>();
    */

#if defined DEBUG
    PrintPacketToSyslog(packet);
#endif

    return MSG_SUCCESS;
}
