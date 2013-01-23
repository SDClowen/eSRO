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

#include "resource_manager.hpp"

/// KHAMAILEON INCLUDES
#include <parser_manager.hpp>
#include <parser/characterdata.hpp>
#include <parser/itemdata.hpp>
#include <parser/skilldata.hpp>
#include <parser/leveldata.hpp>
#include <parser/levelgold.hpp>
#include <parser/npcpos.hpp>
#include <parser/refmappingshopgroup.hpp>
#include <parser/refmappingshopwithtab.hpp>
#include <parser/refpackageitem.hpp>
#include <parser/refpricepolicyofitem.hpp>
#include <parser/refscrapofpackageitem.hpp>
#include <parser/refshopgoods.hpp>
#include <parser/refshopgroup.hpp>
#include <parser/refshoptab.hpp>
#include <parser/teleportbuilding.hpp>
#include <parser/teleportdata.hpp>
#include <parser/teleportlink.hpp>
#include <parser/teleportoptional.hpp>
#include <parser/siegefortress.hpp>

/// SQL INCLUDES
#include <property_query.hpp>
#include "shop_query.hpp"
#include "drop_query.hpp"

/// SOL INCLUDES
#include <entity_property.hpp>
#include <teleport_building.hpp>

#include <boost/bind.hpp>

ResourceManager::ResourceManager (const std::string &path, const std::string &usr, const std::string &pw,
                                  const std::string &host, const std::string &db)
    : m_path(path), m_usr(usr), m_pw(pw), m_host(host), m_db(db), m_db_conn(false)
{
    m_db_conn.connect(db.c_str(),host.c_str(),usr.c_str(),pw.c_str());
}

bool ResourceManager::Initialize ()
{
    using boost::bind;
    using boost::function;

    if (!m_db_conn.connected())
        return false;

    ParserManager manager(m_path);

    manager.attachParser("characterdata.txt",boost::bind(&ResourceManager::OnCharacterdata,this,_1));

    boost::function<void (const ItemProperty&)> itemfunc = boost::bind(&ResourceManager::OnItemdata,this,_1);
    manager.attachParser("itemdata.txt",boost::bind(&ParserItemdata,_1,itemfunc));

    boost::function<void (const SkillProperty&)> skillfunc = boost::bind(&ResourceManager::OnSkilldata,this,_1);
    manager.attachParser("skilldata.txt",boost::bind(&ParserSkilldata,_1,skillfunc));

    boost::function<void (const LevelProperty&)> levelfunc = boost::bind(&ResourceManager::OnLeveldata,this,_1);
    manager.attachParser("leveldata.txt",boost::bind(&ParserLeveldata,_1,levelfunc));

    function<void (const std::pair<uint8_t,uint16_t> &)> goldfunc = bind(&ResourceManager::OnLevelgold,this,_1);
    manager.attachParser("levelgold.txt",bind(&ParserLevelgold,_1,goldfunc));

    manager.attachParser("npcpos.txt",bind(&ResourceManager::OnSpawnpoint,this,_1));

    function<void (const std::pair<std::string,std::string>&)> mgrpfunc = bind(&ResourceManager::OnMapShopGroup,this,_1);
    manager.attachParser("refmappingshopgroup.txt",boost::bind(&ParserRefmappingshopgroup,_1,mgrpfunc));

    function<void (const std::pair<std::string,std::string>&)> mtabfunc = bind(&ResourceManager::OnMapShopTab,this,_1);
    manager.attachParser("refmappingshopwithtab.txt",boost::bind(&ParserRefmappingshopwithtab,_1,mtabfunc));

    function<void (const PackageItem_t&)> pkfunc = bind(&ResourceManager::OnPackageItem,this,_1);
    manager.attachParser("refpackageitem.txt",boost::bind(&ParserRefpackageitem,_1,pkfunc));

    function<void (const ItemPricePolicy_t&)> pricefunc = bind(&ResourceManager::OnPriceItem,this,_1);
    manager.attachParser("refpricepolicyofitem.txt",boost::bind(&ParserRefpricepolicyofitem,_1,pricefunc));

    function<void (const ScrapPackageItem_t&)> scrapfunc = bind(&ResourceManager::OnScrapItem,this,_1);
    manager.attachParser("refscrapofpackageitem.txt",boost::bind(&ParserRefscrapofpackageitem,_1,scrapfunc));

    function<void (const ShopGood_t&)> goodfunc = bind(&ResourceManager::OnShopGood,this,_1);
    manager.attachParser("refshopgoods.txt",boost::bind(&ParserRefshopgoods,_1,goodfunc));

    function<void (const ShopGroup_t&)> groupfunc = bind(&ResourceManager::OnShopGroup,this,_1);
    manager.attachParser("refshopgroup.txt",boost::bind(&ParserRefshopgroup,_1,groupfunc));

    function<void (const ShopTab_t&)> tabfunc = bind(&ResourceManager::OnShopTab,this,_1);
    manager.attachParser("refshoptab.txt",boost::bind(&ParserRefshoptab,_1,tabfunc));

    manager.attachParser("teleportdata.txt",bind(&ResourceManager::OnTeleportdata,this,_1));

    function<void (const TeleportLink&)> linkfunc = bind(&ResourceManager::OnTeleportLink,this,_1);
    manager.attachParser("teleportlink.txt",boost::bind(&ParserTeleportlink,_1,linkfunc));

    function<void (const TeleportOptional&)> optfunc = bind(&ResourceManager::OnTeleportOptional,this,_1);
    manager.attachParser("refoptionalteleport.txt",boost::bind(&ParserOptionalTeleport,_1,optfunc));

    function<void (const BuildingProperty&)> blgfunc = bind(&ResourceManager::OnTeleportBuilding,this,_1);
    manager.attachParser("teleportbuilding.txt",boost::bind(&ParserTeleportBuilding,_1,blgfunc));

    function<void (const FortressProperty&)> fortfunc = bind(&ResourceManager::OnFortressProperty,this,_1);
    manager.attachParser("siegefortress.txt",boost::bind(&ParserSiegefortress,_1,fortfunc));

    manager.loadMedia();

    return true;
}

