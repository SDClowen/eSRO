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

#include "world.hpp"

#include "character_factory.hpp"
#include "item_factory.hpp"
#include "formulas.hpp"
#include "manager/academy_manager.hpp"
#include "manager/event_manager.hpp"
#include "manager/fortress_manager.hpp"
#include "manager/friend_manager.hpp"
#include "manager/guild_manager.hpp"
#include "manager/map_manager.hpp"
#include "manager/party_manager.hpp"
#include "manager/resource_manager.hpp"
#include "manager/script_manager.hpp"
#include "manager/skill_manager.hpp"
#include "manager/stall_manager.hpp"
#include "manager/trade_manager.hpp"
#include "manager/union_manager.hpp"
#include "manager/weather_manager.hpp"
#include "near_objects.hpp"
#include "npc_list.hpp"
#include "npc_util.hpp"
#include "server.hpp"
#include "server_connection.hpp"
#include "server_channel.hpp"
#include "zone.hpp"

/// SOL INCLUDES
#include <mob.hpp>
#include <guild.hpp>
#include <transport.hpp>
#include <storage.hpp>
#include <dropable_item.hpp>
#include <skill_builder.hpp>
#include <interactive_npc.hpp>

/// EPL INCLUDES
#include <packet.hpp>
#include <packet_npc.hpp>
#include <packet_spawn.hpp>
#include <packet_item.hpp>
#include <packet_uniques.hpp>
#include <packet_attack.hpp>
#include <packet_teleport.hpp>
#include <packet_player.hpp>
#include <packet_skill.hpp>
#include <packet_transport.hpp>
#include <packet_job.hpp>

/// KHAMAILEON INCLUDES
#include <model_manager.hpp>

#include <syslog.h>
#include <boost/bind.hpp>

World::World (Server *server)
    :   m_server(server),
        m_EventMgr(new EventManager()),
        m_WeatherMgr(new WeatherManager()),
        m_FortressMgr(new FortressManager()),
        m_ScriptMgr(new ScriptManager()),
        m_AcademyMgr(new AcademyManager()),
        m_PartyMgr(new PartyManager()),
        m_StallMgr(new StallManager()),
        m_stop_flag(false),
        m_time_tg(m_io_service,boost::posix_time::hours(0)),
        m_time_uru(m_io_service,boost::posix_time::hours(0)),
        m_time_isy(m_io_service,boost::posix_time::hours(0)),
        m_time_cerb(m_io_service,boost::posix_time::hours(0)),
        m_time_ivy(m_io_service,boost::posix_time::hours(0)),
        m_world_timer(m_io_service,boost::posix_time::seconds(1)),
        m_thread_count(1),
        m_spawn_count(1),
        m_max_cap(0),
        m_market(false),
        m_allow_ch(false),
        m_allow_eu(false),
        m_bg_counter(0),
        m_bg_task_timer(m_io_service),
        m_state_task_timer(m_io_service),
        m_spawn_timer(m_io_service),
        m_work(m_io_service)
{
	for (int i = 0; i < MAX_UNIQUES; ++i)
        m_unique_spawned[i] = false;
}

World::~World ()
{
    /// CLEAN ALL TIMERS
    for ( timer_iterator it = m_tasks.begin(); it != m_tasks.end(); ++it)
        delete it->second;
}

bool World::Initialize ()
{
    /// CHECK THAT CONFIG VARIABLES ARE VALID
    if (m_db.empty())
    {
        m_last_error = "DatabaseName field not assigned.";
        return false;
    }

    if (m_db_host.empty())
    {
        m_last_error = "DatabaseIP field not assigned.";
        return false;
    }

    if (m_db_usr.empty())
    {
        m_last_error = "DatabaseUser field not assigned.";
        return false;
    }

    if (m_db_pw.empty())
    {
        m_last_error = "DatabasePassword field not assigned.";
        return false;
    }

    if (!m_max_cap || m_max_cap > 140)
    {
        m_last_error = "MaxCap need to be between 0 and 140";
        return false;
    }

    if (!m_allow_ch && !m_allow_eu)
    {
        m_last_error = "Need to allow atleast 1 race.";
        return false;
    }

    if (m_path_data.empty())
    {
        m_last_error = "DataPath field not assigned.";
        return false;
    }

    if (m_path_media.empty())
    {
        m_last_error = "MediaPath field not assigned.";
        return false;
    }

    boost::shared_ptr<ResourceManager> ResourceMgr(new ResourceManager(m_path_media,m_db_usr,m_db_pw,m_db_host,m_db));

    syslog(LOG_INFO,"Initializing Resource Manager.");

    if (!ResourceMgr || !ResourceMgr->Initialize())
        return false;

    syslog(LOG_INFO,"Loading Models.");

    m_ModelMgr.reset(new ModelManager(m_path_data));

    if (!m_ModelMgr)
    {
        syslog(LOG_INFO,"Failed to allocate Model Manager.");
        return false;
    }

    if (!m_ModelMgr->Initialize())
    {
        syslog(LOG_INFO,"Failed to initialize Model Manager.");
        return false;
    }

    m_ModelMgr->LoadModels();

    syslog(LOG_INFO,"Initializing Map.");
    m_MapMgr.reset(new MapManager(m_path_data));

    if (!m_MapMgr)
    {
        syslog(LOG_INFO,"Failed to allocate Map Manager.");
        return false;
    }

    syslog(LOG_INFO,"Creating Zones and Divisions.");

    if (!m_MapMgr->LoadZones())
    {
        syslog(LOG_INFO,"Failed to create Zones and Divisions.");
        return false;
    }

    for (zone_iterator iter = m_MapMgr->begin(); iter != m_MapMgr->end(); ++iter)
    {
        iter->second->setModelManager(m_ModelMgr);

        m_server->InsertChannel(iter->second->ID(),iter->second->GetChannel());
    }

    syslog(LOG_INFO,"Initializing Weather Manager.");

    if (!m_WeatherMgr->Initialize())
        return false;

    syslog(LOG_INFO,"Initializing Guild Manager.");

    m_GuildMgr.reset(new GuildManager(m_db_usr,m_db_pw,m_db_host,m_db));

    if (!m_GuildMgr)
        return false;

    if (!m_GuildMgr->Initialize())
        return false;

    syslog(LOG_INFO,"Initializing Union Manager.");

    m_UnionMgr.reset(new UnionManager(m_db_usr,m_db_pw,m_db_host,m_db,m_GuildMgr));

    if (!m_UnionMgr)
        return false;

    if (!m_UnionMgr->Initialize())
        return false;

    syslog(LOG_INFO,"Initializing Academy Manager.");

    if (!m_AcademyMgr->Initialize())
        return false;

    syslog(LOG_INFO,"Initializing Script Manager.");

    if (!m_ScriptMgr->Initialize())
        return false;

    syslog(LOG_INFO,"Initializing Event Manager.");

    if (!m_EventMgr->Initialize())
        return false;

    syslog(LOG_INFO,"Initializing Stall Manager.");

    if (!m_StallMgr->Initialize())
        return false;

    syslog(LOG_INFO,"Initializing Trade Manager.");

    m_TradeMgr.reset(new TradeManager(m_db_usr,m_db_pw,m_db_host,m_db));

    if (!m_TradeMgr || !m_TradeMgr->Initialize())
    {
        syslog(LOG_INFO,"Failed to initialize trade manager.");
        return false;
    }

    m_FriendMgr.reset(new FriendManager(m_db_usr,m_db_pw,m_db_host,m_db));

    if (!m_FriendMgr || !m_FriendMgr->initialize())
    {
        syslog(LOG_INFO,"Failed to initialize friend manager.");
        return false;
    }

    syslog(LOG_INFO,"Initializing Fortress Manager.");

    if (!m_FortressMgr->Initialize())
        return false;

    syslog(LOG_INFO,"Loading available fortress.");
    std::vector<FortressProperty> fortress_list = ResourceMgr->LoadFortress();

    if (fortress_list.empty())
    {
        syslog(LOG_INFO,"Failed to load available fortress.");
        return false;
    }

    std::for_each(fortress_list.begin(),fortress_list.end(),
        boost::bind(&FortressManager::addFortress,m_FortressMgr,_1));

    syslog(LOG_INFO,"Initializing Items.");

    m_ItemFactory = boost::shared_ptr<ItemFactory>(new ItemFactory(ResourceMgr->LoadItemProperties()));

    if (!m_ItemFactory)
    {
        syslog(LOG_INFO,"Failed to create ItemFactory.");
        return false;
    }

    m_MagicOptionChart = ResourceMgr->LoadMagicOptionProperties();

    syslog(LOG_INFO,"Initializing Characters.");

    m_CharacterFactory.reset(new CharacterFactory(ResourceMgr->LoadCharacterProperties(),
        ResourceMgr->LoadCharacterQualityChart()));

    if (!m_CharacterFactory)
    {
        return false;
    }

    syslog(LOG_INFO,"Creating Character Skills.");

    std::vector<SkillProperty> sp = ResourceMgr->LoadSkillProperties();

    ///CREATE SKILLS
    SkillBuilder builder;
    boost::shared_ptr<Skill> skill;
    boost::unordered_map<uint32_t,boost::shared_ptr<Skill> > skill_list;

    for (std::vector<SkillProperty>::const_iterator iter = sp.begin(); iter != sp.end(); ++iter)
    {
        builder.build(*iter);

        skill = builder.get_built_object();

        if(skill)
            skill_list.insert(std::make_pair(skill->id(),skill));
    }

    syslog (LOG_INFO,"Initializing Skill Manager.");

    m_SkillMgr.reset(new SkillManager(skill_list));

    if (!m_SkillMgr)
    {
        return false;
    }

    m_ShopItemStacks = ResourceMgr->LoadShopItemStacks();

    syslog(LOG_INFO,"Loading Experience/Skill Point data.");

    m_XPChart = ResourceMgr->LoadXPChart();

    if (m_XPChart.empty())
    {
        syslog(LOG_INFO,"Failed to load Experience/Skill Point data.");
        return false;
    }

    syslog(LOG_INFO,"Loading Gold drop data.");

    m_GoldChart = ResourceMgr->LoadGoldChart();

    if (m_GoldChart.empty())
    {
        syslog(LOG_INFO,"Failed to load Gold drop data.");
        return false;
    }

    syslog(LOG_INFO,"Loading Item drop data.");

    m_DropChart = ResourceMgr->LoadDropChart();

    if (m_DropChart.empty())
    {
        syslog(LOG_INFO,"Drop table empty.");
        return false;
    }

    m_DropWeightChart = ResourceMgr->LoadDropWeightChart();

    if (m_DropWeightChart.empty())
    {
        syslog(LOG_INFO,"Drop weight table empty.");
        return false;
    }

    syslog(LOG_INFO,"Loading Teleport data.");

    m_TeleportChart = ResourceMgr->LoadTeleportInfo();

    m_TeleportLinkChart = ResourceMgr->LoadTeleportLinks();

    m_TeleportOptionalChart = ResourceMgr->LoadTeleportOptional();

    if (m_TeleportChart.empty() || m_TeleportLinkChart.empty())
    {
        syslog(LOG_INFO,"Failed to load teleport data.");
        return false;
    }

    syslog(LOG_INFO,"Creating Teleport Buildings.");

    std::vector<boost::shared_ptr<TeleportBuilding> > building_list = ResourceMgr->LoadTeleportBuildings();

    if (building_list.empty())
    {
        syslog(LOG_INFO,"Failed to create teleport buildings.");
        return false;
    }

    for (std::vector<boost::shared_ptr<TeleportBuilding> >::iterator i = building_list.begin();
        i != building_list.end(); ++i)
    {
        boost::shared_ptr<TeleportBuilding> blg = *i;

        for (teleport_const_iterator j = m_TeleportChart.begin(); j != m_TeleportChart.end(); ++j)
        {
            if (j->second.AssignedID == blg->m_Property.refID)
            {
                blg->TeleportID = j->second.ID;
                break;
            }
        }

        ///SET TAX RATE DEPENDING ON THE FORTRESS THATS LINKED TO

        if (m_FortressMgr->isFortressActive(blg->m_Property.TaxFortress))
        {
            blg->TaxActive = true;
            blg->TaxRate = 0;
        }

        if (blg->m_Property.refID == 2197)    /// THIEF TOWN TELEPORT
        {
            blg->Tasks.push_back(TELEPORT_TASK_TELEPORT_RETURN_POINT);

            InsertBuilding(blg);
        }
        else
        {
            blg->Tasks.push_back(TELEPORT_TASK_DESIGNATE);
            blg->Tasks.push_back(TELEPORT_TASK_TELEPORT);

            if (blg->TeleportID)
                InsertBuilding(blg);
        }
    }

    syslog(LOG_INFO,"Loading Spawn points.");
    m_spawn_list = ResourceMgr->LoadSpawnPoints();

    if (m_spawn_list.empty())
    {
        syslog(LOG_INFO,"Failed to load spawn points.");
        return false;
    }

    syslog(LOG_INFO,"Creating NPC/Mobs.");

    boost::shared_ptr<Zone> zone;
    std::multimap<uint32_t,NPCTask> task_table = ResourceMgr->LoadNPCTasks();

    size_t spawn_idx = 0;
    for (std::vector<SpawnPoint>::iterator i = m_spawn_list.begin(); i != m_spawn_list.end(); ++i,++spawn_idx)
    {
        zone = m_MapMgr->Find(i->Zone);

        if (!zone)
        {
            syslog(LOG_INFO,"Zone %i dont exists",i->Zone);
            continue;
        }

        if (IsInteractiveNPC(i->refCharID))
        {
            boost::shared_ptr<NPC> npc = m_CharacterFactory->Create(i->refCharID);

            if (!npc)
            {
                syslog(LOG_INFO,"Failed to create NPC with ID = %i.",i->refCharID);
                continue;
            }

            npc->set_position(Coord(i->x,i->y,i->z,i->Zone));

            boost::shared_ptr<InteractiveNPC> itv = boost::shared_dynamic_cast<InteractiveNPC,NPC>(npc);

            ///ADD THE TASK THEY CAN PERFORM
            typedef std::multimap<uint32_t,NPCTask>::const_iterator task_const_iterator;
            std::pair<task_const_iterator,task_const_iterator> iter_range = task_table.equal_range(i->refCharID);

            for (task_const_iterator titer = iter_range.first; titer != iter_range.second; ++titer)
                itv->InsertTask(titer->second);

            ///SET THE TELEPORT ID
            if (IsTeleportNPC(i->refCharID))
            {
                for (teleport_const_iterator j = m_TeleportChart.begin(); j != m_TeleportChart.end(); ++j)
                {
                    if (j->second.AssignedID == i->refCharID)
                        itv->SetTeleportID(j->second.ID);
                }
            }

            if (m_FortressMgr->isFortressActive(itv->GetFortress()))
            {
                itv->setFortressActive(true);
            }

            InsertNPC(itv);
        }
        else if (IsUniqueNPC(i->refCharID))
        {
            m_UniqueSpawnPoints.insert(std::make_pair(i->refCharID,*i));
        }
        else
        {
            Coord pos = Coord(i->x,i->y,i->z,i->Zone);
            boost::shared_ptr<NPC> npc;
            std::vector<boost::shared_ptr<NPC> > list;

            for (int j = 0; j < m_spawn_count; ++j)
            {
                npc.reset();
                npc = m_CharacterFactory->Create(i->refCharID);

                if (npc)
                {
                    if (!npc->running_speed())
                        break;

                    npc->set_position(pos);

                    boost::shared_ptr<Mob> mob = boost::shared_dynamic_cast<Mob,NPC>(npc);

                    if (mob)
                        mob->SetSpawnPoint(spawn_idx);

                    list.push_back(npc);
                }
                else
                {
                    syslog(LOG_INFO,"Unable to create NPC, ID = %i",i->refCharID);
                }
            }

            InsertNPC(list,pos);

            list.clear();
        }
    }

    syslog(LOG_INFO,"Spawning Uniques.");

    m_time_tg.async_wait(boost::bind(&World::OnSpawnUnique,this,(uint8_t)TIGERWOMAN,(uint32_t)MOB_CH_TIGERWOMAN,
        boost::asio::placeholders::error));

    m_time_uru.async_wait(boost::bind(&World::OnSpawnUnique,this,(uint8_t)URUCHI,(uint32_t)MOB_OA_URUCHI,
        boost::asio::placeholders::error));

    m_time_isy.async_wait(boost::bind(&World::OnSpawnUnique,this,(uint8_t)ISYUTARU,(uint32_t)MOB_KK_ISYUTARU,
        boost::asio::placeholders::error));

    m_time_cerb.async_wait(boost::bind(&World::OnSpawnUnique,this,(uint8_t)CERBERUS,(uint32_t)MOB_EU_KERBEROS,
        boost::asio::placeholders::error));

    m_time_ivy.async_wait(boost::bind(&World::OnSpawnUnique,this,(uint8_t)IVY,(uint32_t)MOB_AM_IVY,
        boost::asio::placeholders::error));

    m_bg_task_timer.expires_from_now(boost::posix_time::seconds(1));
    m_bg_task_timer.async_wait(boost::bind(&World::OnBackgroundTask,this,boost::asio::placeholders::error));

    m_state_task_timer.expires_from_now(boost::posix_time::seconds(1));
    m_state_task_timer.async_wait(boost::bind(&World::OnStateTask,this,boost::asio::placeholders::error));

    m_spawn_timer.expires_from_now(boost::posix_time::seconds(1));
    m_spawn_timer.async_wait(boost::bind(&World::OnRespawnMob,this,boost::asio::placeholders::error));

    syslog(LOG_INFO,"World Initialized Succesfully.");

    return true;
}

void World::Release ()
{
}

boost::shared_ptr<MapManager> World::GetMapManager ()
{
    return m_MapMgr;
}

boost::shared_ptr<EventManager> World::GetEventManager ()
{
    return m_EventMgr;
}

boost::shared_ptr<WeatherManager> World::GetWeatherManager ()
{
    return m_WeatherMgr;
}

boost::shared_ptr<FortressManager> World::GetFortressManager ()
{
    return m_FortressMgr;
}

boost::shared_ptr<ScriptManager> World::GetScriptManager ()
{
    return m_ScriptMgr;
}

boost::shared_ptr<GuildManager> World::GetGuildManager ()
{
    return m_GuildMgr;
}

boost::shared_ptr<UnionManager> World::GetUnionManager ()
{
    return m_UnionMgr;
}

boost::shared_ptr<AcademyManager> World::GetAcademyManager ()
{
    return m_AcademyMgr;
}

boost::shared_ptr<PartyManager> World::GetPartyManager ()
{
    return m_PartyMgr;
}

boost::shared_ptr<StallManager> World::GetStallManager ()
{
    return m_StallMgr;
}

boost::shared_ptr<TradeManager> World::GetTradeManager ()
{
    return m_TradeMgr;
}

boost::shared_ptr<SkillManager> World::GetSkillManager ()
{
    return m_SkillMgr;
}

boost::shared_ptr<FriendManager> World::GetFriendManager()
{
    return m_FriendMgr;
}

boost::shared_ptr<ItemFactory> World::GetItemFactory ()
{
    return m_ItemFactory;
}

boost::shared_ptr<CharacterFactory> World::GetCharacterFactory ()
{
    return m_CharacterFactory;
}

bool World::IsInsideTown (const Coord &position) const
{
    return m_MapMgr->IsInsideTown(position);
}

bool World::resolveMotion (const Coord &src, const Coord &dest, Coord *end) const
{
    return m_MapMgr->ResolveMotion(src,dest,end);
}

void World::InsertNPC (const boost::shared_ptr<NPC> &npc)
{
    int16_t zoneID = npc->get_position().get_zone();

    boost::shared_ptr<Zone> zone = m_MapMgr->Find(zoneID);

    if (zone)
    {
        zone->InsertNPC(npc);

        CreateTimer(npc->GetUniqueID());

        boost::shared_ptr<OPacket> pkt;

        if (zone->Count())
        {            
            pkt.reset(new OPacket);
            boost::shared_ptr<OPacket> p2(new OPacket), p3(new OPacket);

            if (pkt && p2 && p3)
            {
                srv_pkt::SpawnBegin(pkt,1,srv_pkt::SPAWN_OBJECT);
                srv_pkt::Spawn(p2,npc);
                srv_pkt::SpawnEnd(p3);

                m_server->SendChannel(pkt,p2,p3,zoneID);
            }
        }

        switch(npc->type())
        {
        case ET_COS_TRANSPORT:
        case ET_COS_VEHICLE:
            {
                if (zone->Count())
                {
                    pkt.reset(new OPacket);

                    if (pkt)
                    {
                        srv_pkt::Speed(pkt,npc);
                        m_server->SendChannel(pkt,zoneID);
                    }

                    pkt.reset(new OPacket);

                    if (pkt)
                    {
                        srv_pkt::State(pkt,npc->GetUniqueID(),STATE_MOVEMENT,MOVEMENT_ACTION_STAND);
                        m_server->SendChannel(pkt,zoneID);
                    }
                }

                npc->signal_dead.connect(boost::bind(&World::OnSignalTransportDead,this,_1,_2));
            }
            break;
        case ET_MOB:
        case ET_MOB_QUEST:
        case ET_MOB_HUNTER:
        case ET_MOB_THIEF:
        case ET_MOB_EVENT:
            {
                boost::shared_ptr<Mob> mob = boost::shared_dynamic_cast<Mob,NPC>(npc);

                if (zone->Count())
                {
                    if (mob->Quality() == CH_QUALITY_UNIQUE)
                    {
                        pkt.reset(new OPacket);

                        srv_pkt::Unique(pkt,mob->get_id());
                        m_server->Broadcast(pkt);
                    }

                    m_io_service.post(boost::bind(&World::InitTaskAI,this,mob));
                }

                npc->signal_dead.connect(boost::bind(&World::OnSignalMobDead,this,_1,_2));
            }
            break;
        default:
            break;
        }
    }
    else
        syslog(LOG_INFO,"Zone %i dont exists",zoneID);
}

void World::InsertNPC (const std::vector<boost::shared_ptr<NPC> > &list, const Coord &pos)
{
    typedef std::vector<boost::shared_ptr<NPC> >::const_iterator npc_const_iterator;

    boost::shared_ptr<Zone> zone = m_MapMgr->Find(pos.get_zone());

    if (zone)
    {
        uint32_t uniqueID = 0;

        for (npc_const_iterator i = list.begin(); i != list.end(); ++i)
        {
            zone->InsertNPC(*i);
            
            CreateTimer((*i)->GetUniqueID());

            if ((*i)->type() == ET_MOB)
            {
                boost::shared_ptr<Mob> mob = boost::shared_dynamic_cast<Mob,NPC>(*i);

                if (mob->Quality() == CH_QUALITY_UNIQUE)
                    uniqueID = mob->get_id();
            }

            (*i)->signal_dead.connect(boost::bind(&World::OnSignalMobDead,this,_1,_2));
        }

        boost::shared_ptr<OPacket> pkt;

        if (uniqueID)
        {
            pkt.reset(new OPacket);

            srv_pkt::Unique(pkt,uniqueID);
            m_server->Broadcast(pkt);
        }

        if (zone->Count())
        {
            boost::shared_ptr<OPacket> p2,p3;

            int MAX_ENTITIES = 16;
            int size = 0;
            int pending = list.size();
            std::vector<boost::shared_ptr<NPC> >::const_iterator beg = list.begin();

            do
            {
                if (pending > MAX_ENTITIES)
                    size = MAX_ENTITIES;
                else
                    size = pending;

                pkt.reset(new OPacket);
                p2.reset(new OPacket);
                p3.reset(new OPacket);

                if (pkt && p2 && p3)
                {
                    srv_pkt::SpawnBegin(pkt,size,srv_pkt::SPAWN_OBJECT);
                    srv_pkt::Spawn(p2,beg,beg+size);
                    srv_pkt::SpawnEnd(p3);

                    m_server->SendChannel(pkt,p2,p3,pos.get_zone());
                }

                pending -= size;
                beg += size;
            }
            while (beg != list.end() && pending > 0);

            for (npc_const_iterator i = list.begin(); i != list.end(); ++i)
            {
                if ((*i)->type() == ET_MOB)
                    m_io_service.post(boost::bind(&World::InitTaskAI,this,*i));
            }
        }
    }
    else
        syslog(LOG_INFO,"Zone %i dont exists",pos.get_zone());
}

