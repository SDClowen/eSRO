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

#ifndef __SRLF_ITEM_PROPERTY_HPP__
#define __SRLF_ITEM_PROPERTY_HPP__

#include "item_types.hpp"

#include <stdint.h>
#include <string>
#include <utility>
#include <boost/array.hpp>

#define ITEM_TYPE_FLAG_SIZE 6
#define MAX_ITEM_REQUIREMENTS 4
#define MAX_ITEM_EFFECTS 20

struct ItemProperty
{
    ITEM_TYPE GetType() const;

    uint32_t refID;
    std::string refName;
    std::string refBaseName;
    std::string refResName;
    std::string refDescName;
    boost::array<int8_t,ITEM_TYPE_FLAG_SIZE> Types;
    uint32_t SpawnTimeout;
    uint8_t Race;
    uint8_t Sox;
    uint16_t StoreFeed;
    uint32_t SellPrice;
    boost::array<std::pair<int16_t,uint8_t>,MAX_ITEM_REQUIREMENTS> Requirements;   //<Mastery ID,Level>
    int16_t QSP;
    std::string Model;
    std::string DropModel;
    std::string Icon;
    std::string GP;
    uint16_t MaxStack;
    uint8_t Gender;
    uint8_t Degree;
    uint8_t SetID;
    std::pair<uint32_t,uint32_t> durability;  //Minimum and Maximum durability.
    std::pair<double,double> pd;			//Mininum and Maximum Physical Defense.
    double pd_lv;						//PD increase per enchanting level.
    std::pair<double,double> pa;			//Minimun and Maximum Physical Absorb.
    double pa_lv;						//PA increase per enchanting level.
    std::pair<uint8_t,uint8_t> br;			//Minimum and Maximum Blocking ratio.
    std::pair<double,double> md;			//Minimun and Maximum Magical Defense
    double md_lv;						//MD increase per level.
    std::pair<double,double> ma;			//Minimun and Maximum Magical Absorb.
    double ma_lv;						//MA increase per enchanting level.
    uint16_t pdr;							//Physical Defense reinforce.
    uint16_t pr;							//Parry ratio.
    uint16_t mdr;							//Magical Defense reinforce.
    uint16_t attk_dist;						//Attack distance.
    std::pair<uint16_t,uint16_t> min_patk;		//Minimum Physical Attack range.
    std::pair<uint16_t,uint16_t> max_patk;	    //Maximum Physical Attack range.
    double patk_lv;						//Physical Attack increase per level.
    std::pair<uint16_t,uint16_t> min_matk;	    //Minimum Magical Attack range.
    std::pair<uint16_t,uint16_t> max_matk;		//Maximum Magical Attack range.
    double matk_lv;						//Magical Attack increase per level.
    std::pair<uint16_t,uint16_t> min_phyref;	//Minimum Physical Reinforce Range.
    std::pair<uint16_t,uint16_t> max_phyref;	//Maximum Physical Reinforce Range.
    std::pair<uint16_t,uint16_t> min_magref;	//Minimum Magical Reinforce Range.
    std::pair<uint16_t,uint16_t> max_magref;	//Maximum Magical Reinforce Range.
    std::pair<uint16_t,uint16_t> ar;			//Maximum and Minimum Attack Rating.
    std::pair<uint8_t,uint8_t> crit;			//Maximum and Minimum Critical.
    std::pair<int32_t,std::string> Effects[MAX_ITEM_EFFECTS];
    uint8_t MaxOpts;
    bool AvatarFlag;
};

ITEM_TYPE ItemType (uint8_t t4, uint8_t t5, uint8_t t6);

#endif