void ResourceManager::Release ()
{
}

boost::unordered_map<uint32_t,boost::shared_ptr<ItemProperty> > ResourceManager::LoadItemProperties ()
{
    return m_ItemProperties;
}

boost::unordered_map<uint32_t,boost::shared_ptr<EntityProperty> > ResourceManager::LoadCharacterProperties ()
{
    return m_CharProperties;
}

std::multimap<uint32_t,std::pair<uint8_t,uint8_t> > ResourceManager::LoadCharacterQualityChart ()
{
    DB::PROPERTY::CharacterQuality query;
    return query(m_db_conn);
}

std::vector<SkillProperty> ResourceManager::LoadSkillProperties ()
{
    return m_SkillProperties;
}

std::vector<FortressProperty> ResourceManager::LoadFortress ()
{
    return m_FortressProperties;
}

std::vector<boost::shared_ptr<TeleportBuilding> > ResourceManager::LoadTeleportBuildings ()
{
    return m_Buildings;
}

boost::unordered_map<uint16_t,Teleport> ResourceManager::LoadTeleportInfo ()
{
    return m_Teleports;
}

std::multimap<uint16_t,TeleportLink> ResourceManager::LoadTeleportLinks ()
{
    return m_TeleportLinks;
}

boost::unordered_map<uint16_t,TeleportOptional> ResourceManager::LoadTeleportOptional ()
{
    return m_TeleportOptionals;
}

boost::unordered_map<uint8_t,LevelProperty> ResourceManager::LoadXPChart ()
{
    return m_LevelProperties;
}

boost::unordered_map<uint8_t,std::pair<uint16_t,uint16_t> > ResourceManager::LoadGoldChart ()
{
    return m_GoldChart;
}

