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

#ifndef __SLES_ITEM_HPP__
#define __SLES_ITEM_HPP__

#include "types.hpp"
#include "mastery.hpp"
#include "item_types.hpp"

#include <vector>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

enum
{
	PLACE_INVENTORY,
	PLACE_AVATAR,
	PLACE_STORAGE,
	PLACE_GUILD,
	PLACE_PET
};

class ItemProperty;

class Item
{
public:

	typedef std::pair<uint32_t,uint32_t> attribute;

	struct extended_type
	{
		extended_type();

        uint32_t dbid;
		uint32_t id;
		uint8_t enchant_lv;
		uint8_t pos;
		int place;				//Where is the item: Inventory,Storage,Avatar
		uint32_t quantity;
		uint32_t durability;		//Actual item durability.

		int phy_percent;		//Attack and Absorb
		int mag_percent;		//Attack and Absorb
		int dur_percent;
		int ap_percent;			//Attack or Parry Ratio
		int cb_percent;			//Critical or Blocking ratio
		int phyreinf_percent;
		int magreinf_percent;

		uint8_t m_Assimilation;

		std::string m_PetName;
		uint32_t m_PetNPC;
		uint8_t m_PetStatus;

		std::vector<Item::attribute> attr_list;
	};

	//Default item position in player inventory.

	enum
	{
		HEAD_SLOT,
		CHEST_SLOT ,
		SHOULDER_SLOT,
		HAND_SLOT,
		LEG_SLOT,
		FOOT_SLOT,
		WEAPON_SLOT,
		SHIELD_SLOT,
		JOB_CAPE_SLOT,
		EARRING_SLOT,
		NECKLACE_SLOT,
		LEFT_RING_SLOT,
		RIGHT_RING_SLOT,
		MAX_EQUIPMENT_SLOT
	};

	Item ();

    Item (const boost::shared_ptr<ItemProperty> &property);

    Item (const boost::shared_ptr<ItemProperty> &property, const Item::extended_type& extended);

	void ID (const uint32_t newID);

	uint32_t ID () const;

    boost::shared_ptr<ItemProperty> property () const;

	void set_enchant_lv (uint8_t lv);

	void set_position (uint8_t pos);

	void set_quantity (uint32_t quantity);

	void set_durability (uint32_t durability);

	void set_place (int place);

	void set_physical_percent (int percent);

	void set_magical_percent (int percent);

	void set_durability_percent (int percent);

	void set_ap_ratio_percent (int percent);

	void set_cb_ratio_percent (int percent);

	void set_physical_reinf_percent (int percent);

	void set_magical_reinf_percent (int percent);

	uint32_t get_id () const;

	ITEM_TYPE get_type () const;

	uint8_t get_enchant_lv () const;

	uint8_t get_position () const;

	uint32_t get_quantity () const;

	uint32_t get_durability () const;

	uint32_t get_max_durability () const;

	std::vector<attribute> get_attr_list() const;

	uint8_t get_max_stack () const;

	float attack_distance () const;

	RACE get_req_race () const;

	/**
	 *
	 *  @brief Obtains item storage feed.
	 *
	 **/

	uint32_t get_storage_feed () const;

	/**
	 *
	 *  @brief Obtains item sell price at shops.
	 *
	 **/

	uint32_t get_sell_price () const;

	uint16_t physical_defense () const;

	uint16_t magical_defense () const;

	float physical_absorb () const;

	float magical_absorb () const;

	uint16_t hit_ratio () const;

	uint16_t parry_ratio () const;

	uint8_t critical_ratio () const;

	uint8_t block_ratio () const;

	std::pair<uint16_t,uint16_t> physical_attack () const;

	std::pair<uint16_t,uint16_t> magical_attack () const;

	std::pair<uint16_t,uint16_t> physical_reinforce () const;

	std::pair<uint16_t,uint16_t> magical_reinforce () const;