void World::RemoveNPC (uint32_t npcID, const Coord &pos)
{
    boost::shared_ptr<Zone> zone = m_MapMgr->Find(pos.get_zone());

    if (zone)
    {
        boost::shared_ptr<NPC> npc = zone->FindNPC(npcID);

        if (npc)
        {
            zone->RemoveNPC(npcID);
            RemoveTimer(npcID);

            if (zone->Count())
            {
                boost::shared_ptr<OPacket> pkt(new OPacket);

                if (pkt)
                {
                    srv_pkt::Despawn(pkt,npcID);
                    m_server->SendChannel(pkt,pos.get_zone());
                }
            }

            switch(npc->type())
            {
            case ET_COS_TRANSPORT:
            case ET_COS_VEHICLE:
                npc->signal_dead.disconnect(boost::bind(&World::OnSignalTransportDead,this,_1,_2));
                break;
            case ET_MOB:
            case ET_MOB_QUEST:
            case ET_MOB_HUNTER:
            case ET_MOB_THIEF:
            case ET_MOB_EVENT:
                npc->signal_dead.disconnect(boost::bind(&World::OnSignalMobDead,this,_1,_2));
                break;
            default:
                break;
            }
        }
    }
    else
        syslog(LOG_INFO,"NPC %i dont exists",npcID);
}

boost::shared_ptr<NPC> World::FindNPC_CloseToPosition (uint32_t npcID, const Coord &pos) const
{
    int16_t zoneID = 0;
    boost::shared_ptr<NPC> obj;
    boost::shared_ptr<Zone> zone;
    AdjacentZone area = GetAdjacentZones(pos.get_zone());

    for ( uint8_t x = area.min_x; x <= area.max_x; ++x)
    {
        for (uint8_t y = area.min_y; y <= area.max_y; ++y)
        {
            zoneID = (y << 8) + x;
            zone = m_MapMgr->Find(zoneID);

            if (zone)
            {
                obj = zone->FindNPC(npcID);

                if (obj)
                    return obj;
            }
        }
    }

    return obj;
}

void World::InsertItem (const boost::shared_ptr<DropableItem> &item)
{
    int16_t zoneID = item->get_position().get_zone();
    boost::shared_ptr<Zone> zone = m_MapMgr->Find(zoneID);

    if (zone)
    {
        zone->InsertItem(item);
        CreateTimer(item->GetUniqueID());

        /// INITIATE DESPAWN TASK!!
        /// TODO: READ DESPAWN TIME FROM ITEM OBJECT.
        m_io_service.post(boost::bind(&World::InitTask_ItemDespawn,this,item->GetUniqueID(),zoneID,180000));

        if (zone->Count())
        {
            boost::shared_ptr<OPacket> pkt(new OPacket);

            if (pkt)
            {
                srv_pkt::Spawn(pkt,item);
                m_server->SendChannel(pkt,zoneID);
            }

            if (item->m_Item.get_type() != ITEM_GOLD)
            {
                pkt.reset(new OPacket);

                if (pkt)
                {
                    srv_pkt::ItemDrop(pkt,item->GetUniqueID());
                    m_server->SendChannel(pkt,zoneID);
                }
            }
        }
    }
    else
        syslog(LOG_INFO,"Zone %i dont exists",zoneID);
}

void World::RemoveItem (const uint32_t itemID, const Coord &pos)
{
    int16_t zoneID = pos.get_zone();
    boost::shared_ptr<Zone> zone = m_MapMgr->Find(zoneID);

    if (zone)
    {
        if (zone->RemoveItem(itemID))
        {
            RemoveTimer(itemID);

            if (zone->Count())
            {
                boost::shared_ptr<OPacket> pkt(new OPacket);

                if (pkt)
                {
                    srv_pkt::Despawn(pkt,itemID);
                    m_server->SendChannel(pkt,zoneID);
                }
            }
        }
    }
    else
        syslog(LOG_INFO,"Zone %i dont exists",zoneID);
}

boost::shared_ptr<DropableItem> World::FindItem_CloseToPosition (uint32_t itemID, const Coord &pos) const
{
    int16_t zoneID = 0;
    boost::shared_ptr<DropableItem> obj;
    boost::shared_ptr<Zone> zone;
    AdjacentZone area = GetAdjacentZones(pos.get_zone());

    for ( uint8_t x = area.min_x; x <= area.max_x; ++x)
    {
        for (uint8_t y = area.min_y; y <= area.max_y; ++y)
        {
            zoneID = (y << 8) + x;
            zone = m_MapMgr->Find(zoneID);

            if (zone)
            {
                obj = zone->FindItem(itemID);

                if (obj)
                    return obj;
            }
        }
    }

    return obj;
}

void World::InsertBuilding (const boost::shared_ptr<TeleportBuilding> &building)
{
    boost::shared_ptr<Zone> zone = m_MapMgr->Find(building->m_Property.Zone);

    if (zone)
        zone->InsertBuilding(building);
    else
        syslog(LOG_INFO,"Zone %i dont exists",building->m_Property.Zone);
}

void World::RemoveBuilding (const uint32_t buildingID, const Coord &pos)
{
   boost::shared_ptr<Zone> zone = m_MapMgr->Find(pos.get_zone());

    if (zone)
        zone->RemoveBuilding(buildingID);
    else
        syslog(LOG_INFO,"Zone %i dont exists",pos.get_zone());
}

boost::shared_ptr<TeleportBuilding> World::FindBuilding_CloseToPosition (const Coord &pos)
{
    int16_t zoneID = 0;
    boost::shared_ptr<Zone> zone;
    boost::shared_ptr<TeleportBuilding> obj;
    AdjacentZone area = GetAdjacentZones(pos.get_zone());

    for ( uint8_t x = area.min_x; x <= area.max_x; ++x)
    {
        for (uint8_t y = area.min_y; y <= area.max_y; ++y)
        {
            zoneID = (y << 8) + x;
            zone = m_MapMgr->Find(zoneID);

            if (zone)
            {
                obj = zone->FindBuilding();

                if (obj)
                    return obj;
            }
        }
    }

    return obj;
}

boost::shared_ptr<TeleportBuilding> World::FindBuilding_CloseToPosition (uint32_t buildingID, const Coord &pos)
{
    int16_t zoneID = 0;
    boost::shared_ptr<Zone> zone;
    boost::shared_ptr<TeleportBuilding> obj;
    AdjacentZone area = GetAdjacentZones(pos.get_zone());

    for ( uint8_t x = area.min_x; x <= area.max_x; ++x)
    {
        for (uint8_t y = area.min_y; y <= area.max_y; ++y)
        {
            zoneID = (y << 8) + x;
            zone = m_MapMgr->Find(zoneID);

            if (zone)
            {
                obj = zone->FindBuilding(buildingID);

                if (obj)
                    return obj;
            }
        }
    }

    return obj;
}

bool World::InsertPlayer (const boost::shared_ptr<Player> &player)
{
    int16_t zoneID = player->get_position().get_zone();
    boost::shared_ptr<Zone> zone = m_MapMgr->Find(zoneID);

    if (!zone)
        return false;

    boost::mutex::scoped_lock lock(m_player_mutex);
    
    m_PlayerTable.insert(std::make_pair(player->GetUniqueID(),player));

    lock.unlock();

    CreateTimer(player->GetUniqueID());

    AdjacentZone area = GetAdjacentZones(zoneID);

    for (uint8_t x = area.min_x; x <= area.max_x; ++x)
    {
        for (uint8_t y = area.min_y; y <= area.max_y; ++y)
        {
            zoneID = (y << 8) + x;

            m_io_service.post(boost::bind(&World::OnActivateZoneAI,this,zoneID));
        }
    }

    OnJoinArea(player->GetUniqueID(),player->ConnectionID(), area.min_x,area.max_x,area.min_y,area.max_y);

    zone->InsertPlayer(player);

    return true;
}

void World::RemovePlayer (const uint32_t playerID)
{
    boost::mutex::scoped_lock lock(m_player_mutex);

    player_const_iterator i = m_PlayerTable.find(playerID);

    if (i != m_PlayerTable.end())
    {
        int16_t zoneID = i->second->get_position().get_zone();
        boost::shared_ptr<Zone> zone = m_MapMgr->Find(zoneID);

        if (zone)
            zone->RemovePlayer(playerID);
        
        AdjacentZone area = GetAdjacentZones(zoneID);

        m_server->LeaveAdjacentChannels(i->second->ConnectionID(),area.min_x,area.max_x,area.min_y,area.max_y);

        if (zone->Count())
        {
            boost::shared_ptr<OPacket> pkt(new OPacket);

            if (pkt)
            {
                srv_pkt::Despawn(pkt,playerID);
                m_server->SendChannel(pkt,zoneID);
            }
        }

        for (uint8_t x = area.min_x; x <= area.max_x; ++x)
        {
            for (uint8_t y = area.min_y; y <= area.max_y; ++y)
            {
                zoneID = (y << 8) + x;

                m_io_service.post(boost::bind(&World::OnDeactivateZoneAI,this,zoneID));
            }
        }

        m_PlayerTable.erase(playerID);
        RemoveTimer(playerID);
    }
}

boost::shared_ptr<Player> World::FindPlayer_ByID (uint32_t playerID) const
{
    boost::mutex::scoped_lock lock(m_player_mutex);

    boost::shared_ptr<Player> player;

    player_const_iterator iter = m_PlayerTable.find(playerID);

    if (iter != m_PlayerTable.end())
        player = iter->second;

    return player;
}

boost::shared_ptr<Player> World::FindPlayer_ByName (const std::string &name) const
{
    boost::mutex::scoped_lock lock(m_player_mutex);

    boost::shared_ptr<Player> player;

    for (player_const_iterator iter = m_PlayerTable.begin(); iter != m_PlayerTable.end(); ++iter)
    {
        if ( iter->second->get_name() == name)
        {
            player = iter->second;
            break;
        }
    }

    return player;
}

boost::shared_ptr<Player> World::FindPlayer_CloseToPosition (uint32_t playerID, const Coord &pos) const
{
    int16_t zoneID = 0;
    boost::shared_ptr<Player> obj;
    boost::shared_ptr<Zone> zone;
    AdjacentZone area = GetAdjacentZones(pos.get_zone());

    for ( uint8_t x = area.min_x; x <= area.max_x; ++x)
    {
        for (uint8_t y = area.min_y; y <= area.max_y; ++y)
        {
            zoneID = (y << 8) + x;
            zone = m_MapMgr->Find(zoneID);

            if (zone)
            {
                obj = zone->FindPlayer(playerID);

                if (obj)
                    return obj;
            }
        }
    }

    return obj;
}

bool World::InitTaskIdle (const uint32_t playerID)
{
    boost::mutex::scoped_lock lock(m_task_mutex);

    timer_iterator iter = m_tasks.find(playerID);

    if (iter == m_tasks.end())
        return false;

    iter->second->expires_from_now(boost::posix_time::seconds(5));
    iter->second->async_wait(boost::bind(&World::OnTaskIdle,this,playerID,boost::asio::placeholders::error));

    return true;
}

bool World::InitTaskTeleport (const uint32_t playerID, const Coord &pos, const uint32_t time)
{
    boost::mutex::scoped_lock lock(m_task_mutex);

    timer_iterator iter = m_tasks.find(playerID);

    if (iter == m_tasks.end())
        return false;

    iter->second->expires_from_now(boost::posix_time::milliseconds(time));
    iter->second->async_wait(boost::bind(&World::OnTaskTeleport,this,playerID,pos,boost::asio::placeholders::error));

    return true;
}

bool World::InitTaskMove (const uint32_t playerID)
{
    boost::mutex::scoped_lock lock(m_task_mutex);

    timer_iterator iter = m_tasks.find(playerID);

    if (iter == m_tasks.end())
        return false;

    uint32_t dt = 100;

    iter->second->expires_from_now(boost::posix_time::milliseconds(dt));
    iter->second->async_wait(boost::bind(&World::OnTaskMove,this,playerID,dt,boost::asio::placeholders::error));

    return true;
}

bool World::InitTaskMoveTarget (const uint32_t playerID, const uint32_t targetID, const uint32_t skillID)
{
    boost::mutex::scoped_lock lock(m_task_mutex);

    timer_iterator iter = m_tasks.find(playerID);

    if (iter == m_tasks.end())
        return false;

    uint32_t dt = 100;

    iter->second->expires_from_now(boost::posix_time::milliseconds(dt));
    iter->second->async_wait(boost::bind(&World::OnTaskMoveTarget,this,playerID,targetID,skillID,dt,
        boost::asio::placeholders::error));

    return true;
}

bool World::InitTaskMovePickup (const uint32_t playerID, const uint32_t objectID)
{
    boost::mutex::scoped_lock lock(m_task_mutex);

    timer_iterator iter = m_tasks.find(playerID);

    if (iter == m_tasks.end())
        return false;

    uint32_t dt = 100;

    iter->second->expires_from_now(boost::posix_time::milliseconds(dt));
    iter->second->async_wait(boost::bind(&World::OnTaskMovePickup,this,playerID,objectID,dt,
        boost::asio::placeholders::error));

    return true;
}

bool World::InitTaskCastSkill (const uint32_t playerID, const uint32_t targetID, const uint32_t skillID,
    const Coord &pos)
{
    if (!skillID)
        return true;

    boost::shared_ptr<Player> caster = FindPlayer_ByID(playerID);

    if (!caster)
        return false;

    if (caster->IsDead())
        return true;

    if (caster->hasStatus(STATUS_GROUP_RESTRAIN | STATUS_FEAR))
        return true;

    boost::shared_ptr<Skill> skill = m_SkillMgr->find(skillID);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (!skill)
    {
        if (pkt)
        {
            srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_INVALID_SKILL);
            m_server->Send(pkt,caster->ConnectionID());
        }

        return true;
    }

    /// CHECK THAT YOU GOT THE REQUIRED ITEM(S) TO USE THE SKILL
    boost::shared_ptr<Storage> st = caster->get_storage(STORAGE_INVENTORY);

    if (!st)
        return false;

    Item weapon = st->GetItem(Item::WEAPON_SLOT);
    Item shield = st->GetItem(Item::SHIELD_SLOT);

    REQ_WEAPON_TYPE weapon_type;

    if (!weapon.get_id())
        weapon_type = REQ_WEAPON_NONE;
    else
        weapon_type = static_cast<REQ_WEAPON_TYPE>(weapon.get_type());

    if (!skill->is_usable(weapon_type))
    {
        if (pkt)
        {
            srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_INVALID_EQUIPMENT);
            m_server->Send(pkt,caster->ConnectionID());
        }

        return true;
    }

    if (weapon_type != REQ_WEAPON_NONE)
    {
        if (!weapon.get_durability())
        {
            if (pkt)
            {
                srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_WEAPON_BROKEN);
                m_server->Send(pkt,caster->ConnectionID());
            }

            return true;
        }

        /// CHECK RANGED WEAPON AMMO

        if (skill->ActionType() == SKILL_ACTION_ATTACK)
        {
            if (weapon_type == REQ_WEAPON_CH_BOW)
            {
                if (shield.get_type() != ITEM_ARROW || !shield.get_quantity())
                {
                    if (pkt)
                    {
                        srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_INVALID_AMMO);
                        m_server->Send(pkt,caster->ConnectionID());
                    }

                    return true;
                }
            }

            if (weapon_type == REQ_WEAPON_EU_CROSSBOW)
            {
                if (shield.get_type() != ITEM_BOLT || !shield.get_quantity())
                {
                    if (pkt)
                    {
                        srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_INVALID_AMMO);
                        m_server->Send(pkt,caster->ConnectionID());
                    }

                    return true;
                }
            }
        }
    }

    if (skill->require_shield())
    {
        if (shield.get_type() != ITEM_CH_SHIELD && shield.get_type() != ITEM_EU_SHIELD)
        {
            srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_INVALID_EQUIPMENT);
            m_server->Send(pkt,caster->ConnectionID());

            return true;
        }

        if (!shield.get_durability())
        {
            srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_WEAPON_BROKEN);
            m_server->Send(pkt,caster->ConnectionID());

            return true;
        }
    }

    if (caster->mp() < skill->required_mp())
    {
        srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_INVALID_MP);
        m_server->Send(pkt,caster->ConnectionID());
        return true;
    }

    if (skill->required_hp_percent() > (caster->hp()*100)/caster->max_hp())
    {
        srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_INVALID_HP);
        m_server->Send(pkt,caster->ConnectionID());
        return true;
    }

    boost::shared_ptr<NPC> target;

    if (targetID)
    {
        target = GetTarget(targetID,caster->get_position());

        if (!target || target->type() == ET_INTERACTIVE || target->IsDead())
        {
            srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_INVALID_TARGET);
            m_server->Send(pkt,caster->ConnectionID());
            return true;
        }

        /// CHECK IF ITS A PLAYER AND CAN BE INTERACTED WITH

        switch(target->type())
        {
        case ET_PLAYER:
            {
                boost::shared_ptr<Player> ptarget = boost::shared_static_cast<Player,NPC>(target);

                if (!ptarget)
                    return true;

                if (!CanPlayerInteract(caster,ptarget))
                {
                    srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_INVALID_TARGET);
                    m_server->Send(pkt,caster->ConnectionID());
                    return true;
                }

                /// CHECK IF THE TARGET IS MOUNTED ON A TRANSPORT OR NOT

                if (ptarget->TransportID())
                {
                    boost::shared_ptr<Transport> transport = ptarget->GetTransport();

                    if (transport && transport->IsMounted())
                        target = transport;
                }
            }
            break;
        case ET_MOB_HUNTER:
        case ET_MOB_THIEF:
            {
                Job job = caster->get_job();

                bool attackable = false;

                switch(job.get_type())
                {
                case JOB_TRADER:
                case JOB_HUNTER:
                    if (target->type() == ET_MOB_THIEF)
                        attackable = true;
                    break;
                case JOB_THIEF:
                    if (target->type() == ET_MOB_HUNTER)
                        attackable = true;
                    break;
                default:
                    break;
                }

                if (!attackable)
                {
                    return true;
                }
            }
            break;
        default:
            break;
        }

        if (skill->ActionType() == SKILL_ACTION_ATTACK)
        {
            /// CHECK THAT YOUR NOT ATTACKING YOURSELF

            if (playerID == targetID)
                return true;

            /// CHECK IF THE PLAYER IS IN SAFE ZONE.

            if (IsInsideTown(target->get_position()))
            {
                srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_SAFE_ZONE);
                m_server->Send(pkt,caster->ConnectionID());
                return true;
            }

            /// CHECK THAT YOUR NOT ATTACKING A PARTY MEMBER OR TRANSPORT

            boost::shared_ptr<Party> party = caster->get_party();

            if (party)
            {
                uint8_t ret = party->IsMemberOrTransport(targetID);

                if (ret == 1)
                {
                    srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_PARTY_MEMBER);
                    m_server->Send(pkt,caster->ConnectionID());
                    return true;
                }
                else if (ret == 2)
                {
                    srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_PARTY_TRANSPORT);
                    m_server->Send(pkt,caster->ConnectionID());
                    return true;
                }
            }

            if (target->type() == ET_PLAYER)
            {
                /// CHECK THAT YOU ARE ALLOWED TO ENGAGE IN PK

                if (caster->level() < 20)
                {
                    srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_PK_INVALID_LEVEL);
                    m_server->Send(pkt,caster->ConnectionID());
                    return true;
                }

                /// CHECK THAT YOU ARE ALLOWED TO ENGAGE THE OTHER PLAYER IN COMBAT

                if (target->level() < 20)
                {
                    srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_PK_INVALID_LEVEL);
                    m_server->Send(pkt,caster->ConnectionID());
                    return true;
                }
                else
                {
                    /// CHECK THAT UR DAILY PK AINT MAXIMUM
                }
            }
        }
        else
        {
            /// CHECK THAT UR BUFFING//HEALING//POWERING UP A PARTY MEMBER

            boost::shared_ptr<Party> party = caster->get_party();

            if (party && !party->IsMemberOrTransport(targetID))
            {
                syslog(LOG_INFO,"You can only buff a party member or yourself.");
                return true;
            }
        }
    }
    else
        target = caster;

    if (target->get_attribute_state() == ATTRIBUTE_ACTION_INVUNERABLE
            || target->get_attribute_state() == ATTRIBUTE_ACTION_INVISIBLE)
    {
        return true;
    }

    float range = SkillRange(caster,skill,targetID);

    if (!OnMoveInRange(caster,target,skillID,range))
    {
        /// IF THE SKILL IS INSTANT CAST DONT TERMINATE CURRENT TASK (IT SHOULD BE ONLY MOVEMENT RELATED)

        if (skill->init() || skill->delay() || skill->next_delay())
            TerminateTask(caster->GetUniqueID());

        InitTaskExecSkill(playerID,targetID,skillID,pos);
    }

    return true;
}

bool World::InitTaskExecSkill(const uint32_t playerID, const uint32_t targetID, const uint32_t skillID,
    const Coord &pos)
{
    boost::shared_ptr<Player> caster = FindPlayer_ByID(playerID);

    if (!caster || caster->IsDead())
        return false;

    if (caster->hasStatus(STATUS_GROUP_RESTRAIN | STATUS_FEAR))
        return true;

    boost::shared_ptr<Skill> skill = m_SkillMgr->find(skillID);

    if (!skill)
        return true;

    srv_pkt::SKILL_ACTION_TYPE type = srv_pkt::SKILL_ACTION_ATTACK;

    if (skill->ActionType() == SKILL_ACTION_BUFF)
    {
        if (skill->type() == SKT_IMBUE && caster->using_imbue())
            return true;

        Buff buff;
        buff.CastID = caster->GetUniqueID()+skill->id();
        buff.SkillID = skill->id();
        buff.GroupID = skill->group_id();
        buff.Level = skill->level();
        buff.duration = skill->duration();
        buff.Effects = skill->buff_list();

        if (!caster->is_buff_stackable(buff))
        {
#if defined DEBUG
            syslog(LOG_INFO,"InitTaskExecSkill - Cant cast buff bcz a higher buff is active");
#endif
            return true;
        }

        type = srv_pkt::SKILL_ACTION_BUFF;
    }

    boost::shared_ptr<OPacket> pkt(new OPacket);

    srv_pkt::PlayerAction(pkt,srv_pkt::PLAYER_ACTION_INIT);
    m_server->Send(pkt,caster->ConnectionID());

    if (skill->required_mp() > caster->mp())
    {
        srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_INVALID_MP);
        m_server->Send(pkt,caster->ConnectionID());

        pkt.reset(new OPacket);

        srv_pkt::PlayerAction(pkt,srv_pkt::PLAYER_ACTION_ERROR);
        m_server->Send(pkt,caster->ConnectionID());
        return true;
    }

    if (skill->required_hp_percent() > (caster->hp()*100)/caster->max_hp())
    {
        srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_INVALID_HP);
        m_server->Send(pkt,caster->ConnectionID());

        pkt.reset(new OPacket);

        srv_pkt::PlayerAction(pkt,srv_pkt::PLAYER_ACTION_ERROR);
        m_server->Send(pkt,caster->ConnectionID());
        return true;
    }

    BTask task;
    task.task = BG_TASK_COOLDOWN_SKILL;
    task.seconds = skill->cooldown()/1000;
    task.entity = caster;
    task.persistent = false;
    task.arg = skill->id();

    caster->SetCurrentTask(PLAYER_TASK_CAST);

    task.ID = InitBackgroundTask(task);

    caster->AddSkillCooldown(task.ID,skill->id());

    uint32_t attackID = rand();

    bool delayed = false;

    if (skill->ActionType() == SKILL_ACTION_ATTACK)
    {
        if (skill->delay())
            delayed = true;
    }
    else
        delayed = true;

    if (delayed)
    {
        srv_pkt::CastInfo info;
        info.skillID = skill->id();
        info.casterID = playerID;
        info.attackID = attackID;
        info.targetID = targetID;

        pkt.reset(new OPacket);

        srv_pkt::SkillCast_1st(pkt,type,info);
        m_server->SendChannel(pkt,caster->get_position().get_zone());
    }

    /// IF THE SKILL IS INSTANT CASTED RUN IN PARALLEL WITH CURRENT TASK (IT SHOULD BE ONLY MOVEMENT RELATED)
    if (skill->init() || skill->delay() || skill->next_delay())
    {
        boost::mutex::scoped_lock lock(m_task_mutex);

        timer_iterator iter = m_tasks.find(playerID);

        if (iter == m_tasks.end())
            return false;

        iter->second->expires_from_now(boost::posix_time::milliseconds(skill->init()+skill->delay()));
        iter->second->async_wait(boost::bind(&World::OnTaskExecSkill,this,playerID,targetID,skillID,attackID,
            pos,boost::asio::placeholders::error));
    }
    else
    {
        OnTaskExecSkill(playerID,targetID,skillID,attackID,pos,boost::system::error_code());
    }

    return true;
}

