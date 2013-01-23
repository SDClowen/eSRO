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

#include <iostream>
#include <fstream>

#include <entity_property.hpp>
#include <property_query.hpp>

#include <mysql++/mysql++.h>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/unordered_map.hpp>

using namespace boost::lambda;

#define ITEM_SET_WEIGHT 0.10
#define STONE_SET_WEIGHT 0.05
#define TABLET_SET_WEIGHT 0.10
#define MATERIAL_SET_WEIGHT 0.10
#define ELIXIR_SET_WEIGHT 0.05
#define BOLT_SET_WEIGHT 0.10
#define ARROW_SET_WEIGHT 0.10
#define POT_SET_WEIGHT 0.40
#define PILL_SET_WEIGHT 0.10
#define RETURN_SET_WEIGHT 0.05

enum DROP_QUALITY
{
    DROP_QUALITY_ITEM = 1,
    DROP_QUALITY_ETC = 3
};

float ItemRequiredLevel (int degree, int tier)
{
    return 1.19*pow(3*(degree-1)+tier,1.29);
}

void SaveDrop (mysqlpp::Connection &conn, uint32_t refCharID, std::pair<uint32_t,float> Item, int Quality)
{
    mysqlpp::Query query = conn.query();

    query << "insert into refcharacter_drop values (" << refCharID << "," << Item.first << "," << Item.second
        << "," << Quality << ")";

    query.exec();
}

void SaveMob (mysqlpp::Connection &conn, uint32_t refCharID, float Weight, int Quality)
{
    mysqlpp::Query query = conn.query();

    query << "insert into refcharacter_drop_quality values (" << refCharID << "," << Weight << "," << Quality << ")";

    query.exec();
}