	void insert_magic_option (const Item::attribute &attr);

	bool is_opts_full () const;

	int get_place () const;

	uint64_t encoded_mods () const;

	/**
	 *
	 *  @brief Returns Physical Percent.
	 *  Weapon: physical attack power.
	 *  Shield and Armor: physical defense.
	 *  Accessory: physical absorb.
	 *
	 **/

	int get_physical_percent () const;

	/**
	 *
	 *  @brief Returns Magical Percent.
	 *  Weapon: Magical attack power.
	 *  Shield: Magical defense.
	 *  Accessory: Magical Absorb.
	 *
	 **/

	int get_magical_percent () const;

	/**
	 *
	 *  @brief Returns durability percent.
	 *
	 **/

	int get_durability_percent () const;

	int get_physical_reinf_percent () const;

	int get_magical_reinf_percent () const;

	/**
	 *
	 *  @brief Returns Attack Rating or Parry Ratio percent.
	 *  Weapon: Attack rating.
	 *  Armor: Parry Ratio.
	 *
	 **/

	int get_ap_ratio_percent () const;

	/**
	 *
	 *  @brief Returns critical or Blocking ratio percent.
	 *  Weapon: Critical.
	 *  Shield: Blocking Ratio.
	 *
	 **/

	int get_cb_ratio_percent () const;

	/**
	 *
	 *  @brief Return Item gender.
	 *
	 **/

	GENDER get_gender () const;

	uint8_t degree () const;

	uint8_t sox_type () const;

	/**
	 *
	 *  @brief Set Item alternate position, used for stall and exchange.
	 *
	 **/

	void set_alt_position (uint8_t pos) { m_AltPos = pos; }

	/**
	 *
	 *  @brief Obtain item alternate position, used for stall and exchange.
	 *
	 **/

	inline uint8_t get_alt_position () const { return m_AltPos; }


	void set_stall_quantity (uint16_t quantity) { m_StallQuantity = quantity; }

	void set_stall_price (uint64_t price) { m_StallPrice = price; }

	inline uint16_t get_stall_quantity () const { return m_StallQuantity; }

	inline uint64_t get_stall_price () const { return m_StallPrice; }

	bool check_level (const uint8_t level) const;

	bool check_mastery (const Mastery& mastery) const;

	bool check_combination (const Item& item) const;

	/*---- STONE FUNCTIONS ---- */

	void set_assimilation (const uint8_t percent);

	uint8_t get_assimilation () const;

	/*---- PET FUNCTIONS ---- */

	void set_pet_name (const std::string &name);

	void set_pet_npc_id (const uint32_t ID);

	void set_pet_status (const uint8_t status);

	const std::string& get_pet_name () const;

	uint32_t get_pet_npc_id () const;

	uint8_t get_pet_status () const;

	/* ---- ---- */

	bool is_tradable () const;

	bool is_dropable () const;

	bool is_storageable () const;

	bool can_crit () const;

	bool IsWeapon () const;

	bool IsShield () const;

	bool IsArmor () const;

	bool IsAccessory () const;

    bool IsCape () const;

	bool EnchantInc (bool WithLucky);

	bool IsRepairable () const;

	void Repair ();

	uint32_t RepairCost () const;

private:

	void calculate_property ();

	void calculate_type ();

protected:

	ITEM_TYPE m_Type;

    boost::shared_ptr<ItemProperty> m_base_property;
	extended_type m_ext_property;

	uint8_t m_AltPos;

	uint16_t m_StallQuantity;
	uint64_t m_StallPrice;

	uint8_t m_Assimilation;

	std::string m_PetName;
	uint32_t m_PetNPC;
	uint8_t m_PetStatus;

	bool m_can_crit;
	bool m_is_dropable;
	bool m_is_tradable;
	bool m_is_storageable;
	bool m_is_exchangeable;
};

void set_default_position (Item& item);

#endif //__SLES_ITEM_HPP__