bool World::InitTaskWaitSkill (const uint32_t playerID, const uint32_t targetID, const uint32_t msDelay,
        const uint32_t skillID)
{
    boost::mutex::scoped_lock lock(m_task_mutex);

    timer_iterator iter = m_tasks.find(playerID);

    if (iter == m_tasks.end())
        return false;

    boost::shared_ptr<Player> player = FindPlayer_ByID(playerID);

    if (!player)
        return true;

    player->SetCurrentTask(PLAYER_TASK_CAST_WAIT);

    iter->second->expires_from_now(boost::posix_time::milliseconds(msDelay));
    iter->second->async_wait(boost::bind(&World::OnTaskWaitSkill,this,playerID,targetID,skillID,
        boost::asio::placeholders::error));

    return true;
}

bool World::InitTaskPickup (const uint32_t playerID, const uint32_t objectID)
{
    boost::shared_ptr<Player> player = FindPlayer_ByID(playerID);

    if (!player)
        return false;

    if (player->hasStatus(STATUS_GROUP_RESTRAIN))
    {
        player->SetCurrentTask(PLAYER_TASK_IDLE);
        return true;
    }

    boost::shared_ptr<DropableItem> item = FindItem_CloseToPosition(objectID,player->get_position());

    if(!item)
        return true;

    if (distance(player->get_position(),item->get_position()) > 10)
    {
        uint16_t angle = 0;
        Coord pos = RangePosition(player->get_position(),item->get_position(),10,&angle);

        Coord mpos;
        if (!m_MapMgr->ResolveMotion(player->get_position(),item->get_position(),&mpos))
            return true;

        if (mpos != item->get_position())
        {
    #if defined DEBUG
            syslog(LOG_INFO,"World::InitTaskPickup - Unreacheable target.");
    #endif
            return true;
        }

        player->set_angle(angle);

        if (player->GetCurrentTask() != PLAYER_TASK_MOVE_TO_ITEM)
        {
            TerminateTask(player->GetUniqueID());

            player->set_destination(pos);

            player->SetCurrentTask(PLAYER_TASK_MOVE_TO_ITEM);

            if(!InitTaskMovePickup(playerID,objectID))
                return false;
        }
        else
            player->set_destination(pos);

        boost::shared_ptr<OPacket> pkt(new OPacket);

        srv_pkt::Movement(pkt,playerID,pos);
        m_server->SendChannel(pkt,player->get_position().get_zone());
    }
    else
    {
        TerminateTask(player->GetUniqueID());
        InitTaskPickupExec(playerID,objectID);
    }

    return true;
}

void World::InitTaskPickupExec (const uint32_t playerID, const uint32_t objectID)
{
    boost::shared_ptr<Player> player = FindPlayer_ByID(playerID);

    if (!player)
        return;

    boost::shared_ptr<DropableItem> item = FindItem_CloseToPosition(objectID,player->get_position());

    if (item)
    {
        boost::shared_ptr<Storage> st;

        if (!item->m_OwnerAlias.empty())
        {
            if (!player->IsJobbing())
                return;

            Job job = player->get_job();

            bool valid = false;

            switch(job.get_type())
            {
            case JOB_TRADER:
                if (item->m_OwnerAlias == job.get_alias())
                    valid = true;
                break;
            case JOB_THIEF:
                valid = true;
                break;
            default:
                break;
            }

            if (!valid || !player->TransportID())
                return;

            boost::shared_ptr<Transport> transport = player->GetTransport();

            if (!transport)
                return;

            st = transport->GetStorage();
        }
        else
        {
            st = player->get_storage(STORAGE_INVENTORY);
        }

        if (!st)
            return;

        boost::shared_ptr<OPacket> pkt(new OPacket);

        uint8_t pos = st->get_empty_position(13);

        if (pos == 255 && item->m_Item.get_type() != ITEM_GOLD)
        {
#if defined DEBUG
            syslog(LOG_INFO,"InitTaskPickupExec - Inventory full.");
#endif
            return;
        }

        srv_pkt::PlayerAction(pkt,srv_pkt::PLAYER_ACTION_INIT);
        m_server->Send(pkt,player->ConnectionID());

        pkt.reset(new OPacket);

        srv_pkt::PlayerPickup(pkt,player->GetUniqueID());
        m_server->SendChannel(pkt,player->get_position().get_zone());

        RemoveItem(item->GetUniqueID(),item->get_position());

        pkt.reset(new OPacket);

        srv_pkt::PlayerAction(pkt,srv_pkt::PLAYER_ACTION_END);
        m_server->Send(pkt,player->ConnectionID());

        if (item->m_Item.get_type() == ITEM_GOLD)
        {
            player->gain_gold(item->m_GoldAmount);
            item->m_Item.set_position(0xFE);	//GOLD POSITION!!
        }
        else
        {
            item->m_Item.set_place(PLACE_INVENTORY);

            item->m_Item.set_position(pos);

            st->InsertItem(item->m_Item);
        }

        pkt.reset(new OPacket);

        srv_pkt::Item(pkt,srv_pkt::ITEM_MOVE_PICKUP,item->m_Item);
        m_server->Send(pkt,player->ConnectionID());

        if (!item->m_OwnerAlias.empty())
        {
            boost::shared_ptr<Transport> transport = player->GetTransport();

            if (!transport)
                return;

            if (!transport->SpawnTaskID())
            {
                BTask task;
                task.task = BG_TASK_SPAWN_JOB_NPC;
                task.seconds = 30;
                task.entity = transport;
                task.arg = player->get_job().get_type();

                task.ID = InitBackgroundTask(task);

                transport->SpawnTaskID(task.ID);
            }

            transport->CalculateTradeScale();

            pkt.reset(new OPacket);

            srv_pkt::JobTradeScale(pkt,transport->TradeScale());
            m_server->Send(pkt,player->ConnectionID());
        }
    }

    m_io_service.post(boost::bind(&World::InitTaskIdle,this,playerID));
}

bool World::InitTaskMoveItem (const uint32_t playerID, const uint8_t src, const uint8_t dest, const uint16_t quantity,
    const uint8_t time)
{
    boost::mutex::scoped_lock lock(m_task_mutex);

    timer_iterator iter = m_tasks.find(playerID);

    if (iter == m_tasks.end())
        return false;

    iter->second->expires_from_now(boost::posix_time::seconds(time ? 1 : 0));
    iter->second->async_wait(boost::bind(&World::OnTaskMoveItem,this,playerID,src,dest,quantity,time,
        boost::asio::placeholders::error));

    return true;
}

bool World::InitTaskAlchemy (const uint32_t playerID, uint8_t ipos, uint8_t epos, uint8_t lpos, bool advanced)
{
    enum { ALCHEMY_OPERATION_TIMEOUT = 3 };

    boost::mutex::scoped_lock lock(m_task_mutex);

    timer_iterator iter = m_tasks.find(playerID);

    if (iter == m_tasks.end())
        return false;

    iter->second->expires_from_now(boost::posix_time::seconds(ALCHEMY_OPERATION_TIMEOUT));
    iter->second->async_wait(boost::bind(&World::OnTaskAlchemy,this,playerID,ipos,epos,lpos,advanced,
        boost::asio::placeholders::error));


    return true;
}

void World::InitTaskAI (const boost::shared_ptr<NPC> &npc)
{
    if (!npc)
        return;

    if ( rand() % 100 < 50)
    {
        m_io_service.post(boost::bind(&World::InitTaskIdleNPC,this,npc));
    }
    else
    {
        if (npc->hasStatus(STATUS_STUN | STATUS_FROZE | STATUS_PETRIFY | STATUS_SLEEP | STATUS_BIND) ||
            !npc->running_speed())
        {
            m_io_service.post(boost::bind(&World::InitTaskIdleNPC,this,npc));
            return;
        }

        Coord pos = npc->get_position();

        boost::shared_ptr<Zone> zone = m_MapMgr->Find(pos.get_zone());

        if (zone)
        {
            /// ONLY MOVE INSIDE THE SAME ZONE
            int unit = rand()%1000+rand()%50 - 500;

            pos.set_x(pos.get_x() + unit);

            unit = rand()%1000+rand()%50 - 500;

            pos.set_y(pos.get_y() + unit);

            if (pos.get_x() > zone->Length())
                pos.set_x(zone->Length());

            if (pos.get_x() < 0)
                pos.set_x(0);

            if (pos.get_y() > zone->Width())
                pos.set_y(zone->Width());

            if (pos.get_y() < 0)
                pos.set_y(0);

            Coord dest;
            if (m_MapMgr->ResolveMotion(npc->get_position(),pos,&dest))
            {
                npc->set_destination(dest);

                if (zone->Count())
                {
                    boost::shared_ptr<OPacket> pkt(new OPacket);

                    if (pkt)
                    {
                        srv_pkt::Movement(pkt,npc->GetUniqueID(),pos);
                        m_server->SendChannel(pkt,npc->get_position().get_zone());
                    }
                }

                m_io_service.post(boost::bind(&World::InitTaskMoveNPC,this,npc));
            }
            else
                m_io_service.post(boost::bind(&World::InitTaskIdleNPC,this,npc));
        }
    }
}

void World::InitTaskIdleNPC (const boost::shared_ptr<NPC> &npc)
{
    if (!npc)
        return;

    boost::mutex::scoped_lock lock(m_task_mutex);

    timer_iterator iter = m_tasks.find(npc->GetUniqueID());

    if (iter != m_tasks.end())
    {
        iter->second->expires_from_now(boost::posix_time::seconds(10));
        iter->second->async_wait(boost::bind(&World::OnTaskIdleNPC,this,npc,boost::asio::placeholders::error));
    }
}

void World::InitTaskCastNPC (const boost::shared_ptr<NPC> &npc)
{
    if (!npc || npc->IsDead())
        return;

    if (npc->hasStatus(STATUS_GROUP_RESTRAIN | STATUS_FEAR))
    {
        npc->SetCurrentTask(MOB_TASK_IDLE);
        m_io_service.post(boost::bind(&World::InitTaskIdleNPC,this,npc));
        return;
    }

    boost::shared_ptr<Mob> mob = boost::shared_dynamic_cast<Mob,NPC>(npc);

    if (!mob)
        return;

    uint32_t skillID = mob->get_next_skill();
    boost::shared_ptr<Skill> skill = m_SkillMgr->find(skillID);

    if (!skill)
    {
        npc->SetCurrentTask(MOB_TASK_IDLE);
        m_io_service.post(boost::bind(&World::InitTaskIdleNPC,this,npc));
        return;
    }

    boost::shared_ptr<NPC> target;

    if (skill->ActionType() == SKILL_ACTION_ATTACK)
    {
        std::map<uint32_t,uint32_t> dlist = mob->get_attack_list();

        /// FIND AND CHECK THAT TARGET IS IN RANGE.
        for (std::map<uint32_t,uint32_t>::const_iterator piter = dlist.begin(); piter != dlist.end(); ++piter)
        {
            target = GetTarget(piter->first,npc->get_position());

            if (target)
            {
                if (target->type() == ET_PLAYER)
                {
                    boost::shared_ptr<Player> ptarget = boost::shared_static_cast<Player,NPC>(target);

                    if (ptarget)
                    {
                        bool attackable = true;

                        if (ptarget->IsJobbing())
                        {
                            Job job = ptarget->get_job();

                            switch(job.get_type())
                            {
                            case JOB_HUNTER:
                            case JOB_TRADER:
                                if (npc->type() == ET_MOB_HUNTER)
                                    attackable = false;
                                break;
                            case JOB_THIEF:
                                if (npc->type() == ET_MOB_THIEF)
                                    attackable = false;
                                break;
                            default:
                                break;
                            }
                        }
                        else
                        {
                            if (npc->type() == ET_MOB_HUNTER || npc->type() == ET_MOB_THIEF)
                                attackable = false;
                        }

                        if (attackable && ptarget->TransportID())
                        {
                            boost::shared_ptr<Transport> transport = ptarget->GetTransport();

                            if (transport && transport->IsMounted())
                                target = transport;
                        }
                        else
                            continue;
                    }
                }

                if (!target->IsDead() && !IsInsideTown(target->get_position()) &&
                    target->get_attribute_state() != ATTRIBUTE_ACTION_INVUNERABLE &&
                    target->get_attribute_state() != ATTRIBUTE_ACTION_INVISIBLE)
                    break;
            }
        }
    }
    else
        target = npc;

    if (!target || target->IsDead())
    {
        npc->SetCurrentTask(MOB_TASK_IDLE);
        m_io_service.post(boost::bind(&World::InitTaskIdleNPC,this,npc));
        return;
    }

    npc->select_object(target->GetUniqueID());

    uint16_t angle = 0;
    float range = SkillRange(npc,skill,target->GetUniqueID());

    Coord dest = RangePosition(npc->get_position(),target->get_position(),range,&angle);

    Coord mpos;
    if (!m_MapMgr->ResolveMotion(npc->get_position(),target->get_position(),&mpos))
        return;

    if (mpos != target->get_position())
    {
#if defined DEBUG
        syslog(LOG_INFO,"InitTaskCastNPC - Unreacheable target.");
#endif
        return;
    }

    dest = mpos;

    npc->set_angle(angle);

    if (dest.get_zone() && distance(npc->get_position(),dest) > 2)
    {
        if (npc->running_speed() || !npc->hasWall())
        {
            npc->set_destination(dest);
            npc->SetCurrentTask(MOB_TASK_MOVE_TO_TARGET);

            m_io_service.post(boost::bind(&World::InitTaskMoveNPC,this,npc));

            boost::shared_ptr<OPacket> pkt(new OPacket);

            if (pkt)
            {
                srv_pkt::Movement(pkt,npc->GetUniqueID(),dest);
                m_server->SendChannel(pkt,npc->get_position().get_zone());
            }
        }
        else
        {
            npc->SetCurrentTask(MOB_TASK_IDLE);
            m_io_service.post(boost::bind(&World::InitTaskIdleNPC,this,npc));
        }
    }
    else
        m_io_service.post(boost::bind(&World::InitTaskExecCastNPC,this,npc,0));

}

void World::InitTaskExecCastNPC (const boost::shared_ptr<NPC> &npc, const uint32_t skillID)
{
    if (!npc || npc->IsDead())
        return;

    if (npc->hasStatus(STATUS_GROUP_RESTRAIN | STATUS_FEAR))
    {
        npc->SetCurrentTask(MOB_TASK_IDLE);
        m_io_service.post(boost::bind(&World::InitTaskIdleNPC,this,npc));
        return;
    }

    uint32_t targetID = npc->get_selected_object();

    boost::shared_ptr<NPC> target = GetTarget(targetID,npc->get_position());

    if (!target)
    {
        npc->SetCurrentTask(MOB_TASK_IDLE);
        m_io_service.post(boost::bind(&World::InitTaskIdleNPC,this,npc));
        return;
    }

    boost::shared_ptr<Skill> skill = m_SkillMgr->find( skillID ? skillID : npc->GetQueueSkill());

    srv_pkt::SKILL_ACTION_TYPE type = srv_pkt::SKILL_ACTION_ATTACK;

    if (skill->ActionType() == SKILL_ACTION_BUFF)
    {
        if (skill->type() == SKT_IMBUE && npc->using_imbue())
        {
            npc->SetCurrentTask(MOB_TASK_IDLE);
            m_io_service.post(boost::bind(&World::InitTaskIdleNPC,this,npc));
            return;
        }

        Buff buff;
        buff.CastID = npc->GetUniqueID()+skill->id();
        buff.SkillID = skill->id();
        buff.GroupID = skill->group_id();
        buff.Level = skill->level();
        buff.duration = skill->duration();
        buff.Effects = skill->buff_list();

        if (!npc->is_buff_stackable(buff))
        {
#if defined DEBUG
            syslog(LOG_INFO,"Cant cast buff bcz a higher buff is active");
#endif
            npc->SetCurrentTask(MOB_TASK_IDLE);
            m_io_service.post(boost::bind(&World::InitTaskIdleNPC,this,npc));
            return;
        }

        type = srv_pkt::SKILL_ACTION_BUFF;
    }

    BTask task;
    task.task = BG_TASK_COOLDOWN_SKILL;
    task.seconds = skill->cooldown()/1000;
    task.entity = npc;
    task.persistent = false;
    task.arg = skill->id();

    npc->SetCurrentTask(MOB_TASK_CAST);

    task.ID = InitBackgroundTask(task);

    npc->AddSkillCooldown(task.ID,skill->id());

    uint32_t attackID = rand();

    bool delayed = false;

    if (skill->ActionType() == SKILL_ACTION_ATTACK)
    {
        if (skill->delay())
            delayed = true;
    }
    else
    {
        delayed = true;
        targetID = 0;
    }

    if (delayed)
    {
        srv_pkt::CastInfo info;
        info.skillID = skill->id();
        info.casterID = npc->GetUniqueID();
        info.attackID = attackID;
        info.targetID = targetID;

        boost::shared_ptr<OPacket> pkt(new OPacket);

        srv_pkt::SkillCast_1st(pkt,type,info);
        m_server->SendChannel(pkt,npc->get_position().get_zone());
    }

    boost::mutex::scoped_lock lock(m_task_mutex);

    timer_iterator iter = m_tasks.find(npc->GetUniqueID());

    if (iter != m_tasks.end())
    {
        iter->second->expires_from_now(boost::posix_time::milliseconds(skill->init()+skill->delay()));
        iter->second->async_wait(boost::bind(&World::OnTaskExecCastNPC,this,npc,attackID,
            boost::asio::placeholders::error));
    }
}

void World::InitTaskWaitCastNPC (const boost::shared_ptr<NPC> &npc, const uint32_t msDelay,
    const uint32_t skillID)
{
    if (!npc)
        return;

    boost::mutex::scoped_lock lock(m_task_mutex);

    timer_iterator iter = m_tasks.find(npc->GetUniqueID());

    if (iter != m_tasks.end())
    {
        iter->second->expires_from_now(boost::posix_time::milliseconds(msDelay));
        iter->second->async_wait(boost::bind(&World::OnTaskWaitCastNPC,this,npc,skillID,
            boost::asio::placeholders::error));
    }
}

void World::InitTaskMoveNPC (const boost::shared_ptr<NPC> &npc)
{
    if (!npc)
        return;

    boost::mutex::scoped_lock lock(m_task_mutex);

    timer_iterator iter = m_tasks.find(npc->GetUniqueID());

    if (iter != m_tasks.end())
    {
        uint32_t dt = 250;

        iter->second->expires_from_now(boost::posix_time::milliseconds(dt));
        iter->second->async_wait(boost::bind(&World::OnTaskMoveNPC,this,npc,dt,
            boost::asio::placeholders::error));
    }
}

void World::InitTaskTraceNPC (const boost::shared_ptr<NPC> &npc, const uint32_t playerID)
{
    if (!npc)
        return;

    boost::mutex::scoped_lock lock(m_task_mutex);

    timer_iterator iter = m_tasks.find(npc->GetUniqueID());

    if (iter != m_tasks.end())
    {
        iter->second->expires_from_now(boost::posix_time::seconds(1));
        iter->second->async_wait(boost::bind(&World::OnTaskTraceNPC,this,npc,playerID,
            boost::asio::placeholders::error));
    }
}

void World::InitTaskTraceMoveNPC (boost::shared_ptr<NPC> &npc, const uint32_t playerID)
{
    if (!npc)
        return;

    boost::mutex::scoped_lock lock(m_task_mutex);

    timer_iterator iter = m_tasks.find(npc->GetUniqueID());

    if (iter != m_tasks.end())
    {
        uint32_t dt = 100;

        iter->second->expires_from_now(boost::posix_time::milliseconds(dt));
        iter->second->async_wait(boost::bind(&World::OnTaskTraceMoveNPC,this,npc,playerID,dt,
            boost::asio::placeholders::error));
    }
}

bool World::InitTask_ItemDespawn (const uint32_t itemID, const int16_t zone, const uint32_t time)
{
    boost::mutex::scoped_lock lock(m_task_mutex);

    timer_iterator iter = m_tasks.find(itemID);

    if (iter == m_tasks.end())
        return false;

    iter->second->expires_from_now(boost::posix_time::milliseconds(time));
    iter->second->async_wait(boost::bind(&World::OnTaskDespawn,this,itemID,zone,
        false,boost::asio::placeholders::error));

    return true;
}

uint32_t World::TerminateTask (const uint32_t timerID)
{
    boost::mutex::scoped_lock lock(m_task_mutex);

    timer_iterator iter = m_tasks.find(timerID);

    uint32_t tm = 0;

    if (iter != m_tasks.end())
    {
        tm = iter->second->expires_from_now().total_milliseconds();
        iter->second->cancel();
    }

    return tm;
}

uint32_t World::InitBackgroundTask (BTask &task, bool changeID)
{
    boost::mutex::scoped_lock lock(m_bg_task_mutex);

    if (changeID)
        task.ID = ++m_bg_counter;

    if (m_bg_tasks.empty())
    {
        m_bg_task_timer.expires_from_now(boost::posix_time::seconds(1));
        m_bg_task_timer.async_wait(boost::bind(&World::OnBackgroundTask,this,boost::asio::placeholders::error));
    }

    m_bg_tasks.insert(std::make_pair(task.ID,task));

    return m_bg_counter;
}

uint32_t World::GetBackgroundTaskTime (const uint32_t taskID)
{
    uint32_t time = 0;

    boost::mutex::scoped_lock lock(m_bg_task_mutex);

    boost::unordered_map<uint32_t,BTask>::const_iterator iter = m_bg_tasks.find(taskID);

    if (iter != m_bg_tasks.end())
        time = iter->second.seconds;

    return time;
}

void World::TerminateBackgroundTask (const uint32_t taskID)
{
    boost::mutex::scoped_lock lock(m_bg_task_mutex);

    m_bg_tasks.erase(taskID);
}

void World::InitStateTask(const STask &task)
{
    boost::mutex::scoped_lock lock(m_state_task_mutex);

    if (m_state_tasks.empty())
    {
        m_state_task_timer.expires_from_now(boost::posix_time::seconds(1));
        m_state_task_timer.async_wait(boost::bind(&World::OnStateTask,this,boost::asio::placeholders::error));
    }

    m_state_tasks.insert(std::make_pair(task.playerID,task));
}

void World::TerminateStateTask(const uint32_t playerID)
{
    boost::mutex::scoped_lock lock(m_state_task_mutex);

    m_state_tasks.erase(playerID);
}

void World::Run ()
{
    for (int i = 0; i < m_thread_count; ++i)
        m_threads.create_thread(boost::bind(&boost::asio::io_service::run,&m_io_service));
}

void World::Stop ()
{
    m_stop_flag = true;

    m_io_service.stop();
    m_threads.join_all();
}

const std::string& World::GetLastError() const
{
    return m_last_error;
}

void World::Configure (const std::map<std::string,std::string> &config_entries)
{
    typedef std::map<std::string,std::string>::const_iterator const_iterator;

    for (const_iterator it = config_entries.begin(); it != config_entries.end(); ++it)
    {
        if (it->first == "DatabaseName")
            m_db = it->second;
        else if (it->first == "DatabaseIP")
            m_db_host = it->second;
        else if (it->first == "DatabaseUser")
            m_db_usr = it->second;
        else if (it->first == "DatabasePassword")
            m_db_pw = it->second;
        else if (it->first == "WorldThreads")
            m_thread_count = atoi(it->second.c_str());
        else if (it->first == "WorldSpawnCount")
            m_spawn_count = atoi(it->second.c_str());
        else if (it->first == "WorldLevelCap")
            m_max_cap = atoi(it->second.c_str());
        else if (it->first == "WorldAllowStallNetwork")
            m_market = static_cast<bool>(atoi(it->second.c_str()));
        else if (it->first == "WorldAllowChinese")
            m_allow_ch = static_cast<bool>(atoi(it->second.c_str()));
        else if (it->first == "WorldAllowEuropean")
            m_allow_eu = static_cast<bool>(atoi(it->second.c_str()));
        else if (it->first == "MediaPath")
            m_path_media = it->second;
        else if (it->first == "DataPath")
            m_path_data = it->second;
    }

    if (*m_path_data.rbegin() == '/')
        m_path_data.erase(m_path_data.end()-1);

    if (!m_thread_count)
        m_thread_count = 1;
}