int main(int argc, char *argv[])
{
    uint32_t itemID;
    std::string name;
    boost::unordered_map<uint32_t,std::string> materials;

    std::fstream file("material_list.txt");

    if (file)
    {
        while(!file.eof())
        {
            itemID = 0;
            name.clear();

            file >> itemID >> name;

            if (itemID)
            {
                name.erase(name.end()-1);
                materials.insert(std::make_pair(itemID,name));
            }
        }
    }

    file.close();

    mysqlpp::Connection conn("DB_Shard","127.0.0.1","root","npcdoom");

    if (!conn.connected())
    {
        std::cout << "Failed to connect to database server." << std::endl;
        return 0;
    }

    mysqlpp::Query cquery = conn.query();

    cquery << "delete from refcharacter_drop";

    cquery.exec();

    cquery.reset();

    cquery << "delete from refcharacter_drop_quality";

    cquery.exec();

    DB::PROPERTY::Character ch_query;
    boost::unordered_map<uint32_t,boost::shared_ptr<CharacterProperty> > entities = ch_query(conn);

    std::cout << "# Entities = " << entities.size() << std::endl;

    DB::PROPERTY::Item query;
    boost::unordered_map<uint32_t,ItemProperty> items = query(conn);

    std::cout << "# Item = " << items.size() << std::endl;

    typedef std::vector<std::pair<uint32_t,float> > ItemVector;
    typedef boost::unordered_map<uint32_t,ItemProperty>::const_iterator item_const_iterator;
    typedef boost::unordered_map<uint32_t,boost::shared_ptr<CharacterProperty> >::const_iterator char_const_iterator;

    float pitem = 0;
    float pmat = 0;
    float pstones = 0;
    float ptablets = 0;
    float pelixirs = 0;
    float pbolts = 0;
    float parrows = 0;
    float ppots = 0;
    float ppills = 0;
    float preturns = 0;
    ItemVector vitem,mats,stones,tablets,elixirs,bolts,arrows,pots,pills,returns;

    for (char_const_iterator iter = entities.begin(); iter != entities.end(); ++iter)
    {
        switch(iter->second->type())
        {
        case ET_MOB:
        case ET_MOB_HUNTER:
        case ET_MOB_THIEF:
        case ET_MOB_EVENT:
            break;
        default:
            continue;
        }

        if (iter->second->level != 1)
            continue;

        for (item_const_iterator iiter = items.begin(); iiter != items.end(); ++iiter)
        {
            /// SILK ITEM CANT BE DROPABLE OR BASIC ITEMS
            if (iiter->second.type_flag[0] || !iiter->second.sell_price)
                continue;

            switch(iiter->second.GetType())
            {
            case ITEM_HP_POT:
            case ITEM_MP_POT:
            case ITEM_ALL_POT:
            case ITEM_CURE_ALL:
                {
                    int ratio = iter->second->level/20;

                    if (ratio+1 == iiter->second.degree)
                    {
                        if (iiter->second.GetType() == ITEM_CURE_ALL)
                            pills.push_back(std::make_pair(iiter->first,1));
                        else
                            pots.push_back(std::make_pair(iiter->first,1));
                    }
                }
                break;
            case ITEM_ARROW:
                if (iter->second->level <= 40) ;
                arrows.push_back(std::make_pair(iiter->first,1));
                break;
            case ITEM_BOLT:
                if (iter->second->level <= 40) ;
                bolts.push_back(std::make_pair(iiter->first,1));
                break;
            case ITEM_RETURN_SCROLL:
                returns.push_back(std::make_pair(iiter->first,1));
                break;
            case ITEM_CH_SWORD:
            case ITEM_CH_BLADE:
            case ITEM_CH_SPEAR:
            case ITEM_CH_TBLADE:
            case ITEM_CH_BOW:
            case ITEM_EU_SWORD:
            case ITEM_EU_TSWORD:
            case ITEM_EU_AXE:
            case ITEM_EU_DARKSTAFF:
            case ITEM_EU_TSTAFF:
            case ITEM_EU_CROSSBOW:
            case ITEM_EU_DAGGER:
            case ITEM_EU_HARP:
            case ITEM_EU_STAFF:
            case ITEM_CH_SHIELD:
            case ITEM_EU_SHIELD:
            case ITEM_CH_HEAVY_HA:
            case ITEM_CH_HEAVY_SA:
            case ITEM_CH_HEAVY_BA:
            case ITEM_CH_HEAVY_LA:
            case ITEM_CH_HEAVY_AA:
            case ITEM_CH_HEAVY_FA:
            case ITEM_CH_LIGHT_HA:
            case ITEM_CH_LIGHT_SA:
            case ITEM_CH_LIGHT_BA:
            case ITEM_CH_LIGHT_LA:
            case ITEM_CH_LIGHT_AA:
            case ITEM_CH_LIGHT_FA:
            case ITEM_CH_CLOTHES_HA:
            case ITEM_CH_CLOTHES_SA:
            case ITEM_CH_CLOTHES_BA:
            case ITEM_CH_CLOTHES_LA:
            case ITEM_CH_CLOTHES_AA:
            case ITEM_CH_CLOTHES_FA:
            case ITEM_EU_HEAVY_HA:
            case ITEM_EU_HEAVY_SA:
            case ITEM_EU_HEAVY_BA:
            case ITEM_EU_HEAVY_LA:
            case ITEM_EU_HEAVY_AA:
            case ITEM_EU_HEAVY_FA:
            case ITEM_EU_LIGHT_HA:
            case ITEM_EU_LIGHT_SA:
            case ITEM_EU_LIGHT_BA:
            case ITEM_EU_LIGHT_LA:
            case ITEM_EU_LIGHT_AA:
            case ITEM_EU_LIGHT_FA:
            case ITEM_EU_CLOTHES_HA:
            case ITEM_EU_CLOTHES_SA:
            case ITEM_EU_CLOTHES_BA:
            case ITEM_EU_CLOTHES_LA:
            case ITEM_EU_CLOTHES_AA:
            case ITEM_EU_CLOTHES_FA:
            case ITEM_CH_RING:
            case ITEM_CH_EARRING:
            case ITEM_CH_NECKLACE:
            case ITEM_EU_RING:
            case ITEM_EU_EARRING:
            case ITEM_EU_NECKLACE:
                {
                    int diff = iiter->second.req_mast[0].second - iter->second->level;

                    if (iiter->second.req_mast[0].second <= iter->second->level && diff >= -3)
                        vitem.push_back(std::make_pair(iiter->first,5));
                }
                break;
            case ITEM_ALCHEMY_MATERIAL:
                if (materials[iiter->first] == iter->second->name)
                    mats.push_back(std::make_pair(iiter->first,1));
                break;
            case ITEM_ALCHEMY_TABLET:
            case ITEM_ALCHEMY_STATUS_STONE:
            case ITEM_ALCHEMY_MAGIC_STONE:
            case ITEM_ALCHEMY_ATTR_STONE:
                {
                    int min_lv = ItemRequiredLevel(iiter->second.degree,1);
                    int max_lv = ItemRequiredLevel(iiter->second.degree,3);

                    if (min_lv <= iter->second->level && iter->second->level <= max_lv)
                    {
                        if (iiter->second.GetType() == ITEM_ALCHEMY_TABLET)
                            tablets.push_back(std::make_pair(iiter->first,1));
                        else
                            stones.push_back(std::make_pair(iiter->first,1));
                    }
                }
                break;
            case ITEM_ALCHEMY_ELIXIR:
                elixirs.push_back(std::make_pair(iiter->first,1));
                break;
            default:
                break;
            }
        }

        /// CALCULATE TOTAL WEIGHT FOR EACH SET
        std::for_each(vitem.begin(),vitem.end(), pitem += bind(&std::pair<uint32_t,float>::second,_1));
        std::for_each(mats.begin(),mats.end(), pmat += bind(&std::pair<uint32_t,float>::second,_1));
        std::for_each(stones.begin(),stones.end(), pstones += bind(&std::pair<uint32_t,float>::second,_1));
        std::for_each(tablets.begin(),tablets.end(), ptablets += bind(&std::pair<uint32_t,float>::second,_1));
        std::for_each(elixirs.begin(),elixirs.end(), pelixirs += bind(&std::pair<uint32_t,float>::second,_1));

        std::for_each(bolts.begin(),bolts.end(), pbolts += bind(&std::pair<uint32_t,float>::second,_1));
        std::for_each(arrows.begin(),arrows.end(), parrows += bind(&std::pair<uint32_t,float>::second,_1));
        std::for_each(pots.begin(),pots.end(), ppots += bind(&std::pair<uint32_t,float>::second,_1));
        std::for_each(pills.begin(),pills.end(), ppills += bind(&std::pair<uint32_t,float>::second,_1));
        std::for_each(returns.begin(),returns.end(), preturns += bind(&std::pair<uint32_t,float>::second,_1));

        for (ItemVector::iterator piter = vitem.begin(); piter != vitem.end(); ++piter)
        {
            piter->second *= (ITEM_SET_WEIGHT);
            SaveDrop(conn,iter->first,*piter,DROP_QUALITY_ITEM);
        }

        for (ItemVector::iterator piter = mats.begin(); piter != mats.end(); ++piter)
        {
            piter->second *= (MATERIAL_SET_WEIGHT);
            SaveDrop(conn,iter->first,*piter,DROP_QUALITY_ITEM);
        }

        for (ItemVector::iterator piter = stones.begin(); piter != stones.end(); ++piter)
        {
            piter->second *= (STONE_SET_WEIGHT);
            SaveDrop(conn,iter->first,*piter,DROP_QUALITY_ITEM);
        }

        for (ItemVector::iterator piter = tablets.begin(); piter != tablets.end(); ++piter)
        {
            piter->second *= (TABLET_SET_WEIGHT);
            SaveDrop(conn,iter->first,*piter,DROP_QUALITY_ITEM);
        }

        for (ItemVector::iterator piter = elixirs.begin(); piter != elixirs.end(); ++piter)
        {
            piter->second *= (ELIXIR_SET_WEIGHT);
            SaveDrop(conn,iter->first,*piter,DROP_QUALITY_ITEM);
        }

        for (ItemVector::iterator piter = bolts.begin(); piter != bolts.end(); ++piter)
        {
            piter->second *= (BOLT_SET_WEIGHT);
            SaveDrop(conn,iter->first,*piter,DROP_QUALITY_ETC);
        }

        for (ItemVector::iterator piter = arrows.begin(); piter != arrows.end(); ++piter)
        {
            piter->second *= (ARROW_SET_WEIGHT);
            SaveDrop(conn,iter->first,*piter,DROP_QUALITY_ETC);
        }

        for (ItemVector::iterator piter = pills.begin(); piter != pills.end(); ++piter)
        {
            piter->second *= (PILL_SET_WEIGHT);
            SaveDrop(conn,iter->first,*piter,DROP_QUALITY_ETC);
        }

        for (ItemVector::iterator piter = pots.begin(); piter != pots.end(); ++piter)
        {
            piter->second *= (POT_SET_WEIGHT);
            SaveDrop(conn,iter->first,*piter,DROP_QUALITY_ETC);
        }

        for (ItemVector::iterator piter = returns.begin(); piter != returns.end(); ++piter)
        {
            piter->second *= (RETURN_SET_WEIGHT);
            SaveDrop(conn,iter->first,*piter,DROP_QUALITY_ETC);
        }

        SaveMob(conn,iter->first,pitem+pmat+pstones+ptablets+pelixirs,DROP_QUALITY_ITEM);
        SaveMob(conn,iter->first,pbolts+parrows+ppots+ppills+preturns,DROP_QUALITY_ETC);

        /// CLEAR ALL DATA
        pitem = 0;
        pmat = 0;
        pstones = 0;
        ptablets = 0;
        pelixirs = 0;
        pbolts = 0;
        parrows = 0;
        ppots = 0;
        ppills = 0;
        preturns = 0;

        vitem.clear();
        tablets.clear();
        stones.clear();
        elixirs.clear();
        mats.clear();
        pots.clear();
        pills.clear();
        arrows.clear();
        bolts.clear();
        returns.clear();
    }

    return 0;
}
