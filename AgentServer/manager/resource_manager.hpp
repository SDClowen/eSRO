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

#ifndef __SR_GAME_SERVER_RESOURCE_MANAGER_HPP__
#define __SR_GAME_SERVER_RESOURCE_MANAGER_HPP__

/// SRLF INCLUDES
#include <drop.hpp>
#include <npc_task.hpp>
#include <item_property.hpp>
#include <skill_property.hpp>
#include <magicoption_property.hpp>
#include <level_property.hpp>
#include <fortress_property.hpp>
#include <building_property.hpp>
#include <spawn_point.hpp>
#include <teleport.hpp>
#include <packageitem.hpp>
#include <itemprice.hpp>
#include <scrapofpackageitem.hpp>
#include <shopgood.hpp>
#include <shopgroup.hpp>
#include <shoptab.hpp>

#include <mysql++/mysql++.h>

/// GLOBAL INCLUDES
#include <boost/unordered_map.hpp>
#include <boost/tuple/tuple.hpp>

class EntityProperty;
class TeleportBuilding;

class ResourceManager
{
public:

    ResourceManager (const std::string &path, const std::string &usr, const std::string &pw,
                     const std::string &host, const std::string &db);

    bool Initialize ();

    void Release ();

    boost::unordered_map<uint32_t,boost::shared_ptr<ItemProperty> > LoadItemProperties ();

    boost::unordered_map<uint32_t,boost::shared_ptr<EntityProperty> > LoadCharacterProperties ();

    std::multimap<uint32_t,std::pair<uint8_t,uint8_t> > LoadCharacterQualityChart ();

    std::vector<SkillProperty> LoadSkillProperties ();

    std::vector<FortressProperty> LoadFortress ();

    std::vector<boost::shared_ptr<TeleportBuilding> > LoadTeleportBuildings ();

    boost::unordered_map<uint16_t,Teleport> LoadTeleportInfo ();

    std::multimap<uint16_t,TeleportLink> LoadTeleportLinks ();

    boost::unordered_map<uint16_t,TeleportOptional> LoadTeleportOptional ();

    boost::unordered_map<uint8_t,LevelProperty> LoadXPChart ();

    boost::unordered_map<uint8_t,std::pair<uint16_t,uint16_t> > LoadGoldChart ();

    std::vector<SpawnPoint> LoadSpawnPoints ();

    std::multimap<uint32_t,NPCTask> LoadNPCTasks ();

    boost::unordered_map<uint32_t,uint16_t> LoadShopItemStacks ();

    std::vector<MagicOptionProperty> LoadMagicOptionProperties ();

    boost::unordered_multimap<uint32_t,Drop> LoadDropChart ();

    boost::unordered_multimap<uint32_t,DropWeight> LoadDropWeightChart ();

protected:

    void OnCharacterdata (const std::vector<std::string> &entries);

    void OnItemdata (const ItemProperty &property);

    void OnSkilldata (const SkillProperty &property);

    void OnLeveldata (const LevelProperty &property);

    void OnLevelgold (const std::pair<uint8_t,uint16_t> &value);

    void OnSpawnpoint (const std::vector<std::string> &entries);

    void OnMapShopGroup (const std::pair<std::string,std::string> &mapping);

    void OnMapShopTab (const std::pair<std::string,std::string> &mapping);

    void OnPackageItem (const PackageItem_t &item);

    void OnPriceItem (const ItemPricePolicy_t &policy);

    void OnScrapItem (const ScrapPackageItem_t &item);

    void OnShopGood (const ShopGood_t &good);

    void OnShopGroup (const ShopGroup_t &group);

    void OnShopTab (const ShopTab_t &tab);

    void OnTeleportdata (const std::vector<std::string> &entries);

    void OnTeleportLink (const TeleportLink & link);

    void OnTeleportOptional (const TeleportOptional &teleport);

    void OnTeleportBuilding (const BuildingProperty &building);

    void OnFortressProperty (const FortressProperty &property);

private:

    std::string m_path;
    std::string m_usr;
    std::string m_pw;
    std::string m_host;
    std::string m_db;
    mysqlpp::Connection m_db_conn;

    boost::unordered_map<uint32_t,boost::shared_ptr<EntityProperty> > m_CharProperties;
    boost::unordered_map<uint32_t,boost::shared_ptr<ItemProperty> > m_ItemProperties;
    std::vector<SkillProperty> m_SkillProperties;
    boost::unordered_map<uint8_t,LevelProperty> m_LevelProperties;
    std::vector<SpawnPoint> m_SpawnPoints;
    boost::unordered_map<uint8_t,std::pair<uint16_t,uint16_t> > m_GoldChart;
    boost::unordered_map<uint16_t,Teleport> m_Teleports;
    std::vector<boost::shared_ptr<TeleportBuilding> > m_Buildings;
    std::multimap<uint16_t,TeleportLink> m_TeleportLinks;
    boost::unordered_map<uint16_t,TeleportOptional> m_TeleportOptionals;
    std::vector<FortressProperty> m_FortressProperties;

};

#endif