Teleport World::FindTeleport (uint16_t ID) const
{
    Teleport tl;
    tl.ID = 0;

    teleport_const_iterator i = m_TeleportChart.find(ID);

    if (i != m_TeleportChart.end())
        tl = i->second;

    return tl;
}

TeleportOptional World::FindTeleportOptional (uint16_t ID) const
{
    TeleportOptional tl;
    tl.id = 0;

    boost::unordered_map<uint16_t,TeleportOptional>::const_iterator i;
    i = m_TeleportOptionalChart.find(ID);

    if (i != m_TeleportOptionalChart.end())
        tl = i->second;

    return tl;
}

TeleportLink World::FindTeleportLink (uint16_t ID, uint32_t dst) const
{
    TeleportLink tl;
    tl.src = 0;
    tl.dest = 0;

    std::multimap<uint16_t,TeleportLink>::const_iterator i = m_TeleportLinkChart.find(ID);

    if (i == m_TeleportLinkChart.end())
        return tl;

    std::multimap<uint16_t,TeleportLink>::const_iterator upper = m_TeleportLinkChart.upper_bound(ID);

    for ( ; i != upper; ++i)
        if (i->second.dest == dst)
            tl = i->second;

    return tl;
}

uint8_t World::GetMaxCap () const
{
    return MAX_CAP;
}

int32_t World::GetRequiredSP (const uint8_t lv) const
{
    boost::unordered_map<uint8_t,LevelProperty>::const_iterator iter = m_XPChart.find(lv);

    if (iter == m_XPChart.end())
        return -1;

    return iter->second.RequiredSP;
}

uint16_t World::GetShopItemStack (const uint32_t itemID) const
{
    boost::unordered_map<uint32_t,uint16_t>::const_iterator iter = m_ShopItemStacks.find(itemID);

    if (iter == m_ShopItemStacks.end())
        return 0;

    return iter->second;
}

LevelProperty World::GetXPInformation (const uint8_t level) const
{
    LevelProperty info;
    info.Level = 0;
    info.RequiredJobXP = 0;
    info.RequiredSP = 0;
    info.RequiredXP = 0;

    boost::unordered_map<uint8_t,LevelProperty>::const_iterator iter = m_XPChart.find(level);

    if (iter != m_XPChart.end())
        info = iter->second;

    return info;
}

MagicOptionProperty World::GetMagicOptionProperty (const std::string &name, const uint8_t degree) const
{
    MagicOptionProperty prop;
    prop.ID = 0;

    std::vector<MagicOptionProperty>::const_iterator i;
    for (i = m_MagicOptionChart.begin(); i != m_MagicOptionChart.end(); ++i)
    {
        if (i->Degree == degree && i->Name.find(name,0) != std::string::npos)
        {
            prop = *i;
            break;
        }
    }

    return prop;
}

bool World::IsEuropeanAllowed () const
{
    return m_allow_eu;
}

bool World::IsChineseAllowed () const
{
    return m_allow_ch;
}

void World::CreateTimer (const uint32_t timerID)
{
    boost::mutex::scoped_lock lock(m_task_mutex);

    boost::asio::deadline_timer *timer = new boost::asio::deadline_timer(m_io_service);
    m_tasks.insert(std::make_pair(timerID,timer));
}

void World::RemoveTimer (const uint32_t timerID)
{
    boost::mutex::scoped_lock lock(m_task_mutex);

    timer_iterator it = m_tasks.find(timerID);

    if (it != m_tasks.end())
    {
        delete it->second;
        m_tasks.erase(it);
    }
}

bool World::OnMovePlayer (boost::shared_ptr<Player> &player, uint32_t dt, bool with_transport)
{
    Coord curpos = player->get_position();
    Coord nextpos = player->get_destination();

    if (!nextpos.get_zone())
        return true;

    boost::shared_ptr<NPC> entity;

    if (with_transport)
    {
        boost::shared_ptr<NPC> transport = player->GetTransport();

        if (transport)
            entity = transport;
        else
        {
            with_transport = false;
            entity = player;
        }
    }
    else
        entity = player;

    if (!entity)
        return true;

    float speed = entity->current_speed();

    speed = speed * dt/1000;

    int16_t zoneID = curpos.get_zone();

    boost::shared_ptr<Zone> dzone;
    boost::shared_ptr<Zone> zone = m_MapMgr->Find(zoneID);

    if (!zone)
    {
        player->set_destination(Coord());
        return true;
    }

    float angle = player->get_angle()/10430.38208;  /// CONVERT ANGLE TO RADIANS
    float vx = speed*cos(angle);
    float vy = speed*sin(angle);

    float x = curpos.get_x()+vx;
    float y = curpos.get_y()+vy;

    /// CHECK THAT THE MOVEMENT IS NOT BEYOND THE DESTINATION POINT
    if (zoneID == nextpos.get_zone())
    {
        if (vx > 0)
        {
            if (x > nextpos.get_x())
                x = nextpos.get_x();
        }
        else
        {
            if (x < nextpos.get_x())
                x = nextpos.get_x();
        }

        if (vy > 0)
        {
            if (y > nextpos.get_y())
                y = nextpos.get_y();
        }
        else
        {
            if (y < nextpos.get_y())
                y = nextpos.get_y();
        }
    }

    Coord dest;
    if (!m_MapMgr->ResolveMotion(curpos,Coord(x,y,curpos.get_z(),zoneID),&dest))
        return false;

    player->set_position(dest);

    if (with_transport)
        player->m_transport->set_position(dest);

    if (dest.get_zone() != curpos.get_zone())
    {
        /// CALCULATE THE X,Y OF THE CENTER ZONE
        uint8_t xz = (zoneID & 0xFF);
        uint8_t yz = (zoneID >> 8);

        /// CALCULATE ADJACENT AREAS,ZONE AND POSITION
        AdjacentZone adjx,adjy;

        if (x > zone->Length())
        {
            x -= zone->Length();
            ++zoneID;

            adjx.min_x = adjx.max_x = xz + 2;
            adjx.min_y = yz - 1;
            adjx.max_y = yz + 1;

        }
        else if ( x < 0)
        {
            x += zone->Length();
            --zoneID;

            adjx.min_x = adjx.max_x = xz - 2;
            adjx.min_y = yz - 1;
            adjx.max_y = yz + 1;
        }

        /// CALCULATE THE X,Y OF THE CENTER
        xz = (zoneID & 0xFF);
        yz = (zoneID >> 8);

        if (y > zone->Length())
        {
            y -= zone->Length();
            zoneID += 256;

            adjy.min_y = adjy.max_y = yz + 2;
            adjy.min_x = xz - 1;
            adjy.max_x = xz + 1;
        }
        else if ( y < 0)
        {
            y += zone->Length();
            zoneID -= 256;

            adjy.min_y = adjy.max_y = yz - 2;
            adjy.min_x = xz - 1;
            adjy.max_x = xz + 1;
        }

        /// INSERT PLAYER INTO THE NEW ZONE
        dzone = m_MapMgr->Find(zoneID);

        if (!dzone)
        {
            player->set_destination(Coord());
            return true;
        }

        zone->RemovePlayer(player->GetUniqueID());

        dzone->InsertPlayer(player);

        if (with_transport)
        {
            zone->RemoveNPC(player->m_transport->GetUniqueID());

            dzone->InsertNPC(player->m_transport);
        }

        /// REGISTER TO ADJACENT ZONES

        if (x > zone->maxX())
        {
            m_io_service.post(boost::bind(&World::OnJoinArea,this,player->GetUniqueID(),player->ConnectionID(),
                adjx.min_x,adjx.max_x,adjx.min_y,adjx.max_y));

            if (with_transport)
            {
                m_io_service.post(boost::bind(&World::OnJoinArea,this,player->m_transport,adjx.min_x,adjx.max_x,
                    adjx.min_y,adjx.max_y));
            }

            adjx.min_x = adjx.max_x = xz - 1;

            m_io_service.post(boost::bind(&World::OnLeaveArea,this,player->GetUniqueID(),player->ConnectionID(),
                adjx.min_x,adjx.max_x,adjx.min_y,adjx.max_y));

            if (with_transport)
            {
                m_io_service.post(boost::bind(&World::OnLeaveArea,this,player->m_transport->GetUniqueID(),adjx.min_x,
                    adjx.max_x,adjx.min_y,adjx.max_y));
            }

        }
        else if ( x < zone->minX())
        {
            m_io_service.post(boost::bind(&World::OnJoinArea,this,player->GetUniqueID(),player->ConnectionID(),
                adjx.min_x,adjx.max_x,adjx.min_y,adjx.max_y));

            if (with_transport)
            {
                m_io_service.post(boost::bind(&World::OnJoinArea,this,player->m_transport,adjx.min_x,adjx.max_x,
                    adjx.min_y,adjx.max_y));
            }

            adjx.min_x = adjx.max_x = xz + 1;

            m_io_service.post(boost::bind(&World::OnLeaveArea,this,player->GetUniqueID(),player->ConnectionID(),
                adjx.min_x,adjx.max_x,adjx.min_y,adjx.max_y));

            if (with_transport)
            {
                m_io_service.post(boost::bind(&World::OnLeaveArea,this,player->m_transport->GetUniqueID(),adjx.min_x,
                    adjx.max_x,adjx.min_y,adjx.max_y));
            }
        }

        if (y > zone->maxZ())
        {
            m_io_service.post(boost::bind(&World::OnJoinArea,this,player->GetUniqueID(),player->ConnectionID(),
                adjy.min_x,adjy.max_x,adjy.min_y,adjy.max_y));

            if (with_transport)
            {
                m_io_service.post(boost::bind(&World::OnJoinArea,this,player->m_transport,adjy.min_x,adjy.max_x,
                    adjy.min_y,adjy.max_y));
            }

            adjy.min_y = adjy.max_y = yz - 1;

            m_io_service.post(boost::bind(&World::OnLeaveArea,this,player->GetUniqueID(),player->ConnectionID(),
                adjy.min_x,adjy.max_x,adjy.min_y,adjy.max_y));

            if (with_transport)
            {
                m_io_service.post(boost::bind(&World::OnLeaveArea,this,player->m_transport->GetUniqueID(),adjy.min_x,
                    adjy.max_x,adjy.min_y,adjy.max_y));
            }
        }
        else if ( y < zone->minZ())
        {
            m_io_service.post(boost::bind(&World::OnJoinArea,this,player->GetUniqueID(),player->ConnectionID(),
                adjy.min_x,adjy.max_x,adjy.min_y,adjy.max_y));

            if (with_transport)
            {
                m_io_service.post(boost::bind(&World::OnJoinArea,this,player->m_transport,adjy.min_x,adjy.max_x,
                    adjy.min_y,adjy.max_y));
            }

            adjy.min_y = adjy.max_y = yz + 1;

            m_io_service.post(boost::bind(&World::OnLeaveArea,this,player->GetUniqueID(),player->ConnectionID(),
                adjy.min_x,adjy.max_x,adjy.min_y,adjy.max_y));

            if (with_transport)
            {
                m_io_service.post(boost::bind(&World::OnLeaveArea,this,player->m_transport->GetUniqueID(),adjy.min_x,
                    adjy.max_x,adjy.min_y,adjy.max_y));
            }
        }
    }

    if (dest == nextpos)
    {
        player->set_destination(Coord());

        if (with_transport)
            player->m_transport->set_destination(Coord());

#if defined DEBUG
        syslog(LOG_INFO,"Player Finished Moving.");
#endif
    }

    return dest == nextpos;

//    if (zoneID == nextpos.get_zone())
//    {
//        if (vx > 0)
//        {
//            if (x > nextpos.get_x())
//                x = nextpos.get_x();
//        }
//        else
//        {
//            if (x < nextpos.get_x())
//                x = nextpos.get_x();
//        }
//
//        if (vy > 0)
//        {
//            if (y > nextpos.get_y())
//                y = nextpos.get_y();
//        }
//        else
//        {
//            if (y < nextpos.get_y())
//                y = nextpos.get_y();
//        }
//
//        dzone = zone;
//
//        curpos.set_zone(zoneID);
//        curpos.set_x(x);
//        curpos.set_y(y);
//        curpos.set_z(dzone->getHeightAt(x,y,curpos.get_z()));
//
//        player->set_position(curpos);
//
//        if (with_transport)
//            player->m_transport->set_position(curpos);
//    }
//    else
//    {
//        /// CALCULATE THE X,Y OF THE CENTER ZONE
//        uint8_t xz = (zoneID & 0xFF);
//        uint8_t yz = (zoneID >> 8);
//
//        /// CALCULATE ADJACENT AREAS,ZONE AND POSITION
//        AdjacentZone adjx,adjy;
//
//        if (x > zone->Length())
//        {
//            x -= zone->Length();
//            ++zoneID;
//
//            adjx.min_x = adjx.max_x = xz + 2;
//            adjx.min_y = yz - 1;
//            adjx.max_y = yz + 1;
//
//        }
//        else if ( x < 0)
//        {
//            x += zone->Length();
//            --zoneID;
//
//            adjx.min_x = adjx.max_x = xz - 2;
//            adjx.min_y = yz - 1;
//            adjx.max_y = yz + 1;
//        }
//
//        /// CALCULATE THE X,Y OF THE CENTER
//        xz = (zoneID & 0xFF);
//        yz = (zoneID >> 8);
//
//        if (y > zone->Length())
//        {
//            y -= zone->Length();
//            zoneID += 256;
//
//            adjy.min_y = adjy.max_y = yz + 2;
//            adjy.min_x = xz - 1;
//            adjy.max_x = xz + 1;
//        }
//        else if ( y < 0)
//        {
//            y += zone->Length();
//            zoneID -= 256;
//
//            adjy.min_y = adjy.max_y = yz - 2;
//            adjy.min_x = xz - 1;
//            adjy.max_x = xz + 1;
//        }
//
//        /// INSERT PLAYER INTO THE NEW ZONE
//        dzone = m_MapMgr->Find(zoneID);
//
//        if (!dzone)
//        {
//            player->set_destination(Coord());
//            return true;
//        }
//
//        zone->RemovePlayer(player->GetUniqueID());
//
//        curpos.set_zone(zoneID);
//        curpos.set_x(x);
//        curpos.set_y(y);
//        curpos.set_z(dzone->getHeightAt(x,y,curpos.get_z()));
//
//        player->set_position(curpos);
//
//        dzone->InsertPlayer(player);
//
//        if (with_transport)
//        {
//            zone->RemoveNPC(player->m_transport->GetUniqueID());
//
//            player->m_transport->set_position(curpos);
//
//            dzone->InsertNPC(player->m_transport);
//        }
//
//        /// REGISTER TO ADJACENT ZONES
//
//        if (x > zone->maxX())
//        {
//            m_io_service.post(boost::bind(&World::OnJoinArea,this,player->GetUniqueID(),player->ConnectionID(),
//                adjx.min_x,adjx.max_x,adjx.min_y,adjx.max_y));
//
//            if (with_transport)
//            {
//                m_io_service.post(boost::bind(&World::OnJoinArea,this,player->m_transport,adjx.min_x,adjx.max_x,
//                    adjx.min_y,adjx.max_y));
//            }
//
//            adjx.min_x = adjx.max_x = xz - 1;
//
//            m_io_service.post(boost::bind(&World::OnLeaveArea,this,player->GetUniqueID(),player->ConnectionID(),
//                adjx.min_x,adjx.max_x,adjx.min_y,adjx.max_y));
//
//            if (with_transport)
//            {
//                m_io_service.post(boost::bind(&World::OnLeaveArea,this,player->m_transport->GetUniqueID(),adjx.min_x,
//                    adjx.max_x,adjx.min_y,adjx.max_y));
//            }
//
//        }
//        else if ( x < zone->minX())
//        {
//            m_io_service.post(boost::bind(&World::OnJoinArea,this,player->GetUniqueID(),player->ConnectionID(),
//                adjx.min_x,adjx.max_x,adjx.min_y,adjx.max_y));
//
//            if (with_transport)
//            {
//                m_io_service.post(boost::bind(&World::OnJoinArea,this,player->m_transport,adjx.min_x,adjx.max_x,
//                    adjx.min_y,adjx.max_y));
//            }
//
//            adjx.min_x = adjx.max_x = xz + 1;
//
//            m_io_service.post(boost::bind(&World::OnLeaveArea,this,player->GetUniqueID(),player->ConnectionID(),
//                adjx.min_x,adjx.max_x,adjx.min_y,adjx.max_y));
//
//            if (with_transport)
//            {
//                m_io_service.post(boost::bind(&World::OnLeaveArea,this,player->m_transport->GetUniqueID(),adjx.min_x,
//                    adjx.max_x,adjx.min_y,adjx.max_y));
//            }
//        }
//
//        if (y > zone->maxZ())
//        {
//            m_io_service.post(boost::bind(&World::OnJoinArea,this,player->GetUniqueID(),player->ConnectionID(),
//                adjy.min_x,adjy.max_x,adjy.min_y,adjy.max_y));
//
//            if (with_transport)
//            {
//                m_io_service.post(boost::bind(&World::OnJoinArea,this,player->m_transport,adjy.min_x,adjy.max_x,
//                    adjy.min_y,adjy.max_y));
//            }
//
//            adjy.min_y = adjy.max_y = yz - 1;
//
//            m_io_service.post(boost::bind(&World::OnLeaveArea,this,player->GetUniqueID(),player->ConnectionID(),
//                adjy.min_x,adjy.max_x,adjy.min_y,adjy.max_y));
//
//            if (with_transport)
//            {
//                m_io_service.post(boost::bind(&World::OnLeaveArea,this,player->m_transport->GetUniqueID(),adjy.min_x,
//                    adjy.max_x,adjy.min_y,adjy.max_y));
//            }
//        }
//        else if ( y < zone->minZ())
//        {
//            m_io_service.post(boost::bind(&World::OnJoinArea,this,player->GetUniqueID(),player->ConnectionID(),
//                adjy.min_x,adjy.max_x,adjy.min_y,adjy.max_y));
//
//            if (with_transport)
//            {
//                m_io_service.post(boost::bind(&World::OnJoinArea,this,player->m_transport,adjy.min_x,adjy.max_x,
//                    adjy.min_y,adjy.max_y));
//            }
//
//            adjy.min_y = adjy.max_y = yz + 1;
//
//            m_io_service.post(boost::bind(&World::OnLeaveArea,this,player->GetUniqueID(),player->ConnectionID(),
//                adjy.min_x,adjy.max_x,adjy.min_y,adjy.max_y));
//
//            if (with_transport)
//            {
//                m_io_service.post(boost::bind(&World::OnLeaveArea,this,player->m_transport->GetUniqueID(),adjy.min_x,
//                    adjy.max_x,adjy.min_y,adjy.max_y));
//            }
//        }
//    }
//
//    if (curpos == nextpos)
//    {
//        player->set_destination(Coord());
//
//        if (with_transport)
//            player->m_transport->set_destination(Coord());
//
//#if defined DEBUG
//        syslog(LOG_INFO,"Player Finished Moving.");
//#endif
//    }
//
//    return curpos == nextpos;
}

bool World::OnMoveInRange (boost::shared_ptr<Player> &caster, boost::shared_ptr<NPC> &target, uint32_t skillID ,
    float range)
{
    if (caster->GetUniqueID() == target->GetUniqueID())
        return false;

    uint16_t angle = 0;
    Coord dest = RangePosition(caster->get_position(),target->get_position(),range,&angle);

    Coord mpos;
    if (!m_MapMgr->ResolveMotion(caster->get_position(),target->get_position(),&mpos))
        return true;

    if (mpos != target->get_position())
    {
        boost::shared_ptr<OPacket> pkt(new OPacket);

        if (pkt)
        {
            srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_OBSTACLE);
            m_server->Send(pkt,caster->ConnectionID());
        }

        return true;
    }

    caster->set_angle(angle);

    if (dest.get_zone())
    {
        /// IF THE CASTER HAS WALL DONT LET HIM MOVE TO GET IN PLAYER RANGE.
        if (caster->hasWall())
            return true;

        boost::shared_ptr<Zone> zone = m_MapMgr->Find(dest.get_zone());

        if (!zone)
            return false;

        dest.set_z(zone->getHeightAt(dest.get_x(),dest.get_y(),caster->get_position().get_z()));

        if (distance(caster->get_position(),dest) < 2)
            return false;

        if (caster->GetCurrentTask() != PLAYER_TASK_MOVE_TO_TARGET)
        {
            TerminateTask(caster->GetUniqueID());

            caster->set_destination(dest);

            caster->SetCurrentTask(PLAYER_TASK_MOVE_TO_TARGET);

            InitTaskMoveTarget(caster->GetUniqueID(),target->GetUniqueID(),skillID);
        }
        else
        {
            if (!caster->is_moving())
            {
                caster->set_destination(dest);

                InitTaskMoveTarget(caster->GetUniqueID(),target->GetUniqueID(),skillID);
            }
            else
                caster->set_destination(dest);
        }

        boost::shared_ptr<OPacket> pkt(new OPacket);

        if (pkt)
        {
            srv_pkt::Movement(pkt,caster->GetUniqueID(),dest);
            m_server->SendChannel(pkt,caster->get_position().get_zone());
        }

        return true;
    }

    return false;
}

