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

#include "item.hpp"
#include "item_property.hpp"
#include "utility_function.hpp"

Item::extended_type::extended_type ()
{
    dbid = 0;
	id = 0;
	enchant_lv = 0;
	pos = 0;
	quantity = 0;
	durability = 0;
	phy_percent = 0;
	mag_percent = 0;
	dur_percent = 0;
	ap_percent = 0;
	cb_percent = 0;
	phyreinf_percent = 0;
	magreinf_percent = 0;
	place = PLACE_INVENTORY;
}

Item::Item ()
{
	m_Assimilation = 0;
	m_can_crit = false;
}

Item::Item (const boost::shared_ptr<ItemProperty> &property)
    : m_base_property(property)
{
	m_ext_property.quantity = 1;
    m_ext_property.id = m_base_property->refID;
    m_ext_property.durability = m_base_property->durability.first;

	m_Assimilation = 0;

	calculate_property();
}

Item::Item (const boost::shared_ptr<ItemProperty> &property, const Item::extended_type& extended)
    : m_base_property(property), m_ext_property(extended)
{
	m_Assimilation = 0;

	calculate_property();
}

void Item::ID (const uint32_t newID)
{
    m_ext_property.dbid = newID;
}

uint32_t Item::ID () const
{
    return m_ext_property.dbid;
}

boost::shared_ptr<ItemProperty> Item::property () const
{
    return m_base_property;
}

void Item::set_enchant_lv (uint8_t lv)
{
	m_ext_property.enchant_lv = lv;
}

void Item::set_position (uint8_t pos)
{
	m_ext_property.pos = pos;
}

void Item::set_quantity (uint32_t quantity)
{
	m_ext_property.quantity = quantity;
}

void Item::set_durability (uint32_t durability)
{
	m_ext_property.durability = durability;
}

void Item::set_place (int place)
{
	m_ext_property.place = place;
}

void Item::set_physical_percent (int percent)
{
	m_ext_property.phy_percent = percent;
}

void Item::set_magical_percent (int percent)
{
	m_ext_property.mag_percent = percent;
}

void Item::set_durability_percent (int percent)
{
	m_ext_property.dur_percent = percent;
}

void Item::set_ap_ratio_percent (int percent)
{
	m_ext_property.ap_percent = percent;
}

void Item::set_cb_ratio_percent (int percent)
{
	m_ext_property.cb_percent = percent;
}

void Item::set_physical_reinf_percent (int percent)
{
	m_ext_property.phyreinf_percent = percent;
}

void Item::set_magical_reinf_percent (int percent)
{
	m_ext_property.magreinf_percent = percent;
}

uint32_t Item::get_id () const
{
	return m_ext_property.id;
}

ITEM_TYPE Item::get_type () const
{
	return m_Type;
}

uint8_t Item::get_enchant_lv () const
{
	return m_ext_property.enchant_lv;
}

uint8_t Item::get_position () const
{
	return m_ext_property.pos;
}

uint32_t Item::get_quantity () const
{
	return m_ext_property.quantity;
}

uint32_t Item::get_durability () const
{
	return m_ext_property.durability;
}

uint32_t Item::get_max_durability () const
{
    return calculate(m_ext_property.dur_percent,m_base_property->durability);
}

std::vector<Item::attribute> Item::get_attr_list () const
{
	return m_ext_property.attr_list;
}

uint32_t Item::get_sell_price () const
{
    return m_base_property->SellPrice;
}

uint8_t Item::get_max_stack () const
{
    return m_base_property->MaxStack;
}

float Item::attack_distance () const
{
    return m_base_property->attk_dist;
}

RACE Item::get_req_race () const
{
    return static_cast<RACE>(m_base_property->Race);
}

uint32_t Item::get_storage_feed () const
{
    return m_base_property->StoreFeed;
}

uint16_t Item::physical_defense () const
{
    return (uint16_t)calculate(m_ext_property.phy_percent,m_base_property->pd);
}

uint16_t Item::magical_defense () const
{
    return (uint16_t)calculate(m_ext_property.mag_percent,m_base_property->md);
}

uint16_t Item::hit_ratio () const
{
    return calculate(m_ext_property.ap_percent,m_base_property->ar);
}

uint16_t Item::parry_ratio () const
{
    return m_base_property->pr;
}

float Item::physical_absorb () const
{
    return calculate(m_ext_property.phy_percent,m_base_property->pa);
}

float Item::magical_absorb () const
{
    return calculate(m_ext_property.mag_percent,m_base_property->ma);
}