std::vector<SpawnPoint> ResourceManager::LoadSpawnPoints ()
{
    return m_SpawnPoints;
}

std::multimap<uint32_t,NPCTask> ResourceManager::LoadNPCTasks ()
{
    DB::PROPERTY::NPC query;
    return query(m_db_conn);
}

boost::unordered_map<uint32_t,uint16_t> ResourceManager::LoadShopItemStacks ()
{
    DB::SHOP::Stacks query;
    return query(m_db_conn);
}

std::vector<MagicOptionProperty> ResourceManager::LoadMagicOptionProperties ()
{
    DB::PROPERTY::MagicOption query;
    return query(m_db_conn);
}

boost::unordered_multimap<uint32_t,Drop> ResourceManager::LoadDropChart ()
{
    DB::DROP::Info query;
    return query(m_db_conn);
}

boost::unordered_multimap<uint32_t,DropWeight> ResourceManager::LoadDropWeightChart ()
{
    DB::DROP::Weight query;
    return query(m_db_conn);
}

void ResourceManager::OnCharacterdata (const std::vector<std::string> &entries)
{
    EntityProperty *property = new EntityProperty;

    if (property)
    {
        if (ParserCharacterdata(entries,property))
        {
            boost::shared_ptr<EntityProperty> pent(property);

            m_CharProperties.insert(std::make_pair(pent->refID,pent));
        }
        else
            delete property;
    }
}

void ResourceManager::OnItemdata (const ItemProperty &property)
{
    boost::shared_ptr<ItemProperty> pitm(new ItemProperty(property));

    if (pitm)
        m_ItemProperties.insert(std::make_pair(property.refID,pitm));
}

void ResourceManager::OnSkilldata (const SkillProperty &property)
{
    m_SkillProperties.push_back(property);
}

void ResourceManager::OnLeveldata (const LevelProperty &property)
{
    m_LevelProperties.insert(std::make_pair(property.Level,property));
}

void ResourceManager::OnLevelgold (const std::pair<uint8_t,uint16_t> &value)
{
    m_GoldChart.insert(std::make_pair(value.first,value));
}

void ResourceManager::OnSpawnpoint (const std::vector<std::string> &entries)
{
    SpawnPoint point;

    if(ParserNpcpos(entries,&point))
        m_SpawnPoints.push_back(point);
}

void ResourceManager::OnMapShopGroup (const std::pair<std::string,std::string> &mapping)
{
}

void ResourceManager::OnMapShopTab (const std::pair<std::string,std::string> &mapping)
{
}

void ResourceManager::OnPackageItem (const PackageItem_t &item)
{
}

void ResourceManager::OnPriceItem (const ItemPricePolicy_t &policy)
{
}

void ResourceManager::OnScrapItem (const ScrapPackageItem_t &item)
{
}

void ResourceManager::OnShopGood (const ShopGood_t &good)
{
}

void ResourceManager::OnShopGroup (const ShopGroup_t &group)
{
}

void ResourceManager::OnShopTab (const ShopTab_t &tab)
{
}

void ResourceManager::OnTeleportdata (const std::vector<std::string> &entries)
{
    Teleport teleport;

    if (ParserTeleportdata(entries,&teleport))
        m_Teleports.insert(std::make_pair(teleport.ID,teleport));
}

void ResourceManager::OnTeleportLink (const TeleportLink &link)
{
    m_TeleportLinks.insert(std::make_pair(link.src,link));
}

void ResourceManager::OnTeleportOptional (const TeleportOptional &teleport)
{
    m_TeleportOptionals.insert(std::make_pair(teleport.id,teleport));
}

void ResourceManager::OnTeleportBuilding (const BuildingProperty &property)
{
    boost::shared_ptr<TeleportBuilding> blg(new TeleportBuilding);

    if (blg)
    {
        blg->TeleportID = 0;
        blg->m_Property = property;

        m_Buildings.push_back(blg);
    }
}

void ResourceManager::OnFortressProperty (const FortressProperty &property)
{
    m_FortressProperties.push_back(property);
}