bool World::OnMoveNPC (boost::shared_ptr<NPC> &npc, uint32_t dt)
{
    Coord curpos = npc->get_position();
    Coord nextpos = npc->get_destination();

    if (!nextpos.get_zone())
        return true;

    float speed = npc->current_speed()*dt/1000;

    int16_t zoneID = curpos.get_zone();
    float angle = npc->get_angle()/10430.38208;   /// CONVERT ANGLE TO RADIANS
    float vx = speed*cos(angle);
    float vy = speed*sin(angle);

    float x = curpos.get_x()+vx;
    float y = curpos.get_y()+vy;

    boost::shared_ptr<Zone> zone = m_MapMgr->Find(zoneID);

    if (!zone)
    {
        npc->set_destination(Coord());
        return true;
    }

    if (zoneID == nextpos.get_zone())
    {
        if (vx > 0)
        {
            if (x > nextpos.get_x())
                x = nextpos.get_x();
        }
        else
        {
            if (x < nextpos.get_x())
                x = nextpos.get_x();
        }

        if (vy > 0)
        {
            if (y > nextpos.get_y())
                y = nextpos.get_y();
        }
        else
        {
            if (y < nextpos.get_y())
                y = nextpos.get_y();
        }
    }

    Coord dest;
    if (!m_MapMgr->ResolveMotion(curpos,Coord(x,y,curpos.get_z(),zoneID),&dest))
    {
        npc->set_destination(Coord());
        return true;
    }

    if (curpos.get_zone() != dest.get_zone())
    {
        /// CALCULATE THE X,Y OF THE CENTER ZONE
        uint8_t xz = (zoneID & 0xFF);
        uint8_t yz = (zoneID >> 8);

        /// CALCULATE ADJACENT AREAS,ZONE AND POSITION
        AdjacentZone adjx,adjy;

        if (x > 1920)
        {
            x -= 1920;
            ++zoneID;

            adjx.min_x = adjx.max_x = xz + 2;
            adjx.min_y = yz - 1;
            adjx.max_y = yz + 1;
        }
        else if ( x < 0)
        {
            x += 1920;
            --zoneID;

            adjx.min_x = adjx.max_x = xz - 2;
            adjx.min_y = yz - 1;
            adjx.max_y = yz + 1;
        }

        /// CALCULATE THE X,Y OF THE CENTER
        xz = (zoneID & 0xFF);
        yz = (zoneID >> 8);

        if (y > 1920)
        {
            y -= 1920;
            zoneID += 256;

            adjy.min_y = adjy.max_y = yz + 2;
            adjy.min_x = xz - 1;
            adjy.max_x = xz + 1;
        }
        else if ( y < 0)
        {
            y += 1920;
            zoneID -= 256;

            adjy.min_y = adjy.max_y = yz - 2;
            adjy.min_x = xz - 1;
            adjy.max_x = xz + 1;
        }

        /// INSERT PLAYER INTO THE NEW ZONE
        boost::shared_ptr<Zone> dzone = m_MapMgr->Find(zoneID);

        if (!dzone)
        {
             npc->set_destination(Coord());
             return true;
         }

        zone->RemoveNPC(npc->GetUniqueID());

        dzone->InsertNPC(npc);

        /// REGISTER TO ADJACENT ZONES
        if (x > 1920)
        {
            m_io_service.post(boost::bind(&World::OnJoinArea,this,npc,adjx.min_x,adjx.max_x,adjx.min_y,adjx.max_y));

            adjx.min_x = adjx.max_x = xz - 1;

            m_io_service.post(boost::bind(&World::OnLeaveArea,this,npc->GetUniqueID(),adjx.min_x,adjx.max_x,
                adjx.min_y,adjx.max_y));

        }
        else if ( x < 0)
        {
            m_io_service.post(boost::bind(&World::OnJoinArea,this,npc,adjx.min_x,adjx.max_x,adjx.min_y,adjx.max_y));

            adjx.min_x = adjx.max_x = xz + 1;

            m_io_service.post(boost::bind(&World::OnLeaveArea,this,npc->GetUniqueID(),adjx.min_x,adjx.max_x,
                adjx.min_y,adjx.max_y));
        }

        if (y > 1920)
        {
            m_io_service.post(boost::bind(&World::OnJoinArea,this,npc,adjy.min_x,adjy.max_x,adjy.min_y,adjy.max_y));

            adjy.min_y = adjy.max_y = yz - 1;

            m_io_service.post(boost::bind(&World::OnLeaveArea,this,npc->GetUniqueID(),adjy.min_x,adjy.max_x,
                adjy.min_y, adjy.max_y));
        }
        else if ( y < 0)
        {
            m_io_service.post(boost::bind(&World::OnJoinArea,this,npc,adjy.min_x,adjy.max_x,adjy.min_y,adjy.max_y));

            adjy.min_y = adjy.max_y = yz + 1;

            m_io_service.post(boost::bind(&World::OnLeaveArea,this,npc->GetUniqueID(),adjy.min_x,adjy.max_x,
                adjy.min_y,adjy.max_y));
        }
    }

    npc->set_position(dest);

    if (dest == nextpos)
        npc->set_destination(Coord());

    return dest == nextpos;

//    if (zoneID == nextpos.get_zone())
//    {
//        if (vx > 0)
//        {
//            if (x > nextpos.get_x())
//                x = nextpos.get_x();
//        }
//        else
//        {
//            if (x < nextpos.get_x())
//                x = nextpos.get_x();
//        }
//
//        if (vy > 0)
//        {
//            if (y > nextpos.get_y())
//                y = nextpos.get_y();
//        }
//        else
//        {
//            if (y < nextpos.get_y())
//                y = nextpos.get_y();
//        }
//
//        dzone = zone;
//    }
//    else
//    {
//        /// CALCULATE THE X,Y OF THE CENTER ZONE
//        uint8_t xz = (zoneID & 0xFF);
//        uint8_t yz = (zoneID >> 8);
//
//        /// CALCULATE ADJACENT AREAS,ZONE AND POSITION
//        AdjacentZone adjx,adjy;
//
//        if (x > 1920)
//        {
//            x -= 1920;
//            ++zoneID;
//
//            adjx.min_x = adjx.max_x = xz + 2;
//            adjx.min_y = yz - 1;
//            adjx.max_y = yz + 1;
//        }
//        else if ( x < 0)
//        {
//            x += 1920;
//            --zoneID;
//
//            adjx.min_x = adjx.max_x = xz - 2;
//            adjx.min_y = yz - 1;
//            adjx.max_y = yz + 1;
//        }
//
//        /// CALCULATE THE X,Y OF THE CENTER
//        xz = (zoneID & 0xFF);
//        yz = (zoneID >> 8);
//
//        if (y > 1920)
//        {
//            y -= 1920;
//            zoneID += 256;
//
//            adjy.min_y = adjy.max_y = yz + 2;
//            adjy.min_x = xz - 1;
//            adjy.max_x = xz + 1;
//        }
//        else if ( y < 0)
//        {
//            y += 1920;
//            zoneID -= 256;
//
//            adjy.min_y = adjy.max_y = yz - 2;
//            adjy.min_x = xz - 1;
//            adjy.max_x = xz + 1;
//        }
//
//        /// INSERT PLAYER INTO THE NEW ZONE
//        dzone = m_MapMgr->Find(zoneID);
//
//        if (!dzone)
//        {
//             npc->set_destination(Coord());
//             return true;
//         }
//
//        zone->RemoveNPC(npc->GetUniqueID());
//
//        dzone->InsertNPC(npc);
//
//        /// REGISTER TO ADJACENT ZONES
//        if (x > 1920)
//        {
//            m_io_service.post(boost::bind(&World::OnJoinArea,this,npc,adjx.min_x,adjx.max_x,adjx.min_y,adjx.max_y));
//
//            adjx.min_x = adjx.max_x = xz - 1;
//
//            m_io_service.post(boost::bind(&World::OnLeaveArea,this,npc->GetUniqueID(),adjx.min_x,adjx.max_x,
//                adjx.min_y,adjx.max_y));
//
//        }
//        else if ( x < 0)
//        {
//            m_io_service.post(boost::bind(&World::OnJoinArea,this,npc,adjx.min_x,adjx.max_x,adjx.min_y,adjx.max_y));
//
//            adjx.min_x = adjx.max_x = xz + 1;
//
//            m_io_service.post(boost::bind(&World::OnLeaveArea,this,npc->GetUniqueID(),adjx.min_x,adjx.max_x,
//                adjx.min_y,adjx.max_y));
//        }
//
//        if (y > 1920)
//        {
//            m_io_service.post(boost::bind(&World::OnJoinArea,this,npc,adjy.min_x,adjy.max_x,adjy.min_y,adjy.max_y));
//
//            adjy.min_y = adjy.max_y = yz - 1;
//
//            m_io_service.post(boost::bind(&World::OnLeaveArea,this,npc->GetUniqueID(),adjy.min_x,adjy.max_x,
//                adjy.min_y, adjy.max_y));
//        }
//        else if ( y < 0)
//        {
//            m_io_service.post(boost::bind(&World::OnJoinArea,this,npc,adjy.min_x,adjy.max_x,adjy.min_y,adjy.max_y));
//
//            adjy.min_y = adjy.max_y = yz + 1;
//
//            m_io_service.post(boost::bind(&World::OnLeaveArea,this,npc->GetUniqueID(),adjy.min_x,adjy.max_x,
//                adjy.min_y,adjy.max_y));
//        }
//    }
//
//    curpos.set_zone(zoneID);
//    curpos.set_x(x);
//    curpos.set_y(y);
//    curpos.set_z(dzone->getHeightAt(x,y,curpos.get_z()));
//
//    npc->set_position(curpos);
//
//    if (curpos == nextpos)
//        npc->set_destination(Coord());
//
//    return curpos == nextpos;
}

void World::OnAttackTarget(const boost::shared_ptr<NPC> &attacker, const boost::shared_ptr<Skill> &skill,
    const boost::shared_ptr<NPC> &target, const uint32_t attackID, const uint8_t bonus)
{
    /// IF THE SKILL HAS ADITIONAL BUFF ADD THEM AND THEN REMOVE THEM AFTER ATTACK SEQUENCE FINISH
    if (!skill->m_buff_list.empty())
    {
        Buff buff;
        buff.SkillID = skill->id();
        buff.GroupID = skill->group_id();
        buff.Effects = skill->m_buff_list;

        attacker->insert_buff(buff);
    }

    AttackInfo attk_info = skill->attack();

    NPC::attack_type phy_attack = attacker->physical_attack();
    NPC::attack_type mag_attack = attacker->magical_attack();

    srv_pkt::CastInfo info;
    info.attackID = attackID;
    info.casterID = attacker->GetUniqueID();
    info.targetID = target->GetUniqueID();
    info.skillID = skill->id();

    /// TODO: DO AOE ATTACKS!!

    int abs = skill->absolute_damage();

    srv_pkt::DamageInfo dmginfo;
    dmginfo.targetID = target->GetUniqueID();
    dmginfo.damage = 0;
    dmginfo.DmgState = srv_pkt::DAMAGE_STATE_NORMAL;
    dmginfo.MPAbs = 0;
    dmginfo.AttackState = srv_pkt::ATTACK_STATE_NORMAL;

    if (attk_info.type != ATTACK_NONE)
    {
        if (rand() % 100 < target->block_ratio())
            dmginfo.AttackState = srv_pkt::ATTACK_STATE_BLOCK;
        else
        {
            switch(attk_info.type)
            {
            case ATTACK_PHYSICAL_CLOSED:
            case ATTACK_PHYSICAL_RANGED:
                phy_attack.first = (phy_attack.first + attk_info.min_pwr)*attk_info.percent/100;
                phy_attack.second = (phy_attack.second + attk_info.max_pwr)*attk_info.percent/100;

                if (rand() % 100 < attacker->critical_ratio())
                    dmginfo.DmgState |= srv_pkt::DAMAGE_STATE_CRIT;

                break;
            case ATTACK_MAGICAL:
            case ATTACK_MAGICAL_RANGED:
            case ATTACK_MAGICAL_CLOSED:
                mag_attack.first = (mag_attack.first + attk_info.min_pwr)*attk_info.percent/100;
                mag_attack.second = (mag_attack.second + attk_info.max_pwr)*attk_info.percent/100;
                break;
            default:
#if defined DEBUG
                syslog(LOG_INFO,"World::OnAttackTarget - Unknown type = %i",attk_info.type);
#endif
                break;
            }

            /// CALCULATE DAMAGE

            uint32_t phy = phy_attack.first;
            uint32_t mag = mag_attack.first;

            float phy_dmg = ((float)phy)*(1.0-target->physical_absorb()/100.0) - (float)target->physical_defense();

            if (phy_dmg < 0)
                phy_dmg = 0;

            float mag_dmg = ((float)mag)*(1.0-target->magical_absorb()/100.0)- (float)target->magical_defense();

            if (mag_dmg < 0)
                mag_dmg = 0;

            phy_dmg *= (1.0f+static_cast<float>(target->WallPhysicalAbsorb())/100.0f);
            mag_dmg *= (1.0f+static_cast<float>(target->WallMagicalAbsorb())/100.0f);

            dmginfo.damage = (phy_dmg + mag_dmg)*(100+bonus)/100;

            if (target->hasStatus(STATUS_DIVISION))
            {
                std::map<uint32_t,Status> status = target->GetStatus();

                dmginfo.damage *= (1+status[STATUS_DIVISION].Payload/100);
            }

            if (dmginfo.DmgState & srv_pkt::DAMAGE_STATE_CRIT)
                dmginfo.damage *= 2;

            if (!dmginfo.damage)
                dmginfo.damage = 1;

            if (!target->hasWall())
            {
                /// CALCULATE KNOCKBACK

                if (skill->m_KBProbability && rand()%100 < skill->m_KBProbability)
                {
                    Coord pos = KnockbackPosition(attacker->get_position(),target->get_position(),
                        skill->m_KBDistance);

                    /// MOVE TARGET TO THE CLOSEST VALID POSITION.
                    Coord mpos;
                    if (m_server->GetWorld()->resolveMotion(target->get_position(),pos,&mpos))
                    {
                        pos = mpos;

                        TerminateTask(target->GetUniqueID());

                        dmginfo.AttackState |= srv_pkt::ATTACK_STATE_KNOCKBACK;
                        dmginfo.AttackState |= srv_pkt::ATTACK_STATE_POSITION;

                        dmginfo.dest = pos;
                    }
                }

                /// CALCULATE KNOCKDOWN

                if (skill->m_KDProbability && rand()%100 < skill->m_KDProbability)
                {
                    TerminateTask(target->GetUniqueID());

                    dmginfo.AttackState |= srv_pkt::ATTACK_STATE_POSITION;

                    dmginfo.dest = target->get_position();
                }

                /// CALCULATE MP ABSORTION DAMAGE

                dmginfo.MPAbs = dmginfo.damage*target->DamageMP()/100;

                if (dmginfo.MPAbs > target->mp())
                    dmginfo.MPAbs = target->mp();

                dmginfo.damage -= dmginfo.MPAbs;
            }
            else
                dmginfo.Wall = true;
        }
    }
    else
    {
        if (abs)
            dmginfo.damage = abs;
    }

    if (target->hp() > dmginfo.damage)
    {
        /// CALCULATE IF ANY STATUS LANDED ON THE TARGET

        if (!target->hasWall())
        {
            std::vector<Status>::const_iterator si;
            for ( si = skill->m_status_list.begin(); si != skill->m_status_list.end(); ++si)
            {
                if ( !target->hasStatus(si->ID) && si->Probability && rand() % 100 < si->Probability )
                {
                    dmginfo.DmgState |= srv_pkt::DAMAGE_STATE_STATUS;
                    dmginfo.StatusList.push_back(*si);
                }
                else
                {
                    if (!dmginfo.damage)
                        dmginfo.DmgState |= srv_pkt::DAMAGE_STATE_STATUS;
                }
            }

            if (attacker->using_imbue())
            {
                const Imbue &imb = attacker->GetImbue();

                for (si = imb.effect.begin(); si != imb.effect.end(); ++si)
                {
                    if ( !target->hasStatus(si->ID) && si->Probability && rand() % 100 < si->Probability )
                    {
                        dmginfo.DmgState |= srv_pkt::DAMAGE_STATE_STATUS;
                        dmginfo.StatusList.push_back(*si);
                    }
                }
            }
        }
    }
    else
        dmginfo.AttackState |= srv_pkt::ATTACK_STATE_DEAD;

    info.dmgInfo.push_back(dmginfo);

    if (!skill->m_buff_list.empty())
        attacker->remove_buff(skill->id());

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (!skill->delay())
    {
        srv_pkt::SkillCast_1st(pkt,srv_pkt::SKILL_ACTION_ATTACK,info);
        m_server->SendChannel(pkt,attacker->get_position().get_zone());
    }
    else
    {
        srv_pkt::SkillCast_2nd(pkt,info);
        m_server->SendChannel(pkt,attacker->get_position().get_zone());
    }

    /// CHECK IF THE ATTACKER IS IN ATTACK STATE
    if (attacker->get_attack_state() != ATTACK_ACTION_START)
    {
        if (attacker->get_attack_state() == ATTACK_ACTION_STOP)
        {
            if (attacker->type() == ET_PLAYER)
            {
                pkt.reset(new OPacket);

                srv_pkt::State(pkt,attacker->GetUniqueID(),STATE_ATTACK,ATTACK_ACTION_START);
                m_server->SendChannel(pkt,attacker->get_position().get_zone());
            }

            BTask task;
            task.task = BG_TASK_COOLDOWN_ATTACK;
            task.seconds = 10;
            task.entity = attacker;

            m_io_service.post(boost::bind(&World::InitBackgroundTask,this,task,true));
        }

        attacker->set_attack_state(ATTACK_ACTION_START);
    }

    /// CHECK IF THE TARGET IS IN ATTACK STATE
    if (target->get_attack_state() != ATTACK_ACTION_START)
    {
        if (target->get_attack_state() == ATTACK_ACTION_STOP)
        {
            if (target->type() == ET_PLAYER)
            {
                pkt.reset(new OPacket);

                srv_pkt::State(pkt,target->GetUniqueID(),STATE_ATTACK,ATTACK_ACTION_START);
                m_server->SendChannel(pkt,target->get_position().get_zone());
            }

            BTask task;
            task.task = BG_TASK_COOLDOWN_ATTACK;
            task.seconds = 10;
            task.entity = target;

            m_io_service.post(boost::bind(&World::InitBackgroundTask,this,task,true));
        }

        target->set_attack_state(ATTACK_ACTION_START);
    }

    /// UPDATE NEW STATUS(ES)
    std::vector<Status>::iterator si;
    for ( si = dmginfo.StatusList.begin(); si != dmginfo.StatusList.end(); ++si)
    {
        if (si->Duration)
        {
            BTask task;
            task.task = BG_TASK_STATUS;
            task.seconds = si->Duration/1000;
            task.arg = si->ID;
            task.entity = target;

            si->taskID = InitBackgroundTask(task);
        }

        target->insert_status(*si);

        if (target->type() == ET_PLAYER)
        {
            /// SEND STATUS COMPLETE INFORMATION
            boost::shared_ptr<Player> player = boost::shared_static_cast<Player,NPC>(target);

            if (player)
            {
                uint16_t lv = si->Level ? si->Level : si->Effect;

                pkt.reset(new OPacket);

                srv_pkt::PlayerStatus(pkt,player->GetEncodedStatus(),si->Duration,lv);
                m_server->Send(pkt,player->ConnectionID());
            }
        }

        pkt.reset(new OPacket);

        srv_pkt::BadStatus(pkt,target->GetUniqueID(),target->GetEncodedStatus(),target->GetStatus());
        m_server->SendChannel(pkt,target->get_position().get_zone());

        if (target->type() != ET_PLAYER && (si->ID == STATUS_FROST || si->ID == STATUS_DULL))
        {
            pkt.reset(new OPacket);

            srv_pkt::Speed(pkt,target);
            m_server->SendChannel(pkt,target->get_position().get_zone());
        }

        if (si->ID == STATUS_POISON || si->ID == STATUS_BURN)
        {
            /// ADD A DAMAGE OVER TIME TASK

            BTask task;
            task.task = BG_TASK_STATUS_DAMAGE;
            task.seconds = 5;
            task.entity = target;
            task.arg = si->Payload;


            si->payloadTaskID = InitBackgroundTask(task);
        }
    }

    if (dmginfo.damage)
    {
        if (!dmginfo.Wall)
        {
            target->receive_damage(dmginfo.damage,attacker->GetUniqueID());

            if (target->type() == ET_PLAYER)
            {
                boost::shared_ptr<Player> ptar = boost::shared_static_cast<Player,NPC>(target);

                if (ptar)
                {
                    srv_pkt::PLAYER_DEATH_TYPE death = srv_pkt::PLAYER_DEATH_NORMAL;

                    if (ptar->IsFreePvP())
                        death = srv_pkt::PLAYER_DEATH_CAPE;

                    pkt.reset(new OPacket);

                    srv_pkt::PlayerDeath(pkt,death);
                    m_server->Send(pkt,ptar->ConnectionID());
                }
            }

            std::map<uint32_t,Status> status = target->GetStatus();

            uint8_t type = status.empty() ? srv_pkt::STAT_HP : srv_pkt::STAT_HP | srv_pkt::STAT_STATUS;

            pkt.reset(new OPacket);

            srv_pkt::HPMP(pkt,target->GetUniqueID(),srv_pkt::BASE_OP_DEC_HP,status.size(),type,target->hp(),
                target->GetEncodedStatus(),status);

            m_server->SendChannel(pkt,target->get_position().get_zone());

            if (target->type() != ET_PLAYER)
            {
                m_io_service.post(boost::bind(&World::OnSignalHPMP,this,target->GetUniqueID(),
                    target->get_position().get_zone()));
            }
            else
            {
                boost::shared_ptr<Player> ptar = boost::shared_static_cast<Player,NPC>(target);

                if (ptar)
                {
                    if (ptar->IsDead())
                    {
                        ptar->ClearStatus();

                        pkt.reset(new OPacket);

                        if (pkt)
                        {
                            srv_pkt::PlayerStatus(pkt,ptar->GetEncodedStatus());
                            m_server->Send(pkt,ptar->ConnectionID());
                        }
                    }

                    boost::shared_ptr<Party> party = ptar->get_party();

                    if (party)
                    {
                        party->SendHPMPNotification(ptar->GetUniqueID(),ptar->hp(),ptar->max_hp(),
                            ptar->mp(),ptar->max_mp());
                    }
                }
            }

            if (dmginfo.MPAbs)
            {
                attacker->consume_mp(dmginfo.MPAbs);

                type = status.empty() ? srv_pkt::STAT_MP : srv_pkt::STAT_MP | srv_pkt::STAT_STATUS;

                pkt.reset(new OPacket);

                srv_pkt::HPMP(pkt,attacker->GetUniqueID(),srv_pkt::BASE_OP_DEC_MP,status.size(),type,attacker->mp(),
                    attacker->GetEncodedStatus(),status);

                m_server->SendChannel(pkt,attacker->get_position().get_zone());
            }

            if (attacker->type() == ET_PLAYER && target->type() == ET_PLAYER)
            {
                /// CHECK IF THE ATTACKER MURDERED SOMEONE AND MAKE IT PK

                boost::shared_ptr<Player> patk = boost::shared_static_cast<Player,NPC>(attacker);

                boost::shared_ptr<Player> ptar = boost::shared_static_cast<Player,NPC>(target);

                if (patk && ptar)
                {
                    if (!ptar->IsFreePvP())
                    {
//                        uint32_t points = 2000 + 200*std::abs((patk->level()-ptar->level()));

                        ///TODO: ADD MURDER POINTS TO CHAR
                    }
                }
            }
        }
        else
        {
            if (!target->hasWall())
            {
                std::pair<uint32_t,uint32_t> ret = target->remove_buff(target->WallSkillID());

                pkt.reset(new OPacket);

                srv_pkt::TermBuff(pkt,ret.second);
                m_server->SendChannel(pkt,target->get_position().get_zone());

                m_io_service.post(boost::bind(&World::TerminateBackgroundTask,this,ret.first));
            }
        }

        if (dmginfo.AttackState & srv_pkt::ATTACK_STATE_POSITION)
        {
            /// SET TARGET CONDITION AS STUNNED AND CANCEL ANY RUNNING TASK IN THE FOREGROUND

            switch(target->type())
            {
            case ET_MOB:
            case ET_MOB_QUEST:
            case ET_MOB_EVENT:
                target->SetCurrentTask(MOB_TASK_STUNNED);
                break;
            case ET_PLAYER:
                target->SetCurrentTask(PLAYER_TASK_STUNNED);
            default:
                break;
            }

            target->set_position(dmginfo.dest);

            BTask task;
            task.task = BG_TASK_STUN;
            task.seconds = 1;
            task.entity = target;
            
            m_io_service.post(boost::bind(&World::InitBackgroundTask,this,task,true));
        }
    }
}

void World::OnBuffTarget (const boost::shared_ptr<NPC> &caster, const boost::shared_ptr<Skill> &skill,
    const boost::shared_ptr<NPC> &target, const uint32_t attackID, const uint32_t targetID)
{
    /// THIS PACKET ACTIVATE SKILL COOLDOWN!!
    srv_pkt::CastInfo info;
    info.attackID = attackID;
    info.targetID = targetID;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    srv_pkt::SkillCast_2nd(pkt,info);
    m_server->SendChannel(pkt,caster->get_position().get_zone());

    std::vector<BuffEffect> buff_list = skill->buff_list();

    if (skill->type() == SKT_IMBUE && buff_list.empty())
    {
        Imbue imb;
        imb.skillID = skill->id();
        imb.castID = rand();
        imb.duration = skill->duration();
        imb.attack = skill->attack();
        imb.area = skill->area();
        imb.effect = skill->m_status_list;

        BTask task;
        task.task = BG_TASK_IMBUE;
        task.entity = target;
        task.persistent = false;
        task.seconds = skill->duration()/1000;
        task.arg = imb.skillID;

        imb.taskID = InitBackgroundTask(task);

        target->insert_imbue(imb);

        pkt.reset(new OPacket);

        srv_pkt::InitBuff(pkt,target->GetUniqueID(),skill->id(),imb.castID);
        m_server->SendChannel(pkt,target->get_position().get_zone());
    }
    else
    {
        Buff buff;
        buff.CastID = rand();
        buff.SkillID = skill->id();
        buff.GroupID = skill->group_id();
        buff.Level = skill->level();
        buff.duration = skill->duration();
        buff.Effects = buff_list;

        buff.ReqShield = skill->require_shield();
        buff.ReqDevil = skill->require_devil();
        buff.ReqWeapon = skill->required_weapons();

        /// NOTE: THE SKILL THAT ARE FINITE DONT CONSUME MP OVER TIME

        if (skill->duration())
        {
            BTask task;
            task.task = BG_TASK_BUFF;
            task.entity = target;
            task.persistent = false;
            task.seconds = skill->duration()/1000;
            task.arg = buff.SkillID;

            buff.taskID = InitBackgroundTask(task);
        }

        if (skill->m_MPConsumeTime)
        {
            BTask task;
            task.task = BG_TASK_CONSUME_MP;
            task.entity = target;
            task.seconds = skill->m_MPConsumeTime;
            task.arg = skill->m_MPConsume;
            task.arg1 = skill->id();

            buff.taskID = InitBackgroundTask(task);
        }

        /// REMOVE ALL BUFF THAT THIS BUFF CANCEL
        
        std::vector<std::pair<uint32_t,uint32_t> > term_buffs = target->RemoveLesserBuffs(buff);

        for (size_t i = 0; i < term_buffs.size(); ++i)
        {
            pkt.reset(new OPacket);

            srv_pkt::TermBuff(pkt,term_buffs[i].second);
            m_server->SendChannel(pkt,target->get_position().get_zone());

            m_io_service.post(boost::bind(&World::TerminateBackgroundTask,this,term_buffs[i].first));
        }

        target->insert_buff(buff);

        pkt.reset(new OPacket);

        srv_pkt::InitBuff(pkt,target->GetUniqueID(),skill->id(),buff.CastID);
        m_server->SendChannel(pkt,target->get_position().get_zone());

        /// AOE BUFFS (MOVE INSIDE PARTY CODE)
        /*
        AttackArea area = skill->area();

        if (area.reference == REF_PLAYER_POSITION && m_player->in_party())
        {
            boost::shared_ptr<Party> party = m_player->get_party();

            int count = 1;
            uint32_t buffID = 0;

            for (auto i = party->begin(); i != party->end() && count < area.max_targets; ++i,++count)
            {
                if (m_player->GetUniqueID() != (*i)->GetUniqueID() &&
                    distance(m_player->get_position(),(*i)->get_position()) < area.distance*0.1)
                {
                    buff.CastID = ((*i)->GetUniqueID()+skill->id());

                    if ( (*i)->is_buff_superseded(buff))
                        (*i)->insert_buff(buff);
                }
            }
        }
        */
    }
}