std::pair<uint16_t,uint16_t> Item::physical_attack () const
{
	std::pair<uint16_t,uint16_t> atk;

    atk.first = calculate(m_ext_property.phy_percent,m_base_property->min_patk);
    atk.second = calculate(m_ext_property.phy_percent,m_base_property->max_patk);

	return atk;
}

std::pair<uint16_t,uint16_t> Item::magical_attack () const
{
	std::pair<uint16_t,uint16_t> atk;

    atk.first = calculate(m_ext_property.mag_percent,m_base_property->min_matk);
    atk.second = calculate(m_ext_property.mag_percent,m_base_property->max_matk);

	return atk;
}

std::pair<uint16_t,uint16_t> Item::physical_reinforce () const
{
	std::pair<uint16_t,uint16_t> reinforce;

	reinforce.first = calculate(m_ext_property.phyreinf_percent,
                                m_base_property->min_phyref);

	reinforce.second = calculate(m_ext_property.phyreinf_percent,
                                 m_base_property->max_phyref);

	return reinforce;
}

std::pair<uint16_t,uint16_t> Item::magical_reinforce () const
{
	std::pair<uint16_t,uint16_t> reinforce;

	reinforce.first = calculate(m_ext_property.magreinf_percent,
                                m_base_property->min_magref);

	reinforce.second = calculate(m_ext_property.magreinf_percent,
                                 m_base_property->max_magref);

	return reinforce;
}

uint8_t Item::critical_ratio () const
{
    return calculate(m_ext_property.cb_percent,m_base_property->crit);
}

uint8_t Item::block_ratio () const
{
    return calculate(m_ext_property.cb_percent,m_base_property->br);
}

void Item::insert_magic_option (const Item::attribute &attr)
{
	m_ext_property.attr_list.push_back(attr);
}

bool Item::is_opts_full () const
{
    return m_ext_property.attr_list.size() == m_base_property->MaxOpts;
}

int Item::get_place () const
{
	return m_ext_property.place;
}

uint64_t Item::encoded_mods () const
{
	uint64_t mods = 0;

	ITEM_TYPE itype = get_type();

	uint8_t dura_bonus = m_ext_property.dur_percent;
	uint8_t phy_re_bonus = m_ext_property.phyreinf_percent;
	uint8_t mag_re_bonus = m_ext_property.magreinf_percent;
	uint8_t ap_bonus = m_ext_property.ap_percent;
	uint8_t phy_bonus = m_ext_property.phy_percent;
	uint8_t mag_bonus = m_ext_property.mag_percent;
	uint8_t cb_bonus = m_ext_property.cb_percent;

	if ( itype >= ITEM_CH_SWORD && itype <= ITEM_EU_DARKSTAFF)
	{
		mods = dura_bonus | (phy_re_bonus<<5) | (mag_re_bonus<<10) |
			(ap_bonus<<15) | (phy_bonus<<20) | (mag_bonus<<25) | (cb_bonus<<30);
	}
	else if (itype >= ITEM_CH_SHIELD && itype <= ITEM_EU_SHIELD)
	{
		mods = dura_bonus | (phy_re_bonus<<5) | (mag_re_bonus<<10) |
			(cb_bonus<<15) | (phy_bonus<<20) | (mag_bonus<<25);
	}
	else if (itype >= ITEM_CH_HEAVY_HA && itype <= ITEM_EU_CLOTHES_FA)
	{
		//EXAMPLE!!  0x02068CE0 E0 8C 06 02
		mods = dura_bonus | (phy_re_bonus<<5) | (mag_re_bonus<<10) |
			(phy_bonus<<15) | (mag_bonus<<20) | (ap_bonus<<25);
	}
	else if (itype >= ITEM_CH_RING && itype <= ITEM_EU_NECKLACE)
	{
		mods = phy_bonus | (mag_bonus<<5);
	}

	return mods;
}

int Item::get_physical_percent () const
{
	return m_ext_property.phy_percent;
}

int Item::get_magical_percent () const
{
	return m_ext_property.mag_percent;
}

int Item::get_durability_percent () const
{
	return m_ext_property.dur_percent;
}

int Item::get_ap_ratio_percent () const
{
	return m_ext_property.ap_percent;
}

int Item::get_cb_ratio_percent () const
{
	return m_ext_property.cb_percent;
}

int Item::get_physical_reinf_percent () const
{
	return m_ext_property.phyreinf_percent;
}

int Item::get_magical_reinf_percent () const
{
	return m_ext_property.magreinf_percent;
}

GENDER Item::get_gender () const
{
    return static_cast<GENDER>(m_base_property->Gender);
}

uint8_t Item::degree () const
{
    return m_base_property->Degree;
}

uint8_t Item::sox_type () const
{
    return m_base_property->Sox;
}

bool Item::check_level (const uint8_t level) const
{
    for (int i = 0; i < MAX_ITEM_REQUIREMENTS; ++i)
    {
        if ((m_base_property->Requirements[i].first == 1 &&
            m_base_property->Requirements[i].second <= level) || m_base_property->Requirements[i].first == -1)
            return true;
    }

    return false;
}

bool Item::check_mastery (const Mastery& mastery) const
{
    for (int i = 0; i < MAX_ITEM_REQUIREMENTS; ++i)
	{
        if (m_base_property->Requirements[i].first == (int32_t)mastery.ID() &&
            m_base_property->Requirements[i].second <= mastery.Level())
            return true;
	}

    return false;
}

bool Item::check_combination (const Item& item) const
{
	enum
	{
		CH_CLOTH = 1,
		CH_LIGHT,
		CH_HEAVY,
		EU_CLOTH = 9,
		EU_LIGHT,
		EU_HEAVY
	};

	bool valid = true;

    switch(m_base_property->Types[4])
	{
	case CH_HEAVY:
	case CH_LIGHT:
        if (item.m_base_property->Types[4] != CH_HEAVY && item.m_base_property->Types[4] != CH_LIGHT)
            valid = false;
		break;
	case CH_CLOTH:
        if (item.m_base_property->Types[4] != CH_CLOTH)
            valid = false;
		break;
	case EU_HEAVY:
	case EU_LIGHT:
		break;
	case EU_CLOTH:
		break;
	default:
		break;
	}

	return valid;
}

void Item::set_assimilation (const uint8_t assimilation)
{
	m_Assimilation = assimilation;
}

uint8_t Item::get_assimilation () const
{
	return m_Assimilation;
}

void Item::set_pet_name (const std::string &name)
{
	m_PetName = name;
}

const std::string& Item::get_pet_name() const
{
	return m_PetName;
}

bool Item::is_dropable () const
{
    if (m_base_property->Types[0])
        return false;

    return true;
}

bool Item::is_tradable() const
{
    if (m_base_property->Types[0])
        return false;

    return true;
}

bool Item::can_crit () const
{
	return m_can_crit;
}

bool Item::IsWeapon () const
{
    bool weapon = false;

    switch(get_type())
    {
    case ITEM_CH_SWORD:
	case ITEM_CH_BLADE:
	case ITEM_CH_BOW:
	case ITEM_CH_SPEAR:
	case ITEM_CH_TBLADE:
	case ITEM_EU_SWORD:
	case ITEM_EU_TSWORD:
	case ITEM_EU_AXE:
	case ITEM_EU_STAFF:
	case ITEM_EU_TSTAFF:
	case ITEM_EU_DARKSTAFF:
	case ITEM_EU_DAGGER:
	case ITEM_EU_CROSSBOW:
	case ITEM_EU_HARP:
        weapon = true;
        break;
    default:
        break;
    }

    return weapon;
}

bool Item::IsShield () const
{
    bool shield = false;

    switch(get_type())
    {
    case ITEM_CH_SHIELD:
    case ITEM_EU_SHIELD:
        shield = true;
        break;
    default:
        break;
    }

    return shield;
}

bool Item::IsArmor () const
{
    bool armor = false;

    switch(get_type())
    {
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
        armor = true;
        break;
    default:
        break;
    }

    return armor;
}

bool Item::IsAccessory () const
{
    bool acc = false;

    switch(get_type())
    {
    case ITEM_CH_RING:
    case ITEM_CH_EARRING:
    case ITEM_CH_NECKLACE:
    case ITEM_EU_RING:
    case ITEM_EU_EARRING:
    case ITEM_EU_NECKLACE:
        acc = true;
        break;
    default:
        break;
    }

    return acc;
}

bool Item::IsCape () const
{
    bool cape = false;

    switch(get_type())
    {
    case ITEM_PVP_CAPE:
    case ITEM_JOB_TRADER:
    case ITEM_JOB_THIEF:
    case ITEM_JOB_HUNTER:
        cape = true;
        break;
    default:
        break;
    }

    return cape;
}

bool Item::EnchantInc (bool WithLucky)
{
    int success_rate = 80 - 10*get_enchant_lv() + WithLucky*5;

    if (success_rate < 5)
        success_rate = 5;

    bool ret;

    if (rand() % 100 < success_rate)
    {
        ++m_ext_property.enchant_lv;

        ret = true;
    }
    else
    {
        m_ext_property.enchant_lv = 0;

        ret = false;
    }

    return ret;
}

bool Item::IsRepairable () const
{
    return get_max_durability();
}

void Item::Repair ()
{
    m_ext_property.durability = get_max_durability();
}