boost::shared_ptr<NPC> World::GetTarget (const uint32_t targetID, const Coord &position) const
{
    boost::shared_ptr<NPC> target;

    if (targetID)
    {
        target = FindNPC_CloseToPosition(targetID,position);

        if (!target)
            target = FindPlayer_CloseToPosition(targetID,position);
    }

    return target;
}

void World::OnJoinArea (const uint32_t playerID, const uint32_t connectionID, const uint8_t min_x, const uint8_t max_x,
    const uint8_t min_y, const uint8_t max_y)
{
    int16_t zoneID = 0;
    NearObjects objs;
    boost::shared_ptr<Zone> zone;

    for ( uint8_t x = min_x; x <= max_x; ++x)
    {
        for (uint8_t y = min_y; y <= max_y; ++y)
        {
            zoneID = (y << 8) + x;
            zone = m_MapMgr->Find(zoneID);

            if (zone)
            {
                if (!zone->Count())
                    m_io_service.post(boost::bind(&World::OnActivateZoneAI,this,zoneID));

                OnSpawnPlayer(playerID,zoneID,true);
                zone->GetNearObjects(playerID,&objs);
            }
        }
    }

    /// ADD OBJECTS IN RANGE IN PLAYER VIEW
    boost::shared_ptr<OPacket> p1,p2,p3;

    if (!objs.building_list.empty())
    {
#if defined DEBUG
        syslog(LOG_INFO,"SpawnObjects - # Buildings %i",objs.building_list.size());
#endif

        p1.reset(new OPacket);
        p2.reset(new OPacket);
        p3.reset(new OPacket);

        if (p1 && p2 && p3)
        {
            srv_pkt::SpawnBegin(p1,objs.building_list.size(),srv_pkt::SPAWN_OBJECT);
            srv_pkt::Spawn(p2,objs.building_list);
            srv_pkt::SpawnEnd(p3);
        }

        m_server->Send(p1,p2,p3,connectionID);
    }

    if (!objs.npc_list.empty())
    {
#if defined DEBUG
        syslog(LOG_INFO,"SpawnObjects - # NPCs %i",objs.npc_list.size());
#endif

        int MAX_ENTITIES = 16;
        int size = 0;
        int pending = objs.npc_list.size();
        std::vector<boost::shared_ptr<NPC> >::const_iterator beg = objs.npc_list.begin();

        do
        {
            if (pending > MAX_ENTITIES)
                size = MAX_ENTITIES;
            else
                size = pending;

            p1.reset(new OPacket);
            p2.reset(new OPacket);
            p3.reset(new OPacket);

            if (p1 && p2 && p3)
            {
                srv_pkt::SpawnBegin(p1,size,srv_pkt::SPAWN_OBJECT);
                srv_pkt::Spawn(p2,beg,beg+size);
                srv_pkt::SpawnEnd(p3);
                m_server->Send(p1,p2,p3,connectionID);
            }

            pending -= size;
            beg += size;
        }
        while (beg != objs.npc_list.end() && pending > 0);
    }

    if (!objs.item_list.empty())
    {
#if defined DEBUG
        syslog(LOG_INFO,"SpawnObjects - # Items %i",objs.item_list.size());
#endif

        for ( NearObjects::item_const_iterator i = objs.item_list.begin(); i != objs.item_list.end(); ++i)
        {
            p1.reset(new OPacket);

            if (p1)
            {
                srv_pkt::Spawn(p1,*i);
                m_server->Send(p1,connectionID);
            }
        }
    }

    if (!objs.player_list.empty())
    {
#if defined DEBUG
        syslog(LOG_INFO,"SpawnObjects - # Players %i",objs.player_list.size());
#endif

        p1.reset(new OPacket);
        p2.reset(new OPacket);
        p3.reset(new OPacket);

        if (p1 && p2 && p3)
        {
            srv_pkt::SpawnBegin(p1,objs.player_list.size(),srv_pkt::SPAWN_OBJECT);
            srv_pkt::Spawn(p2,objs.player_list);
            srv_pkt::SpawnEnd(p3);
            m_server->Send(p1,p2,p3,connectionID);
        }
    }

    m_server->JoinAdjacentChannels(connectionID,min_x,max_x,min_y,max_y);
}

void World::OnLeaveArea (const uint32_t playerID, const uint32_t connectionID, const uint8_t min_x, const uint8_t max_x,
    const uint8_t min_y, const uint8_t max_y)
{
    if(m_server->LeaveAdjacentChannels(connectionID,min_x,max_x,min_y,max_y))
    {
        int16_t zoneID = 0;
        std::set<uint32_t> ids;
        boost::shared_ptr<Zone> zone;

        boost::shared_ptr<OPacket> p1,p2,p3;

        for ( uint8_t x = min_x; x <= max_x; ++x)
        {
            for (uint8_t y = min_y; y <= max_y; ++y)
            {
                zoneID = (y << 8) + x;
                zone = m_MapMgr->Find(zoneID);

                if (zone)
                {
                    if (!zone->Count())
                        m_io_service.post(boost::bind(&World::OnDeactivateZoneAI,this,zoneID));

                    p1.reset(new OPacket);

                    if (p1)
                    {
                        srv_pkt::Despawn(p1,playerID);
                        m_server->SendChannel(p1,zoneID,true);
                    }

                    zone->GetNearObjectIDs(ids);
                }
            }
        }

        p1.reset(new OPacket);
        p2.reset(new OPacket);
        p3.reset(new OPacket);

        srv_pkt::SpawnBegin(p1,ids.size(),srv_pkt::DESPAWN_OBJECT);
        srv_pkt::Spawn(p2,ids.begin(),ids.end());
        srv_pkt::SpawnEnd(p3);
        m_server->Send(p1,p2,p3,connectionID);
    }
}

void World::OnJoinArea (const boost::shared_ptr<NPC> &npc, const uint8_t min_x, const uint8_t max_x,
    const uint8_t min_y, const uint8_t max_y)
{
    int16_t zoneID = 0;
    boost::shared_ptr<Zone> zone;
    boost::shared_ptr<OPacket> p1,p2,p3;

    for ( uint8_t x = min_x; x <= max_x; ++x)
    {
        for (uint8_t y = min_y; y <= max_y; ++y)
        {
            zoneID = (y << 8) + x;
            zone = m_MapMgr->Find(zoneID);

            if (zone)
            {
                /// DEACTIVATE AI IF THE MOB IS NOT NEAR A PLAYER
                if (!zone->Count())
                {
                    switch(npc->type())
                    {
                    case ET_MOB:
                    case ET_MOB_QUEST:
                    case ET_MOB_EVENT:
                        m_io_service.post(boost::bind(&World::TerminateTask,this,npc->GetUniqueID()));
                        break;
                    default:
                        break;
                    }
                }

                p1.reset(new OPacket);
                p2.reset(new OPacket);
                p3.reset(new OPacket);

                if (p1 && p2 && p3)
                {
                    srv_pkt::SpawnBegin(p1,1,srv_pkt::SPAWN_OBJECT);
                    srv_pkt::Spawn(p2,npc);
                    srv_pkt::SpawnEnd(p3);
                    m_server->SendChannel(p1,p2,p3,zoneID,true);
                }
            }
        }
    }
}

void World::OnLeaveArea (const uint32_t npcID, const uint8_t min_x, const uint8_t max_x,
    const uint8_t min_y, const uint8_t max_y)
{
    int16_t zoneID = 0;
    boost::shared_ptr<Zone> zone;
    boost::shared_ptr<OPacket> pkt;

    for ( uint8_t x = min_x; x <= max_x; ++x)
    {
        for (uint8_t y = min_y; y <= max_y; ++y)
        {
            zoneID = (y << 8) + x;
            zone = m_MapMgr->Find(zoneID);

            if (zone)
            {
                pkt.reset(new OPacket);

                if (pkt)
                {
                    srv_pkt::Despawn(pkt,npcID);
                    m_server->SendChannel(pkt,zoneID,true);
                }
            }
        }
    }
}

void World::OnSpawnPlayer (const uint32_t playerID, const int16_t zone, bool priv)
{
    boost::shared_ptr<Player> player = FindPlayer_ByID(playerID);

    if (player)
    {
        boost::shared_ptr<OPacket> p1(new OPacket), p2(new OPacket), p3(new OPacket);

        if (p1 && p2 && p3)
        {
            srv_pkt::SpawnBegin(p1,1,srv_pkt::SPAWN_OBJECT);
            srv_pkt::Spawn(p2,player);
            srv_pkt::SpawnEnd(p3);
        }

        m_server->SendChannel(p1,p2,p3,zone,priv);
    }
}

void World::OnTaskTeleport (const uint32_t playerID, const Coord &pos, const boost::system::error_code &error)
{
    boost::shared_ptr<Player> player = FindPlayer_ByID(playerID);

    if (!player)
        return;

    player->SetCurrentTask(PLAYER_TASK_IDLE);

    if (!error)
    {
        TerminateStateTask(playerID);

        boost::shared_ptr<OPacket> pkt;

        /// TERMINATE AND REMOVE UNPERSISTENT BUFF AND ONLY TERMINATE PERSISTENT ONES

        std::map<uint32_t,Buff> buffs = player->get_buff_list();

        for (std::map<uint32_t,Buff>::iterator it = buffs.begin(); it != buffs.end(); ++it)
        {
            if (it->second.passive)
                continue;

            if (!it->second.persistent)
            {
                player->remove_buff(it->first,false);
                m_io_service.post(boost::bind(&World::TerminateBackgroundTask,this,it->second.taskID));
            }
        }

        /// TERMINATE AND REMOVE STATUS

        std::map<uint32_t,Status> status = player->GetStatus();

        for (std::map<uint32_t,Status>::iterator it = status.begin(); it != status.end(); ++it)
        {
            m_io_service.post(boost::bind(&World::TerminateBackgroundTask,this,it->second.taskID));

            if (it->second.payloadTaskID)
                m_io_service.post(boost::bind(&World::TerminateBackgroundTask,this,it->second.payloadTaskID));
        }

        player->ClearStatus();

        pkt.reset(new OPacket);

        srv_pkt::PlayerStatus(pkt,player->GetEncodedStatus());
        m_server->Send(pkt,player->ConnectionID());

        /// DESPAWN PLAYER TRANSPORT

        if (player->TransportID())
        {
            boost::shared_ptr<Transport> transport = player->GetTransport();

            if (transport)
            {
                if (transport->type() == ET_COS_TRANSPORT)
                {
                    /// IN CASE THE PLAYER DIES THE TRANSPORT HAS TO DESPAWN AND THE LOAD DROP TO THE FLOOR
                    OnSignalTransportDead(transport->GetUniqueID(),transport->get_position().get_zone());
                }
                else
                {
                    RemoveNPC(transport->GetUniqueID(),transport->get_position());

                    transport->set_position(pos);
                    transport->set_destination(Coord());
                }
            }
        }

        /// REMOVE FROM THE WORLD AND MOVE IT TO THE DESIRED POSITION

        RemovePlayer(playerID);

        player->set_position(pos);
        player->set_destination(Coord());

        pkt.reset(new OPacket);

        srv_pkt::TeleportZone(pkt,pos.get_zone());
        m_server->Send(pkt,player->ConnectionID());
    }
    else
    {
        boost::shared_ptr<OPacket> pkt(new OPacket);

        srv_pkt::State(pkt,playerID,STATE_TELEPORT,TELEPORT_ACTION_STOP);
        m_server->SendChannel(pkt,player->get_position().get_zone());
    }
}

void World::OnTaskMove (const uint32_t playerID, uint32_t dt, const boost::system::error_code &error)
{
    boost::shared_ptr<Player> player = FindPlayer_ByID(playerID);

    if (!player)
        return;

    /// CHECK IF THE PLAYER IS MOUNTED ON A TRANSPORT
    boost::shared_ptr<NPC> transport = player->GetTransport();

    bool with_transport = false;

    if (transport && player->GetCurrentTask() == PLAYER_TASK_MOVE_TRANSPORT)
        with_transport = true;

    if (!error)
    {
        if (player->IsDead())
        {
            player->SetCurrentTask(PLAYER_TASK_IDLE);
            player->set_destination(Coord());

            if (with_transport)
                transport->set_destination(Coord());

            return;
        }

        if (!OnMovePlayer(player,dt,with_transport))
        {
            InitTaskMove(playerID);
        }
        else
        {
            player->SetCurrentTask(PLAYER_TASK_IDLE);
            m_io_service.post(boost::bind(&World::InitTaskIdle,this,playerID));

            boost::shared_ptr<Party> party = player->get_party();

            if (party)
                party->SendMoveNotification(player->GetUniqueID(),player->get_position());

            if (player->get_guild_id())
            {
                boost::shared_ptr<Guild> guild = player->get_guild();

                guild->member_zone(player->get_name(),player->get_position().get_zone());
            }
        }
    }
    else
    {
        boost::mutex::scoped_lock lock(m_task_mutex);

        timer_iterator iter = m_tasks.find(playerID);

        if (iter == m_tasks.end())
            return;

        uint32_t secs = iter->second->expires_from_now().total_milliseconds();

        if (secs > dt)
            dt = 0;
        else
            dt -= secs;

        OnMovePlayer(player,dt,with_transport);

        boost::shared_ptr<Party> party = player->get_party();

        if (party)
            party->SendMoveNotification(player->GetUniqueID(),player->get_position());

        if (player->get_guild_id())
        {
            boost::shared_ptr<Guild> guild = player->get_guild();

            guild->member_zone(player->get_name(),player->get_position().get_zone());
        }

        if (with_transport)
            transport->set_destination(Coord());
    }
}

void World::OnTaskMoveItem (const uint32_t playerID, const uint8_t src, const uint8_t dest, const uint16_t quantity,
    const uint8_t time, const boost::system::error_code &error)
{
    boost::shared_ptr<Player> player = FindPlayer_ByID(playerID);

    if (!player)
        return;

    boost::shared_ptr<OPacket> pkt;

    if (!error)
    {
        if (time)
        {
            /// TODO: INCREAMENT THE PLAYER WAIT TIME IN 1
            InitTaskMoveItem(playerID,src,dest,quantity,time-1);
        }
        else
        {
            boost::shared_ptr<Storage> store = player->get_storage(STORAGE_INVENTORY);

            Item isrc = store->GetItem(src);
            Item idest = store->GetItem(dest);

            store->MoveItem(src,dest,quantity);

            pkt.reset(new OPacket);

            srv_pkt::Item(pkt,src,dest,quantity,srv_pkt::ITEM_MOVE_INV_TO_INV);
            m_server->Send(pkt,player->ConnectionID());

            if ( dest < Item::MAX_EQUIPMENT_SLOT)
            {
                if (isrc.get_type() != ITEM_ARROW && isrc.get_type() != ITEM_BOLT)
                {
                    pkt.reset(new OPacket);

                    srv_pkt::ItemEquip(pkt,player->GetUniqueID(),dest,isrc.get_id(),0);
                    m_server->SendChannel(pkt,player->get_position().get_zone());
                }

                if (dest == Item::WEAPON_SLOT || dest == Item::SHIELD_SLOT)
                {
                    /// CANCEL INVALIDATED BUFFS

                    std::vector<std::pair<uint32_t,uint32_t> > term_buffs = player->RemoveInvalidatedBuffs();

                    for (size_t i = 0; i < term_buffs.size(); ++i)
                    {
                        pkt.reset(new OPacket);

                        srv_pkt::TermBuff(pkt,term_buffs[i].second);
                        m_server->SendChannel(pkt,player->get_position().get_zone());

                        TerminateBackgroundTask(term_buffs[i].first);
                    }
                }

                player->RecalculateStats();
            }
            else if ( src < Item::MAX_EQUIPMENT_SLOT)
            {
                pkt.reset(new OPacket);

                srv_pkt::ItemUnequip(pkt,playerID,src,isrc.get_id());
                m_server->SendChannel(pkt,player->get_position().get_zone());

                if (src == Item::WEAPON_SLOT || src == Item::SHIELD_SLOT)
                {
                    /// CANCEL INVALIDATED BUFFS

                    std::vector<std::pair<uint32_t,uint32_t> > term_buffs = player->RemoveInvalidatedBuffs();

                    for (size_t i = 0; i < term_buffs.size(); ++i)
                    {
                        pkt.reset(new OPacket);

                        srv_pkt::TermBuff(pkt,term_buffs[i].second);
                        m_server->SendChannel(pkt,player->get_position().get_zone());

                        TerminateBackgroundTask(term_buffs[i].first);
                    }
                }

                player->RecalculateStats();
            }

            if ((dest == Item::JOB_CAPE_SLOT || src == Item::JOB_CAPE_SLOT) && isrc.get_type() != ITEM_PVP_CAPE)
            {
                boost::shared_ptr<TeleportBuilding> blg = FindBuilding_CloseToPosition(player->get_position());

                if (blg)
                {
                    syslog(LOG_INFO,"Teleport Building ID = %i",blg->m_Property.refID);

                    Teleport tlp = FindTeleport(blg->TeleportID);

                    if (tlp.ID)
                    {
                        InitTaskTeleport(playerID,Coord(tlp.x,tlp.y,tlp.z,tlp.Zone),0);
                    }
                    else
                    {
                        syslog(LOG_INFO,"OnTaskMoveItem - Unknown Teleport = %i",blg->TeleportID);
                    }
                }
            }
            else
            {
                player->SetCurrentTask(PLAYER_TASK_IDLE);
                m_io_service.post(boost::bind(&World::InitTaskIdle,this,playerID));
            }
        }
    }
    else
    {
        if (dest == Item::JOB_CAPE_SLOT || src == Item::JOB_CAPE_SLOT)
        {
            pkt.reset(new OPacket);

            srv_pkt::ItemEquipWaitCancel(pkt,playerID);
            m_server->SendChannel(pkt,player->get_position().get_zone());
        }

        player->SetCurrentTask(PLAYER_TASK_IDLE);
    }
}

void World::OnTaskMoveTarget (const uint32_t playerID, const uint32_t targetID, const uint32_t skillID,
    uint32_t dt, const boost::system::error_code &error)
{
    boost::shared_ptr<Player> player = FindPlayer_ByID(playerID);

    if (!player)
        return;

    if (!error)
    {
        if (player->IsDead())
        {
            player->set_destination(Coord());
            player->SetCurrentTask(PLAYER_TASK_IDLE);
            return;
        }

        if (!OnMovePlayer(player,dt,false))
        {
            if (!InitTaskMoveTarget(playerID,targetID,skillID))
            {
                player->set_destination(Coord());
                player->SetCurrentTask(PLAYER_TASK_IDLE);
            }
        }
        else
        {
            boost::shared_ptr<Party> party = player->get_party();

            if (party)
                party->SendMoveNotification(player->GetUniqueID(),player->get_position());

            if (player->get_guild_id())
            {
                boost::shared_ptr<Guild> guild = player->get_guild();

                guild->member_zone(player->get_name(),player->get_position().get_zone());
            }

            InitTaskCastSkill(playerID,targetID,skillID,Coord());
        }
    }
    else
    {
        boost::mutex::scoped_lock lock(m_task_mutex);

        timer_iterator iter = m_tasks.find(playerID);

        if (iter == m_tasks.end())
            return;

        uint32_t secs = iter->second->expires_from_now().total_milliseconds();

        if (secs > dt)
            dt = 0;
        else
            dt -= secs;

        OnMovePlayer(player,dt,false);

        boost::shared_ptr<Party> party = player->get_party();

        if (party)
            party->SendMoveNotification(player->GetUniqueID(),player->get_position());

        if (player->get_guild_id())
        {
            boost::shared_ptr<Guild> guild = player->get_guild();

            guild->member_zone(player->get_name(),player->get_position().get_zone());
        }
    }
}

void World::OnTaskMovePickup (const uint32_t playerID, const uint32_t objectID, uint32_t dt,
    const boost::system::error_code &error)
{
    boost::shared_ptr<Player> player = FindPlayer_ByID(playerID);

    if (!player)
        return;

    if (!error)
    {
        if (player->IsDead())
            return;

        boost::shared_ptr<DropableItem> item = FindItem_CloseToPosition(objectID,player->get_position());

        if (item)
        {
            if (!OnMovePlayer(player,dt,player->m_transport ? true : false))
            {
                if (!InitTaskMovePickup(playerID,objectID))
                {
                    boost::shared_ptr<Party> party = player->get_party();

                    if (party)
                        party->SendMoveNotification(player->GetUniqueID(),player->get_position());

                    if (player->get_guild_id())
                    {
                        boost::shared_ptr<Guild> guild = player->get_guild();

                        guild->member_zone(player->get_name(),player->get_position().get_zone());
                    }

                    player->set_destination(Coord());
                    player->SetCurrentTask(PLAYER_TASK_IDLE);
                }
            }
            else
            {
                boost::shared_ptr<Party> party = player->get_party();

                if (party)
                    party->SendMoveNotification(player->GetUniqueID(),player->get_position());

                InitTaskPickupExec(playerID,objectID);
            }
        }
        else
        {
            player->set_destination(Coord());
            player->SetCurrentTask(PLAYER_TASK_IDLE);
        }
    }
    else
    {
        boost::mutex::scoped_lock lock(m_task_mutex);

        timer_iterator iter = m_tasks.find(playerID);

        if (iter == m_tasks.end())
            return;

        uint32_t secs = iter->second->expires_from_now().total_milliseconds();

        if (secs > dt)
            dt = 0;
        else
            dt -= secs;

        OnMovePlayer(player,dt,false);

        boost::shared_ptr<Party> party = player->get_party();

        if (party)
            party->SendMoveNotification(player->GetUniqueID(),player->get_position());

        if (player->get_guild_id())
        {
            boost::shared_ptr<Guild> guild = player->get_guild();

            guild->member_zone(player->get_name(),player->get_position().get_zone());
        }
    }
}

void World::OnTaskExecSkill(const uint32_t playerID, const uint32_t targetID, const uint32_t skillID,
    const uint32_t attackID, const Coord &pos, const boost::system::error_code &error)
{
    boost::shared_ptr<Player> player = FindPlayer_ByID(playerID);

    if (!player)
        return;

    if (!error)
    {
#if defined DEBUG
        syslog(LOG_INFO,"OnTaskExecSkill");
#endif

        if (player->IsDead())
            return;

        boost::shared_ptr<OPacket> pkt(new OPacket);

        if (player->hasStatus(STATUS_GROUP_RESTRAIN | STATUS_FEAR))
        {
            player->SetCurrentTask(PLAYER_TASK_IDLE);

            srv_pkt::PlayerAction(pkt,srv_pkt::PLAYER_ACTION_END);
            m_server->Send(pkt,player->ConnectionID());
            return;
        }

        boost::shared_ptr<NPC> target;

        if (targetID)
            target = GetTarget(targetID,player->get_position());
        else
            target = player;

        if (!target || target->IsDead())
        {
            player->SetCurrentTask(PLAYER_TASK_IDLE);

            srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_INVALID_TARGET);
            m_server->Send(pkt,player->ConnectionID());

            pkt.reset(new OPacket);

            srv_pkt::PlayerAction(pkt,srv_pkt::PLAYER_ACTION_END);
            m_server->Send(pkt,player->ConnectionID());
            return;
        }

        boost::shared_ptr<Skill> skill = m_SkillMgr->find(skillID);

        if (skill->required_hp_percent() > (player->hp()*100)/player->max_hp())
        {
            player->SetCurrentTask(PLAYER_TASK_IDLE);

            srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_INVALID_HP);
            m_server->Send(pkt,player->ConnectionID());

            pkt.reset(new OPacket);

            srv_pkt::PlayerAction(pkt,srv_pkt::PLAYER_ACTION_ERROR);
            m_server->Send(pkt,player->ConnectionID());
            return;
        }

        if (skill->required_mp() > player->mp())
        {
            player->SetCurrentTask(PLAYER_TASK_IDLE);

            srv_pkt::SkillCast_1st(pkt,pkt::ERROR_SKCAST_INVALID_MP);
            m_server->Send(pkt,player->ConnectionID());

            pkt.reset(new OPacket);

            srv_pkt::PlayerAction(pkt,srv_pkt::PLAYER_ACTION_ERROR);
            m_server->Send(pkt,player->ConnectionID());
            return;
        }

        /// TODO: CHECK FOR KD!!

        switch (skill->ActionType())
        {
        case SKILL_ACTION_ATTACK:
            {
                uint8_t bonus = player->GetMasteryBonus(skill->required_mastery());

                OnAttackTarget(player,skill,target,attackID,bonus);

                /// CONSUME AMMO FOR BOW AND CROSSBOWS ATTACKS
                boost::shared_ptr<Storage> st = player->get_storage(STORAGE_INVENTORY);

                Storage::iterator weapon = st->find(Item::WEAPON_SLOT);
                Storage::iterator ammo = st->find(Item::SHIELD_SLOT);

                if (weapon->second.get_type() == ITEM_CH_BOW || weapon->second.get_type() == ITEM_EU_CROSSBOW)
                {
                    if (ammo->second.get_type() == ITEM_ARROW || ammo->second.get_type() == ITEM_BOLT)
                    {
                        ammo->second.set_quantity(ammo->second.get_quantity()-1);

                        srv_pkt::ItemAmmo(pkt,ammo->second.get_quantity());
                        m_server->Send(pkt,player->ConnectionID());

                        /// TODO: SAVE QUANTITY IN THE DB!
                    }
                }
            }
            break;
        case SKILL_ACTION_TELEPORT:
            {
                /// FOR PHANTOM WALK AND TELEPORT SKILLS!!
                uint16_t angle;
                Coord dest = TeleportPosition(target->get_position(),pos,skill->TeleportDistance(),&angle);

                /// MOVE TARGET TO THE CLOSEST VALID POSITION.
                Coord mpos;
                if (m_server->GetWorld()->resolveMotion(target->get_position(),dest,&mpos))
                {
                    dest = mpos;

                    target->set_position(dest);
                    target->set_angle(angle);
                }

                srv_pkt::CastInfo info;
                info.attackID = attackID;
                info.targetID = targetID;
                info.dest = target->get_position();

                srv_pkt::SkillCast_2nd(pkt,info);
                m_server->SendChannel(pkt,target->get_position().get_zone());
            }
            break;
        case SKILL_ACTION_BUFF:
            OnBuffTarget(player,skill,target,attackID,targetID);
            break;
        case SKILL_ACTION_HEAL:
            break;
        case SKILL_ACTION_RESURRECT:
            break;
        default:
            break;
        }

        pkt.reset(new OPacket);

        srv_pkt::PlayerAction(pkt,srv_pkt::PLAYER_ACTION_END);
        m_server->Send(pkt,player->ConnectionID());

        std::map<uint32_t,Status> status = player->GetStatus();
        uint8_t type = 0;

        if (skill->required_mp())
        {
            player->consume_mp(skill->required_mp());

            type = status.empty() ? srv_pkt::STAT_MP : srv_pkt::STAT_MP | srv_pkt::STAT_STATUS;

            pkt.reset(new OPacket);

            srv_pkt::HPMP(pkt,player->GetUniqueID(),srv_pkt::BASE_OP_DEC_MP,status.size(),type,player->mp(),
                player->GetEncodedStatus(),status);

            m_server->SendChannel(pkt,player->get_position().get_zone());

            boost::shared_ptr<Party> party = player->get_party();

            if (party)
            {
                party->SendHPMPNotification(player->GetUniqueID(),player->hp(),player->max_hp(),
                    player->mp(),player->max_mp());
            }
        }

        if(skill->required_hp_percent())
        {
            type = status.empty() ? srv_pkt::STAT_HP : srv_pkt::STAT_HP | srv_pkt::STAT_STATUS;

            player->consume_hp(skill->required_hp_percent()*player->max_hp()/100);

            pkt.reset(new OPacket);

            srv_pkt::HPMP(pkt,player->GetUniqueID(),srv_pkt::BASE_OP_DEC_HP,status.size(),type,player->hp(),
                player->GetEncodedStatus(),status);

            m_server->SendChannel(pkt,player->get_position().get_zone());

            boost::shared_ptr<Party> party = player->get_party();

            if (party)
            {
                party->SendHPMPNotification(player->GetUniqueID(),player->hp(),player->max_hp(),
                    player->mp(),player->max_mp());
            }
        }

        if (skill->init() || skill->delay() || skill->next_delay())
        {
            /// IF ITS BASE ATTACK KEEP ATTACKING IF THERES NO OTHER SKILL IN QUEUE

            if (!skill->next_skill() && !player->GetQueueSkill())
            {
                switch(skill->id())
                {
                case SKILL_PUNCH_01:
                case SKILL_CH_SWORD_BASE_01:
                case SKILL_CH_SPEAR_BASE_01:
                case SKILL_CH_BOW_BASE_01:
                case SKILL_EU_SWORD_BASE_01:
                case SKILL_EU_TSWORD_BASE_01:
                case SKILL_EU_AXE_BASE_01:
                case SKILL_EU_CROSSBOW_BASE_01:
                case SKILL_EU_DAGGER_BASE_01:
                case SKILL_EU_STAFF_BASE_01:
                case SKILL_EU_WAND_WARLOCK_BASE_01:
                case SKILL_EU_HARP_BASE_01:
                case SKILL_EU_WAND_CLERIC_BASE_01:
                    player->SetQueueSkill(skill->id(),targetID);
                    break;
                default:
                    break;
                }
            }

            InitTaskWaitSkill(playerID,targetID,skill->next_delay(),skill->next_skill());
        }
        else
            player->SetCurrentTask(PLAYER_TASK_IDLE);
    }
    else
        player->SetCurrentTask(PLAYER_TASK_IDLE);
}

void World::OnTaskWaitSkill(const uint32_t playerID, const uint32_t targetID, const uint32_t skillID,
        const boost::system::error_code &error)
{
    boost::shared_ptr<Player> player = FindPlayer_ByID(playerID);

    if (!player)
        return;

    player->SetCurrentTask(PLAYER_TASK_IDLE);

    if (!error)
    {
        if (skillID)
        {
            InitTaskCastSkill(playerID,targetID,skillID,Coord());
        }
        else
        {
            if (player->GetQueueSkill() && player->GetQueueSkill() != 0xFFFFFFFF)
            {
                InitTaskCastSkill(playerID,player->GetQueueTarget(),player->GetQueueSkill(),Coord());

                player->SetQueueSkill(0,0);
            }
            else
            {
                m_io_service.post(boost::bind(&World::InitTaskIdle,this,playerID));
            }
        }
    }
}

void World::OnTaskAlchemy (const uint32_t playerID, uint8_t ipos, uint8_t epos, uint8_t lpos, bool advanced,
        const boost::system::error_code &error)
{
    boost::shared_ptr<Player> player = FindPlayer_ByID(playerID);

    if (!player)
        return;

    player->SetCurrentTask(PLAYER_TASK_IDLE);

    if (!error)
    {
        boost::shared_ptr<Storage> inv = player->get_storage(STORAGE_INVENTORY);

        Storage::iterator item_iter = inv->find(ipos);

        bool ret = item_iter->second.EnchantInc(lpos != 0);

        boost::shared_ptr<OPacket> pkt(new OPacket);

        srv_pkt::ItemUpdate(pkt,ret,item_iter->second);
        m_server->Send(pkt,player->ConnectionID());

        pkt.reset(new OPacket);

        srv_pkt::ItemEnchant(pkt,epos);
        m_server->Send(pkt,player->ConnectionID());

        inv->RemoveItem(epos);

        if (lpos != 0)
        {
            pkt.reset(new OPacket);

            srv_pkt::ItemEnchant(pkt,lpos);
            m_server->Send(pkt,player->ConnectionID());

            inv->RemoveItem(lpos);
        }

//        m_server->GetService().post(boost::bind<void>(DB::ITEM::EnchantLv(),boost::ref(m_server->DBConnection()),
//            item_iter->second.ID(),item_iter->second.get_enchant_lv()));

        m_io_service.post(boost::bind(&World::InitTaskIdle,this,playerID));
    }
}

void World::OnTaskIdle (const uint32_t playerID, const boost::system::error_code &error)
{
    if (!error)
    {
        boost::shared_ptr<Player> player = FindPlayer_ByID(playerID);

        if (!player || player->IsDead() || player->get_attack_state() != ATTACK_ACTION_STOP)
            return;

        std::map<uint32_t,Status> status = player->GetStatus();

        uint8_t type = 0;

        boost::shared_ptr<OPacket> pkt;

        if (player->hp() < player->max_hp())
        {
            double amount = 0.0004373218*(std::pow((float)player->get_str(),3))
                -0.028104191*(std::pow((float)player->get_str(),2))+1.50593656*player->get_str();

            player->heal_hp((uint32_t)amount);

            type = status.empty() ? srv_pkt::STAT_HP : srv_pkt::STAT_HP | srv_pkt::STAT_STATUS;

            pkt.reset(new OPacket);

            srv_pkt::HPMP(pkt,player->GetUniqueID(),srv_pkt::BASE_OP_INC_SELF,status.size(),type,player->hp(),
                player->GetEncodedStatus(),status);

            m_server->SendChannel(pkt,player->get_position().get_zone());
        }

        if (player->mp() < player->max_mp())
        {
            double amount = 0.0004373218*(std::pow((float)player->get_int(),3))
                -0.028104191*(std::pow((float)player->get_int(),2))+1.50593656*player->get_int();

            player->heal_mp((uint32_t)amount);

            type = status.empty() ? srv_pkt::STAT_MP : srv_pkt::STAT_MP | srv_pkt::STAT_STATUS;

            pkt.reset(new OPacket);

            srv_pkt::HPMP(pkt,player->GetUniqueID(),srv_pkt::BASE_OP_INC_SELF,status.size(),type,player->mp(),
                player->GetEncodedStatus(),status);

            m_server->SendChannel(pkt,player->get_position().get_zone());
        }

        boost::shared_ptr<Party> party = player->get_party();

        if (party)
        {
            party->SendHPMPNotification(player->GetUniqueID(),player->hp(),player->max_hp(),
                player->mp(),player->max_mp());
        }

        m_io_service.post(boost::bind(&World::InitTaskIdle,this,playerID));
    }
}

void World::OnTaskIdleNPC (boost::shared_ptr<NPC> &npc, const boost::system::error_code &error)
{
    if (!error)
    {
        if (!npc)
            return;

        m_io_service.post(boost::bind(&World::InitTaskAI,this,npc));
    }
}

void World::OnTaskExecCastNPC (boost::shared_ptr<NPC> &npc, const uint32_t attackID,
        const boost::system::error_code &error)
{
    if (!error)
    {
        if (!npc)
            return;

        if (npc->IsDead())
        {
            npc->set_destination(Coord());
            return;
        }

        if (npc->hasStatus(STATUS_STUN | STATUS_FROZE | STATUS_PETRIFY | STATUS_SLEEP | STATUS_BIND))
        {
            npc->SetCurrentTask(MOB_TASK_IDLE);
            m_io_service.post(boost::bind(&World::InitTaskIdleNPC,this,npc));
            return;
        }

        uint32_t targetID = npc->get_selected_object();

        boost::shared_ptr<NPC> target = GetTarget(targetID,npc->get_position());

        if (!target || target->IsDead())
        {
            npc->SetCurrentTask(MOB_TASK_IDLE);
            m_io_service.post(boost::bind(&World::InitTaskIdleNPC,this,npc));
            return;
        }

        if (target->get_attribute_state() == ATTRIBUTE_ACTION_INVUNERABLE
            || target->get_attribute_state() == ATTRIBUTE_ACTION_INVISIBLE)
        {
            npc->SetCurrentTask(MOB_TASK_IDLE);
            m_io_service.post(boost::bind(&World::InitTaskIdleNPC,this,npc));
            return;
        }

        /// CHECK FOR KD!!

        boost::shared_ptr<Skill> skill = m_SkillMgr->find(npc->GetQueueSkill());

        switch(skill->ActionType())
        {
        case SKILL_ACTION_ATTACK:
            OnAttackTarget(npc,skill,target,attackID);
            break;
        case SKILL_ACTION_BUFF:
            OnBuffTarget(npc,skill,target,attackID,targetID);
            break;
        case SKILL_ACTION_SUMMON:
            {
                boost::shared_ptr<OPacket> pkt(new OPacket);

                srv_pkt::CastInfo info;
                info.attackID = attackID;
                info.targetID = targetID;
                info.dest = target->get_position();

                srv_pkt::SkillCast_2nd(pkt,info);
                m_server->SendChannel(pkt,target->get_position().get_zone());

                if (rand()%100 < 5)
                {
                    bool next = false;
                    boost::shared_ptr<NPC> nnpc;
                    std::vector<boost::shared_ptr<NPC> > npcs;
                    std::vector<NPC_SPAWN_INFO>::const_iterator it;

                    for (it = skill->m_spawn_list.begin(); it != skill->m_spawn_list.end(); ++it)
                    {
                        next = false;

                        for (int i = 0; i < it->GiantCount; ++i)
                        {
                            nnpc = m_CharacterFactory->Create(it->refCharID);

                            if (!nnpc)
                            {
                                next = true;
                                break;
                            }

                            nnpc->set_position(npc->get_position());
                            npcs.push_back(nnpc);
                        }

                        if (next)
                            continue;

                        for (int i = 0; i < it->ChampionCount; ++i)
                        {
                            nnpc = m_CharacterFactory->Create(it->refCharID);

                            if (nnpc)
                            {
                                nnpc->set_position(npc->get_position());
                                npcs.push_back(nnpc);
                            }
                        }

                        for (int i = 0; i < it->NormalCount; ++i)
                        {
                            nnpc = m_CharacterFactory->Create(it->refCharID);

                            if (nnpc)
                            {
                                nnpc->set_position(npc->get_position());
                                npcs.push_back(nnpc);
                            }
                        }
                    }

                    m_io_service.post(boost::bind(&World::InsertNPC,this,npcs,npc->get_position()));
                }
            }
            break;
        default:
            break;
        }

        m_io_service.post(boost::bind(&World::InitTaskWaitCastNPC,this,npc,skill->next_delay(),
            skill->next_skill()));
    }
}

void World::OnTaskWaitCastNPC (boost::shared_ptr<NPC> &npc, const uint32_t skillID,
    const boost::system::error_code &error)
{
    if (!npc)
        return;

    npc->SetCurrentTask(MOB_TASK_IDLE);

    if (!error)
    {
        if (skillID)
        {
            m_io_service.post(boost::bind(&World::InitTaskExecCastNPC,this,npc,skillID));
        }
        else
        {
            m_io_service.post(boost::bind(&World::InitTaskCastNPC,this,npc));
        }
    }
}

void World::OnTaskMoveNPC (boost::shared_ptr<NPC> &npc, uint32_t dt, const boost::system::error_code &error)
{
    if (!npc)
        return;

    if (!error)
    {
        if (npc->IsDead())
        {
            npc->set_destination(Coord());
            return;
        }

        if(!OnMoveNPC(npc,dt))
        {
            if (npc->hasStatus(STATUS_GROUP_RESTRAIN | STATUS_FEAR))
            {
                npc->SetCurrentTask(MOB_TASK_IDLE);
                m_io_service.post(boost::bind(&World::InitTaskIdleNPC,this,npc));
                return;
            }

            m_io_service.post(boost::bind(&World::InitTaskMoveNPC,this,npc));
        }
        else
        {
            switch(npc->type())
            {
            case ET_MOB:
            case ET_MOB_QUEST:
            case ET_MOB_EVENT:
                {
                    boost::shared_ptr<Mob> mob = boost::shared_dynamic_cast<Mob,NPC>(npc);

                    if (mob)
                    {
                        if (mob->GetCurrentTask() == MOB_TASK_MOVE_TO_TARGET)
                            m_io_service.post(boost::bind(&World::InitTaskExecCastNPC,this,npc,0));
                        else
                            m_io_service.post(boost::bind(&World::InitTaskAI,this,npc));
                    }
                }
                break;
            default:
                break;
            }
        }
    }
    else
    {
        boost::mutex::scoped_lock lock(m_task_mutex);

        timer_iterator iter = m_tasks.find(npc->GetUniqueID());

        if (iter == m_tasks.end())
            return;

        uint32_t secs = iter->second->expires_from_now().total_milliseconds();

        if (secs > dt)
            dt = 0;
        else
            dt -= secs;

        OnMoveNPC(npc,dt);
    }
}

void World::OnTaskTraceNPC (boost::shared_ptr<NPC> &npc, const uint32_t playerID,
        const boost::system::error_code &error)
{
    if (!error)
    {
        boost::shared_ptr<Player> player = FindPlayer_ByID(playerID);

        if (!player || !npc || npc->IsDead())
            return;

        if (npc->hasStatus(STATUS_GROUP_RESTRAIN))
        {
            m_io_service.post(boost::bind(&World::InitTaskTraceNPC,this,npc,playerID));
            return;
        }

        if (distance(player->get_position(),npc->get_position()) > 50)
        {
            Coord pos = RangePosition(npc->get_position(),player->get_position(),40,NULL);

            Coord mpos;
            if (!pos.get_zone() ||
                !m_MapMgr->ResolveMotion(npc->get_position(),player->get_position(),&mpos))
                m_io_service.post(boost::bind(&World::InitTaskTraceNPC,this,npc,playerID));

            pos = mpos;

            npc->set_destination(pos);

            boost::shared_ptr<OPacket> pkt(new OPacket);

            if (pkt)
            {
                srv_pkt::Movement(pkt,npc->GetUniqueID(),pos);
                m_server->SendChannel(pkt,npc->get_position().get_zone());
            }

            m_io_service.post(boost::bind(&World::InitTaskTraceMoveNPC,this,npc,playerID));
        }
        else
        {
            m_io_service.post(boost::bind(&World::InitTaskTraceNPC,this,npc,playerID));
        }
    }
}

void World::OnTaskTraceMoveNPC (boost::shared_ptr<NPC> &npc, const uint32_t playerID, uint32_t dt,
        const boost::system::error_code &error)
{
    if (!npc)
        return;

    if (!error)
    {
        boost::shared_ptr<Player> player = FindPlayer_ByID(playerID);

        if (!player || npc->IsDead())
            return;

        if (!OnMoveNPC(npc,dt))
        {
            if (npc->hasStatus(STATUS_GROUP_RESTRAIN))
            {
                npc->set_destination(Coord());
                m_io_service.post(boost::bind(&World::InitTaskTraceNPC,this,npc,playerID));
                return;
            }

            m_io_service.post(boost::bind(&World::InitTaskTraceMoveNPC,this,npc,playerID));
        }
        else
            m_io_service.post(boost::bind(&World::InitTaskTraceNPC,this,npc,playerID));
    }
}

void World::OnTaskDespawn (const uint32_t objID, const int16_t zoneID, bool IsNPC,
    const boost::system::error_code &error)
{
    if (!error)
    {
        boost::shared_ptr<Zone> zone = m_MapMgr->Find(zoneID);

        if (zone)
        {
            boost::shared_ptr<OPacket> pkt(new OPacket);

            if (IsNPC)
            {
                RemoveNPC(objID,Coord(0,0,0,zoneID));
            }
            else
            {
                RemoveItem(objID,Coord(0,0,0,zoneID));
            }
        }
    }
}

void World::OnBackgroundTask (const boost::system::error_code &error)
{
    if (!error)
    {
        boost::mutex::scoped_lock lock(m_bg_task_mutex);

        OPacket pkt;
        std::pair<uint32_t,uint32_t> ret;
        boost::unordered_map<uint32_t,BTask>::iterator iter;

        for ( iter = m_bg_tasks.begin(); iter != m_bg_tasks.end(); ++iter)
        {
            --iter->second.seconds;

            if (!iter->second.seconds)
            {
                m_io_service.post(boost::bind(&World::OnBackgroundTaskRemove,this,iter->first));

                m_io_service.post(boost::bind(&World::OnBackgroundTaskExec,this,iter->second));                
            }
        }

        m_bg_task_timer.expires_from_now(boost::posix_time::seconds(1));
        m_bg_task_timer.async_wait(boost::bind(&World::OnBackgroundTask,this,boost::asio::placeholders::error));
    }
}