uint32_t Item::RepairCost () const
{
    return 5*degree()*(get_max_durability() - m_ext_property.durability);
}

void Item::calculate_property ()
{
	calculate_type();

	switch(get_type())
	{
	case ITEM_CH_SWORD:
	case ITEM_CH_BLADE:
	case ITEM_CH_BOW:
	case ITEM_CH_SPEAR:
	case ITEM_CH_TBLADE:
	case ITEM_EU_SWORD:
	case ITEM_EU_TSWORD:
	case ITEM_EU_AXE:
	case ITEM_EU_DAGGER:
	case ITEM_EU_CROSSBOW:
		m_can_crit = true;
        break;
	default:
		m_can_crit = false;
		break;
	}
}

void Item::calculate_type ()
{
    m_Type = m_base_property->GetType();
}

void set_default_position (Item& item)
{
	switch (item.get_type())
	{
	case ITEM_CH_SWORD:
	case ITEM_CH_BLADE:
	case ITEM_CH_SPEAR:
	case ITEM_CH_BOW:
	case ITEM_CH_TBLADE:
	case ITEM_EU_SWORD:
	case ITEM_EU_TSWORD:
	case ITEM_EU_AXE:
	case ITEM_EU_DAGGER:
	case ITEM_EU_CROSSBOW:
	case ITEM_EU_HARP:
	case ITEM_EU_TSTAFF:
	case ITEM_EU_STAFF:
	case ITEM_EU_DARKSTAFF:
		item.set_position(Item::WEAPON_SLOT);
		break;
	case ITEM_CH_RING:
	case ITEM_EU_RING:
		item.set_position(Item::LEFT_RING_SLOT);
		break;
	case ITEM_CH_EARRING:
	case ITEM_EU_EARRING:
		item.set_position(Item::EARRING_SLOT);
		break;
	case ITEM_CH_NECKLACE:
	case ITEM_EU_NECKLACE:
		item.set_position(Item::NECKLACE_SLOT);
		break;
	case ITEM_CH_SHIELD:
	case ITEM_EU_SHIELD:
	case ITEM_ARROW:
	case ITEM_BOLT:
		item.set_position(Item::SHIELD_SLOT);
		break;
	case ITEM_CH_HEAVY_HA:
	case ITEM_CH_LIGHT_HA:
	case ITEM_CH_CLOTHES_HA:
	case ITEM_EU_HEAVY_HA:
	case ITEM_EU_LIGHT_HA:
	case ITEM_EU_CLOTHES_HA:
		item.set_position(Item::HEAD_SLOT);
		break;
	case ITEM_CH_HEAVY_SA:
	case ITEM_CH_LIGHT_SA:
	case ITEM_CH_CLOTHES_SA:
	case ITEM_EU_HEAVY_SA:
	case ITEM_EU_LIGHT_SA:
	case ITEM_EU_CLOTHES_SA:
		item.set_position(Item::SHOULDER_SLOT);
		break;
	case ITEM_CH_HEAVY_BA:
	case ITEM_CH_LIGHT_BA:
	case ITEM_CH_CLOTHES_BA:
	case ITEM_EU_HEAVY_BA:
	case ITEM_EU_LIGHT_BA:
	case ITEM_EU_CLOTHES_BA:
		item.set_position(Item::CHEST_SLOT);
		break;
	case ITEM_CH_HEAVY_LA:
	case ITEM_CH_LIGHT_LA:
	case ITEM_CH_CLOTHES_LA:
	case ITEM_EU_HEAVY_LA:
	case ITEM_EU_LIGHT_LA:
	case ITEM_EU_CLOTHES_LA:
		item.set_position(Item::LEG_SLOT);
		break;
	case ITEM_CH_HEAVY_AA:
	case ITEM_CH_LIGHT_AA:
	case ITEM_CH_CLOTHES_AA:
	case ITEM_EU_HEAVY_AA:
	case ITEM_EU_LIGHT_AA:
	case ITEM_EU_CLOTHES_AA:
		item.set_position(Item::HAND_SLOT);
		break;
	case ITEM_CH_HEAVY_FA:
	case ITEM_CH_LIGHT_FA:
	case ITEM_CH_CLOTHES_FA:
	case ITEM_EU_HEAVY_FA:
	case ITEM_EU_LIGHT_FA:
	case ITEM_EU_CLOTHES_FA:
		item.set_position(Item::FOOT_SLOT);
		break;
    case ITEM_JOB_TRADER:
	case ITEM_JOB_THIEF:
	case ITEM_JOB_HUNTER:
    case ITEM_PVP_CAPE:
        item.set_position(Item::JOB_CAPE_SLOT);
        break;
	default:
		break;
	}
}