void World::OnBackgroundTaskExec (const BTask &task)
{
    boost::shared_ptr<OPacket> pkt;
    std::pair<uint32_t,uint32_t> ret;

    boost::shared_ptr<NPC> entity = task.entity.lock();

    if (!entity)
        return;

    switch(task.task)
    {
    case BG_TASK_COOLDOWN_ITEM:
        {
            boost::shared_ptr<Player> player;
            player = boost::shared_dynamic_cast<Player,NPC>(entity);

            if (player)
                player->RemoveItemCooldown(task.arg);
        }
        break;
    case BG_TASK_COOLDOWN_SKILL:
        entity->RemoveSkillCooldown(task.arg);
        break;
    case BG_TASK_COOLDOWN_ATTACK:
        switch(entity->get_attack_state())
        {
        case ATTACK_ACTION_START:
            {
                if (entity->IsDead())
                {
                    entity->set_attack_state(ATTACK_ACTION_STOP);
                }
                else
                {
                    entity->set_attack_state(ATTACK_ACTION_MIDDLE);

                    BTask new_task = task;
                    new_task.seconds = 10;

                    m_io_service.post(boost::bind(&World::InitBackgroundTask,this,new_task,true));
                }
            }
            break;
        case ATTACK_ACTION_MIDDLE:
            entity->set_attack_state(ATTACK_ACTION_STOP);

            if (entity->type() == ET_PLAYER)
            {
                boost::shared_ptr<Player> player = FindPlayer_ByID(entity->GetUniqueID());

                if (!player || player->IsDead())
                    return;

                pkt.reset(new OPacket);

                srv_pkt::State(pkt,entity->GetUniqueID(),STATE_ATTACK,ATTACK_ACTION_STOP);
                m_server->SendChannel(pkt,entity->get_position().get_zone());
            }
            break;
        default:
            break;
        }
        break;
    case BG_TASK_IMBUE:
        ret = entity->remove_imbue(task.arg);

        if (ret.second)
        {
            pkt.reset(new OPacket);

            srv_pkt::TermBuff(pkt,ret.second);
            m_server->SendChannel(pkt,entity->get_position().get_zone());
        }
        break;
    case BG_TASK_BUFF:
        ret = entity->remove_buff(task.arg);

        if (ret.second)
        {
            pkt.reset(new OPacket);

            srv_pkt::TermBuff(pkt,ret.second);
            m_server->SendChannel(pkt,entity->get_position().get_zone());
        }
        break;
    case BG_TASK_STUN:
        entity->SetCurrentTask(0);  /// MARK AS IDLE!!

        switch(entity->type())
        {
        case ET_MOB:
        case ET_MOB_QUEST:
        case ET_MOB_EVENT:
            m_io_service.post(boost::bind(&World::InitTaskCastNPC,this,entity));
            break;
        default:
            break;
        }

        break;
    case BG_TASK_STATUS:
        ret = entity->RemoveStatus(task.arg);

        if (entity->type() == ET_PLAYER)
        {
            boost::shared_ptr<Player> player = boost::shared_static_cast<Player,NPC>(entity);

            if (player)
            {
                pkt.reset(new OPacket);

                srv_pkt::PlayerStatus(pkt,player->GetEncodedStatus());
                m_server->Send(pkt,player->ConnectionID());
            }
        }

        pkt.reset(new OPacket);

        srv_pkt::BadStatus(pkt,entity->GetUniqueID(),entity->GetEncodedStatus(),entity->GetStatus());
        m_server->SendChannel(pkt,entity->get_position().get_zone());

        /// TERMINATE PAYLOAD TASK IF ITS OVER
        if (ret.second)
            TerminateBackgroundTask(ret.second);

        break;
    case BG_TASK_CONSUME_MP:
        if (entity->mp() < task.arg)
        {
            ret = entity->remove_buff(task.arg1);

            pkt.reset(new OPacket);

            srv_pkt::TermBuff(pkt,ret.second);
            m_server->SendChannel(pkt,entity->get_position().get_zone());
        }
        else
        {
            entity->consume_mp(task.arg);

            std::map<uint32_t,Status> status = entity->GetStatus();

            uint8_t type = status.empty() ? srv_pkt::STAT_MP : srv_pkt::STAT_MP | srv_pkt::STAT_STATUS;

            pkt.reset(new OPacket);

            srv_pkt::HPMP(pkt,entity->GetUniqueID(),srv_pkt::BASE_OP_DEC_MP,status.size(),type,entity->mp(),
                entity->GetEncodedStatus(),status);

            m_server->SendChannel(pkt,entity->get_position().get_zone());

            boost::shared_ptr<Skill> skill = m_SkillMgr->find(task.arg1);

            if (skill)
            {
                BTask new_task = task;
                
                new_task.seconds = skill->m_MPConsumeTime;

                m_io_service.post(boost::bind(&World::InitBackgroundTask,this,new_task,false));
            }
        }
        break;
    case BG_TASK_STATUS_DAMAGE:
        {
            if (entity->IsDead())
                return;

            entity->consume_hp(task.arg);

            pkt.reset(new OPacket);

            srv_pkt::StatusDmg(pkt,entity->GetUniqueID(),task.arg);
            m_server->SendChannel(pkt,entity->get_position().get_zone());

            std::map<uint32_t,Status> status = entity->GetStatus();

            uint8_t type = status.empty() ? srv_pkt::STAT_HP : srv_pkt::STAT_HP | srv_pkt::STAT_STATUS;

            pkt.reset(new OPacket);

            srv_pkt::HPMP(pkt,entity->GetUniqueID(),srv_pkt::BASE_OP_DEC_HP,status.size(),type,entity->hp(),
                entity->GetEncodedStatus(),status);

            m_server->SendChannel(pkt,entity->get_position().get_zone());

            BTask new_task = task;
            new_task.seconds = 5;

            m_io_service.post(boost::bind(&World::InitBackgroundTask,this,new_task,false));
        }
        break;
    case BG_TASK_SPAWN_JOB_NPC:
        {
            enum
            {
                CH_THIEF_NPC = 2222,
                CH_HUNTER_NPC = 7828,
                EU_THIEF_NPC = 16111,
                EU_HUNTER_NPC = 17511
            };

            if (entity->IsDead())
                return;

            boost::shared_ptr<Transport> transport = boost::shared_static_cast<Transport,NPC>(entity);

            if (transport)
            {
                if (!IsInsideTown(transport->get_position()))
                {
                    boost::shared_ptr<Storage> storage = transport->GetStorage();

                    if (!storage || storage->empty())
                        return;

                    uint8_t scale = transport->TradeScale();

                    boost::shared_ptr<Player> owner = transport->GetOwner();

                    if (!owner)
                        return;

                    uint32_t refID = 0;

                    switch(owner->get_job().get_type())
                    {
                    case JOB_THIEF:
                        refID = CH_HUNTER_NPC;
                        break;
                    case JOB_TRADER:
                        refID = CH_THIEF_NPC;
                        break;
                    default:
                        break;
                    }

                    if (!refID)
                        return;

                    refID += 10*(owner->level()-1);

                    Coord new_pos;
                    boost::shared_ptr<NPC> jnpc;

                    for (int n = 0; n < scale*2; ++n)
                    {
                        jnpc = m_CharacterFactory->Create(refID+rand()%9);

                        if (!jnpc)
                            return;

                        new_pos = transport->get_position();

                        new_pos.set_x(new_pos.get_x() + (rand()%40)*sin((rand()%360)*3.1415/180));
                        new_pos.set_y(new_pos.get_y() + (rand()%40)*sin((rand()%360)*3.1415/180));

                        if (!IsInsideTown(new_pos))
                        {
                            jnpc->set_position(new_pos);

                            m_io_service.post(boost::bind(&World::InsertNPC,this,jnpc));
                        }
                    }
                }

                BTask new_task = task;
                new_task.seconds = 30;

                m_io_service.post(boost::bind(&World::InitBackgroundTask,this,new_task,false));
            }
        }
        break;
    default:
        break;
    }
}

void World::OnBackgroundTaskRemove (const uint32_t taskID)
{
    boost::mutex::scoped_lock lock(m_bg_task_mutex);

    m_bg_tasks.erase(taskID);
}

void World::OnStateTask(const boost::system::error_code &error)
{
    if (!error)
    {
        boost::mutex::scoped_lock lock(m_state_task_mutex);

        boost::unordered_map<uint32_t,STask>::iterator iter;

        for (iter = m_state_tasks.begin(); iter != m_state_tasks.end(); ++iter)
        {
            --iter->second.seconds;

            if (!iter->second.seconds)
            {
                m_io_service.post(boost::bind(&World::OnStateTaskRemove,this,iter->first));
                m_io_service.post(boost::bind(&World::OnStateTaskExec,this,iter->second));
            }
        }

        m_state_task_timer.expires_from_now(boost::posix_time::seconds(1));
        m_state_task_timer.async_wait(boost::bind(&World::OnStateTask,this,boost::asio::placeholders::error));
    }
}

void World::OnStateTaskRemove (const uint32_t ID)
{
    boost::mutex::scoped_lock lock(m_state_task_mutex);

    m_state_tasks.erase(ID);
}

void World::OnStateTaskExec (const STask &task)
{
    boost::shared_ptr<Player> player = FindPlayer_ByID(task.playerID);

    if (player && !player->IsDead())
    {
        switch(task.task)
        {
        case STATE_TASK_BERSERK:
            player->berserk(false);
            break;
        case STATE_TASK_INVUNERABLE:
            player->clear_invulnerable();
            break;
        default:
            break;
        }
    }
}

void World::OnRespawnMob (const boost::system::error_code &error)
{
    if (!error)
    {
        boost::mutex::scoped_lock(m_spawn_mutex);

        std::vector<SpawnPoint>::const_iterator siter;
        std::deque<std::pair<uint32_t,size_t> >::iterator it;
        for (it = m_spawn_deque.begin(); it != m_spawn_deque.end();)
        {
            --it->first;

            if (!it->first)
            {
                if (it->second >= m_spawn_list.size())
                {
                    it = m_spawn_deque.erase(it);
                    continue;
                }

                siter = m_spawn_list.begin() + it->second;

                if (siter != m_spawn_list.end())
                {
                    boost::shared_ptr<NPC> npc = m_CharacterFactory->Create(siter->refCharID);

                    if (npc)
                    {
                        npc->set_position(Coord(siter->x,siter->y,siter->z,siter->Zone));

                        boost::shared_ptr<Mob> mob = boost::shared_dynamic_cast<Mob,NPC>(npc);

                        if (mob)
                            mob->SetSpawnPoint(it->second);

                        m_io_service.post(boost::bind(&World::InsertNPC,this,npc));
                    }
                }

                it = m_spawn_deque.erase(it);
            }
            else
                ++it;
        }

        m_spawn_timer.expires_from_now(boost::posix_time::seconds(1));
        m_spawn_timer.async_wait(boost::bind(&World::OnRespawnMob,this,boost::asio::placeholders::error));
    }
}

void World::OnActivateZoneAI (const int16_t zoneID)
{
#if defined DEBUG
    syslog(LOG_INFO,"OnActivateZoneAI - ZoneID = %i",zoneID);
#endif

    boost::shared_ptr<Zone> zone = m_MapMgr->Find(zoneID);

    if (zone && zone->Count())
    {
        NearObjects objs;

        zone->GetNearObjects(0,&objs);

        std::vector<boost::shared_ptr<NPC> >::iterator it;
        for (it = objs.npc_list.begin(); it != objs.npc_list.end(); ++it)
        {
            switch((*it)->type())
            {
            case ET_MOB:
            case ET_MOB_QUEST:
            case ET_MOB_EVENT:
                m_io_service.post(boost::bind(&World::InitTaskAI,this,*it));
                break;
            default:
                break;
            }
        }
    }
}

void World::OnDeactivateZoneAI (const int16_t zoneID)
{
#if defined DEBUG
    syslog(LOG_INFO,"OnDeactivateZoneAI - ZoneID = %i",zoneID);
#endif

    boost::shared_ptr<Zone> zone = m_MapMgr->Find(zoneID);

    if (zone && !zone->Count())
    {
        NearObjects objs;

        zone->GetNearObjects(0,&objs);

        std::vector<boost::shared_ptr<NPC> >::iterator it;
        for (it = objs.npc_list.begin(); it != objs.npc_list.end(); ++it)
        {
            switch((*it)->type())
            {
            case ET_MOB:
            case ET_MOB_QUEST:
            case ET_MOB_EVENT:
                m_io_service.post(boost::bind(&World::TerminateTask,this,(*it)->GetUniqueID()));
                break;
            default:
                break;
            }
        }
    }
}

void World::OnSignalMobDead (const uint32_t entityID, const int16_t zoneID)
{
    boost::shared_ptr<Zone> zone = m_MapMgr->Find(zoneID);

    if (!zone)
        return;

    boost::shared_ptr<NPC> npc = zone->FindNPC(entityID);

    if (!npc)
        return;

    boost::shared_ptr<Mob> mob = boost::shared_dynamic_cast<Mob,NPC>(npc);

    if (!mob)
        return;

    boost::mutex::scoped_lock lock(m_task_mutex);

    timer_iterator iter = m_tasks.find(entityID);

    if (iter != m_tasks.end())
    {
        iter->second->cancel();

        iter->second->expires_from_now(boost::posix_time::seconds(5));
        iter->second->async_wait(boost::bind(&World::OnTaskDespawn,this,entityID,zoneID,true,
            boost::asio::placeholders::error));
    }

    lock.unlock();

    if (mob->IsRespawnable())
    {
        boost::mutex::scoped_lock lock(m_spawn_mutex);

        m_spawn_deque.push_back(std::make_pair(rand()%5,mob->GetSpawnPoint()));
    }

    std::vector<boost::shared_ptr<DropableItem> > dlist(GetDropList(npc->get_id(),npc->level(),mob->Quality()));

    boost::shared_ptr<OPacket> pkt;
    boost::shared_ptr<Player> player;
    std::map<uint32_t,uint32_t> exp_table = npc->experience_gained();

    bool first = true;
    for (std::map<uint32_t,uint32_t>::const_iterator it = exp_table.begin(); it != exp_table.end(); ++it)
    {
        player = FindPlayer_CloseToPosition(it->first,npc->get_position());

        if (player)
        {
            if (player->level() < GetMaxCap())
            {
                boost::unordered_map<uint8_t,LevelProperty>::const_iterator xpiter = m_XPChart.find(player->level());

                if (xpiter == m_XPChart.end())
                {
                    syslog(LOG_INFO,"XP information for level %i is missing.",player->level());
                    continue;
                }

                uint64_t xp = ExpGained(it->second,player->level(),0,npc->level())*10;

                switch(mob->Quality())
                {
                case CH_QUALITY_CHAMPION:
                    xp *= 2;
                    break;
                case CH_QUALITY_GIANT:
                    xp *= 15;
                    break;
                default:
                    break;
                }

                uint16_t spexp = xp*100;

                uint64_t totalxp = player->get_exp() + xp;

                if( totalxp >= xpiter->second.RequiredXP)
                {
                    totalxp -= xpiter->second.RequiredXP;

                    player->levelup();

                    player->inc_exp(totalxp);

                    player->inc_spexp(spexp);

                    pkt.reset(new OPacket);

                    srv_pkt::PlayerXPSP(pkt,entityID,xp,spexp,player->get_stat());
                    m_server->Send(pkt,player->ConnectionID());

                    pkt.reset(new OPacket);

                    srv_pkt::PlayerLevel(pkt,player->GetUniqueID());
                    m_server->SendChannel(pkt,player->get_position().get_zone());

                    player->RecalculateStats();

                    player->heal_mp(player->max_hp());

                    std::map<uint32_t,Status> status = player->GetStatus();

                    uint8_t type = status.empty() ? srv_pkt::STAT_HP : srv_pkt::STAT_HP | srv_pkt::STAT_STATUS;

                    pkt.reset(new OPacket);

                    srv_pkt::HPMP(pkt,player->GetUniqueID(),srv_pkt::BASE_OP_INC_POT,status.size(),type,player->hp(),
                        player->GetEncodedStatus(),status);

                    m_server->SendChannel(pkt,player->get_position().get_zone());

                    player->heal_mp(player->max_mp());

                    type = status.empty() ? srv_pkt::STAT_MP : srv_pkt::STAT_MP | srv_pkt::STAT_STATUS;

                    pkt.reset(new OPacket);

                    srv_pkt::HPMP(pkt,player->GetUniqueID(),srv_pkt::BASE_OP_INC_POT,status.size(),type,player->mp(),
                        player->GetEncodedStatus(),status);

                    m_server->SendChannel(pkt,player->get_position().get_zone());

                    boost::shared_ptr<Party> party = player->get_party();

                    if (party)
                    {
                        party->SendLevelNotification(player->GetUniqueID(),player->level());
                        party->SendHPMPNotification(player->GetUniqueID(),player->hp(),player->max_hp(),
                            player->mp(),player->max_mp());
                    }

                    boost::shared_ptr<Guild> guild = player->get_guild();

                    if (guild)
                        guild->member_level(player->get_name(),player->level());
                }
                else
                {
                    pkt.reset(new OPacket);

                    srv_pkt::PlayerXPSP(pkt,entityID,xp,spexp);
                    m_server->Send(pkt,player->ConnectionID());

                    player->inc_exp(xp);
                }
            }

            if (first)
            {
                if (mob->Quality() == CH_QUALITY_UNIQUE)
                {
                    pkt.reset(new OPacket);

                    srv_pkt::Unique(pkt,npc->get_id(),player->get_name());
                    m_server->Broadcast(pkt);

                    int index = MAX_UNIQUES;
                    boost::asio::deadline_timer *unique_timer = 0;

                    switch(entityID)
                    {
                    case MOB_CH_TIGERWOMAN:
                        index = TIGERWOMAN;
                        unique_timer = &m_time_tg;
                        break;
                    case MOB_OA_URUCHI:
                        index = URUCHI;
                        unique_timer = &m_time_uru;
                        break;
                    case MOB_KK_ISYUTARU:
                        index = ISYUTARU;
                        unique_timer = &m_time_isy;
                        break;
                    case MOB_EU_KERBEROS:
                        index = CERBERUS;
                        unique_timer = &m_time_cerb;
                        break;
                    case MOB_AM_IVY:
                        index = IVY;
                        unique_timer = &m_time_ivy;
                        break;
                    default:
                        break;
                    }

                    if (unique_timer && index >= TIGERWOMAN && index < MAX_UNIQUES)
                    {
                        unique_timer->expires_from_now(boost::posix_time::hours(rand()%4+1));
                        unique_timer->async_wait(boost::bind(&World::OnSpawnUnique,this,index,entityID,
                            boost::asio::placeholders::error));

                        m_unique_spawned[index] = false;
                    }
                }

                for (size_t i = 0; i < dlist.size(); ++i)
                {
                    Coord new_pos = npc->get_position();

                    new_pos.set_x(new_pos.get_x() + (rand()%(5+i))*sin((rand()%360)*3.1415/180));
                    new_pos.set_y(new_pos.get_y() + (rand()%(5+i))*sin((rand()%360)*3.1415/180));

                    dlist[i]->set_position(new_pos);
                    dlist[i]->m_OwnerID = player->GetUniqueID();

                    InsertItem(dlist[i]);
                }

                if (player->get_attribute_state() != ATTRIBUTE_ACTION_BERSERK)
                {
                    if (player->get_berserk_count() < MAX_PLAYER_BERSERK_COUNT && rand() % 100 <= 20)
                    {
                        player->set_berserk_count(player->get_berserk_count()+1);

                        pkt.reset(new OPacket);

                        srv_pkt::PlayerBerserkGauge(pkt,player->get_berserk_count(),entityID);
                        m_server->Send(pkt,player->ConnectionID());
                    }
                }

                first = false;
            }
        }
    }
}

void World::OnSignalTransportDead (const uint32_t transportID, const int16_t zoneID)
{
    boost::shared_ptr<Zone> zone = m_MapMgr->Find(zoneID);

    if (!zone)
        return;

    boost::shared_ptr<NPC> npc = zone->FindNPC(transportID);

    if (!npc)
        return;

    boost::shared_ptr<Transport> transport = boost::shared_dynamic_cast<Transport,NPC>(npc);

    if (!transport)
        return;

    boost::shared_ptr<Player> player = transport->GetOwner();

    if (player)
    {
        Coord pos = transport->get_position();

        boost::shared_ptr<OPacket> pkt(new OPacket);

        if (!player->IsDead())
        {
            srv_pkt::TransportAction(pkt,player->GetUniqueID(),srv_pkt::TRANSPORT_ACTION_UNMOUNT,transportID);
            m_server->SendChannel(pkt,player->get_position().get_zone());

            pkt.reset(new OPacket);
        }

        srv_pkt::TransportStatsRemove(pkt,transportID);
        m_server->Send(pkt,player->ConnectionID());

        Job job = player->get_job();

        boost::shared_ptr<srv::IConnection> conn = m_server->FindConnection(player->ConnectionID());

        if (conn)
        {
            uint32_t accountID = boost::shared_static_cast<srv::Connection,srv::IConnection>(conn)->AccountID();

            boost::shared_ptr<DropableItem> item;
            boost::shared_ptr<Storage> storage = transport->GetStorage();

            if (storage)
            {
                Coord new_pos;

                for (Storage::iterator it = storage->begin(); it != storage->end(); ++it)
                {
                    item.reset(new DropableItem());

                    if (item)
                    {
                        /// DROP ITEMS IN A VALID POSITION
                        Coord mpos;

                        do
                        {
                            new_pos.set_x(pos.get_x() + (rand()%10)*sin((rand()%360)*3.1415/180));
                            new_pos.set_y(pos.get_y() + (rand()%10)*sin((rand()%360)*3.1415/180));
                        }
                        while (!m_server->GetWorld()->resolveMotion(pos,new_pos,&mpos));

                        new_pos = mpos;

                        item->m_Item = it->second;
                        item->m_OwnerAlias = job.get_alias();
                        item->m_OwnerAuth = accountID;
                        item->m_OwnerID = player->GetUniqueID();
                        item->set_position(new_pos);

                        m_server->GetWorld()->InsertItem(item);
                    }
                }
            }
        }
    }

    m_io_service.post(boost::bind(&World::RemoveNPC,this,transportID,transport->get_position()));

    if (player)
    {
        transport.reset();
        player->SetTransport(transport);
    }
}

void World::OnSignalHPMP (const uint32_t npcID, const int16_t zoneID)
{
    boost::shared_ptr<Zone> zone = m_MapMgr->Find(zoneID);

    if (zone)
    {
        boost::shared_ptr<NPC> npc = zone->FindNPC(npcID);

        if (!npc)
            return;

        boost::shared_ptr<Mob> mob = boost::shared_dynamic_cast<Mob,NPC>(npc);

        if (!mob || mob->IsDead())
            return;

        if (mob->IsTaskInterruptable())
        {
            TerminateTask(mob->GetUniqueID());
            m_io_service.post(boost::bind(&World::InitTaskCastNPC,this,mob));
        }
    }
}

void World::SpawnUniqueRandomPosition (const uint32_t refCharID)
{
    std::multimap<uint32_t,SpawnPoint>::const_iterator first = m_UniqueSpawnPoints.find(refCharID);

    if (first == m_UniqueSpawnPoints.end())
        return;

    std::multimap<uint32_t,SpawnPoint>::const_iterator last = m_UniqueSpawnPoints.upper_bound(refCharID);

	size_t count = m_UniqueSpawnPoints.count(refCharID);

    if (!count)
        return;

	size_t i;
	for ( i = 0; first != last, i < rand()%count; ++first, ++i)
		;

	boost::shared_ptr<NPC> unique = m_CharacterFactory->Create(refCharID);

    if (unique)
    {
        unique->set_position(Coord(first->second.x,first->second.y,first->second.z,first->second.Zone));

        InsertNPC(unique);
    }
}

void World::OnSpawnUnique (uint8_t Index, uint32_t refCharID,const boost::system::error_code &error)
{
	if (!error)
	{
		if (!m_unique_spawned[Index])
		{
			m_unique_spawned[Index] = true;

			SpawnUniqueRandomPosition(refCharID);
		}
	}
}

std::vector<boost::shared_ptr<DropableItem> > World::GetDropList (const uint32_t refCharID, uint8_t lv,
    int MobQuality) const
{
    enum
    {
        ITEM_ETC_GOLD_01 = 1,
        ITEM_ETC_GOLD_02 = 2,
        ITEM_ETC_GOLD_03 = 3
    };

    typedef boost::unordered_multimap<uint32_t,Drop> dmap_type;
    typedef std::pair<dmap_type::const_iterator,dmap_type::const_iterator> drange_type;

    typedef boost::unordered_multimap<uint32_t,DropWeight> dqmap_type;
    typedef std::pair<dqmap_type::const_iterator,dqmap_type::const_iterator> dqrange_type;

    drange_type drange = m_DropChart.equal_range(refCharID);
    dqrange_type dqrange = m_DropWeightChart.equal_range(refCharID);

    boost::shared_ptr<DropableItem> ditem;
    std::vector<boost::shared_ptr<DropableItem> > ret;

    int item_quantity = 0;
    int gold_quantity = 0;
    int etc_quantity = 0;

    switch(MobQuality)
    {
    case CH_QUALITY_UNIQUE:
    case CH_QUALITY_LESSER_UNIQUE:
        item_quantity = 10;
        etc_quantity = 10;
        gold_quantity = 20;
        break;
    case CH_QUALITY_NORMAL:
        item_quantity = gold_quantity = etc_quantity = 1;
        break;
    case CH_QUALITY_GIANT:
    case CH_QUALITY_CHAMPION:
    default:
        item_quantity = gold_quantity = etc_quantity = MobQuality;
        break;
    }


    for (dqmap_type::const_iterator dqit = dqrange.first; dqit != dqrange.second; ++dqit)
    {
        int quantity = 0;

        switch(dqit->second.Quality)
        {
        case 1:
            quantity = item_quantity;
            break;
        case 3:
            quantity = etc_quantity;
            break;
        default:
            break;
        }

        for (int i = 0; i < quantity; ++i)
        {
            float prob = rand()%((int)dqit->second.Weight);

            for (dmap_type::const_iterator dit = drange.first; dit != drange.second; ++dit)
            {
                if (dqit->second.Quality == dit->second.Quality)
                {
                    prob -= dit->second.Weight;

                    if (prob <= 0)
                    {
                        try
                        {
                            ditem.reset(new DropableItem);

                            if (!ditem)
                                break;

                            ditem->m_Item = m_ItemFactory->Create(dit->second.refItemID);

                            if (ditem->m_Item.get_id())
                            {
                                ditem->m_Item.set_quantity(1);
                                ret.push_back(ditem);
                            }
                        }
                        catch (const Exception::InvalidItemID &error)
                        {
                        }

                        break;
                    }
                }
            }
        }
    }

    boost::unordered_map<uint8_t,std::pair<uint16_t,uint16_t> >::const_iterator giter = m_GoldChart.find(lv);

    if (giter != m_GoldChart.end())
    {
        uint32_t id = 0;
        uint32_t amount = giter->second.second;

        switch(MobQuality)
        {
        case CH_QUALITY_UNIQUE:
        case CH_QUALITY_LESSER_UNIQUE:
            amount *= 10;
            break;
        case CH_QUALITY_NORMAL:
            break;
        case CH_QUALITY_GIANT:
        case CH_QUALITY_CHAMPION:
        default:
            amount *= MobQuality;
            break;
        }

        if ( amount <= 1000)
            id = ITEM_ETC_GOLD_01;
        else if ( amount <= 10000)
            id = ITEM_ETC_GOLD_02;
        else
            id = ITEM_ETC_GOLD_03;

        for (int i = 0; i < gold_quantity; ++i)
        {
            try
            {
                ditem.reset(new DropableItem);

                if (!ditem)
                    break;

                ditem->m_Item = m_ItemFactory->Create(id);

                if (ditem->m_Item.get_id())
                {
                    ditem->m_GoldAmount = amount;
                    ditem->m_Item.set_quantity(amount);
                    ret.push_back(ditem);
                }
            }
            catch (const Exception::InvalidItemID &error)
            {
            }
        }
    }

    return ret;
}
