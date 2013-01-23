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

#include "player.hpp"
#include "skill.hpp"
#include "guild.hpp"
#include "party.hpp"
#include "stall.hpp"
#include "exchange_window.hpp"
#include "entity_property.hpp"
#include "transport.hpp"

/// GLOBAL INCLUDES
#include <cmath>
#include <algorithm>
#include <functional>
#include <boost/bind.hpp>

template <typename T>
inline T linear_regression (const T &value, const short percent)
{
	return  ((100+percent)*value)/100;
}

Player::Player (const boost::shared_ptr<EntityProperty> &property)
	: 	NPC(property),
        m_ConnectionID(0),
		m_AvaStore(new Storage(5,STORAGE_AVATAR)),
		m_ExchangeWnd(new ExchangeWnd()),
        m_TransportID(0)
{
    m_BaseChar.RefCharID = property->refID;

    m_mastery_tree = MasteryTree::Create(race());
	m_mastery_tree.SetCharacterLevel(m_BaseChar.Level);

    m_InvStore.reset(new Storage(m_BaseChar.InvSlots,STORAGE_INVENTORY));
    m_MainStore.reset(new Storage(m_BaseChar.StoreSlots,STORAGE_MAIN));

	m_HP = m_BaseChar.HP;
	m_MP = m_BaseChar.MP;

	m_exchange_approve = false;
	m_help_mark = false;
	m_Invitation = INVITATION_NONE;
	m_invitation_player = 0;

	calculate_stats();

    if (is_gm())
        m_BaseChar.Name = "[GM]" + m_BaseChar.Name;

    m_CurrentTask = PLAYER_TASK_IDLE;
}

Player::Player (const boost::shared_ptr<EntityProperty>& basenpc, const std::vector<Item>& item_list,
				const std::string& name, uint8_t volume)
	: 	NPC(basenpc),
        m_ConnectionID(0),
		m_AvaStore(new Storage(5,STORAGE_AVATAR)),
		m_ExchangeWnd(new ExchangeWnd()),
        m_TransportID(0)
{
    m_BaseChar.RefCharID = basenpc->refID;
	m_BaseChar.Name = name;
	m_BaseChar.Volume = volume;

	m_mastery_tree = MasteryTree::Create(race());
	m_mastery_tree.SetCharacterLevel(m_BaseChar.Level);

    m_InvStore.reset(new Storage(m_BaseChar.InvSlots,STORAGE_INVENTORY));
    m_MainStore.reset(new Storage(m_BaseChar.StoreSlots,STORAGE_MAIN));

	m_HP = m_BaseChar.HP;
	m_MP = m_BaseChar.MP;

	m_exchange_approve = false;
	m_help_mark = false;
	m_Invitation = INVITATION_NONE;
	m_invitation_player = 0;

	std::for_each(item_list.begin(),item_list.end(), boost::bind(&Storage::InsertItem,m_InvStore,_1));

	int weapon_type = m_InvStore->GetItem(Item::WEAPON_SLOT).get_type();

    switch(m_property->Race)
	{
    case RACE_EU:
		switch(weapon_type)
		{
		case ITEM_EU_TSTAFF:
		case ITEM_EU_DARKSTAFF:
			set_position(Coord(1459.8226, 1468.6079, 79.168945, ZoneID(77,105)));
			break;
		case ITEM_EU_SWORD:
		case ITEM_EU_TSWORD:
		case ITEM_EU_AXE:
		default:
			set_position(Coord(1180.5, 383.594, 80.146, ZoneID(79,107)));
			break;
		}

		m_BaseChar.ReturnID = 22;
        break;
    case RACE_CH:
		switch (weapon_type)
		{
		case ITEM_CH_SPEAR:
		case ITEM_CH_BOW:
			set_position(Coord(953.57257, 165.26994, -0.26359621, ZoneID(168,97)));
			break;
		case ITEM_CH_SWORD:
		case ITEM_CH_BLADE:
		case ITEM_CH_TBLADE:
		default:
			set_position(Coord(956.6355, 1077.78, 40, ZoneID(168,98)));
			break;
		}

		m_BaseChar.ReturnID = 3;
        break;
    default:
        break;
	}

	calculate_stats();

    if (is_gm())
        m_BaseChar.Name = "[GM]" + m_BaseChar.Name;

    m_CurrentTask = PLAYER_TASK_IDLE;
}

Player::Player (const boost::shared_ptr<EntityProperty>& basenpc, const PlayerProperty& basechar)
	: 	NPC(basenpc),
        m_ConnectionID(0),
		m_BaseChar(basechar),
		m_AvaStore(new Storage(5,STORAGE_AVATAR)),
		m_ExchangeWnd(new ExchangeWnd()),
        m_TransportID(0)
{
    m_mastery_tree = MasteryTree::Create(race());
	m_mastery_tree.SetCharacterLevel(m_BaseChar.Level);

    m_InvStore.reset(new Storage(m_BaseChar.InvSlots,STORAGE_INVENTORY));
    m_MainStore.reset(new Storage(m_BaseChar.StoreSlots,STORAGE_MAIN));

	m_HP = m_BaseChar.HP;
	m_MP = m_BaseChar.MP;
	m_Angle = m_BaseChar.Angle;

	set_position(Coord(m_BaseChar.X, m_BaseChar.Y, m_BaseChar.Z,m_BaseChar.Zone));

	m_exchange_approve = false;
	m_help_mark = false;
	m_Invitation = INVITATION_NONE;
	m_invitation_player = 0;

	calculate_stats();

    if (is_gm())
        m_BaseChar.Name = "[GM]" + m_BaseChar.Name;

    if (!m_HP)
        m_StatusState = STATUS_ACTION_DEAD;

    m_CurrentTask = PLAYER_TASK_IDLE;
}

Player::~Player ()
{
}

void Player::ID (const uint32_t newID)
{
    m_BaseChar.ID = newID;
}

uint32_t Player::ID () const
{
    return m_BaseChar.ID;
}

void Player::ConnectionID(const uint32_t connID)
{
    m_ConnectionID = connID;
}

uint32_t Player::ConnectionID() const
{
    return m_ConnectionID;
}

void Player::set_name (const std::string& name)
{
	m_BaseChar.Name = name;
}

void Player::set_volume (uint8_t vol)
{
	m_BaseChar.Volume = vol;
}

void Player::set_pk (uint8_t pk)
{
	m_BaseChar.PK = pk;
}

void Player::set_murderlv (uint32_t lv)
{
	m_BaseChar.PKLevel = lv;
}

void Player::set_gm (bool flag)
{
	m_BaseChar.GM = flag;
}

void Player::set_rank (PLAYER_RANK rank)
{
	m_BaseChar.Rank = rank;
}

void Player::set_berserk_count (uint8_t count)
{
	m_BaseChar.BskCount = count;
}

void Player::set_job (const Job &job)
{
	m_Job = job;
}

std::string Player::get_name () const
{
	return m_BaseChar.Name;
}

uint8_t Player::get_volume () const
{
	return m_BaseChar.Volume;
}

uint8_t Player::get_maxlv () const
{
	return m_BaseChar.MaxLevel;
}

uint16_t Player::get_str () const
{
	return m_Str;
}

uint16_t Player::get_int () const
{
	return m_Int;
}

uint16_t Player::get_stat () const
{
	return m_BaseChar.Stats;
}

uint32_t Player::get_sp () const
{
	return m_BaseChar.SP;
}

bool Player::is_erased () const
{
	return m_BaseChar.ExpirationMark;
}

uint64_t Player::get_exp () const
{
	return m_BaseChar.Exp;
}

uint16_t Player::get_spexp () const
{
	return m_BaseChar.SPExp;
}

uint64_t Player::get_gold () const
{
	return m_BaseChar.Gold;
}

uint8_t Player::get_pk () const
{
	return m_BaseChar.PK;
}

uint32_t Player::get_murderlv () const
{
	return m_BaseChar.PKLevel;
}

bool Player::is_gm () const
{
	return m_BaseChar.GM;
}

void Player::erase ()
{
	m_BaseChar.ExpirationMark = true;
}

void Player::restore ()
{
	m_BaseChar.ExpirationMark = false;
}

uint16_t Player::get_timeleft () const
{
	return m_BaseChar.ExpirationTime;
}

void Player::set_timeleft (uint16_t minutes)
{
	m_BaseChar.ExpirationTime = minutes;
}

Job Player::get_job () const
{
	return m_Job;
}

boost::shared_ptr<Storage> Player::get_storage(const STORAGE_TYPE type)
{
	boost::shared_ptr<Storage> st;

	switch(type)
	{
	case STORAGE_INVENTORY:
		st = m_InvStore;
		break;
	case STORAGE_AVATAR:
		st = m_AvaStore;
		break;
	case STORAGE_MAIN:
		st = m_MainStore;
		break;
	default:
		//Throw invalid type
		break;
	}

	return st;
}

bool Player::IsInvExpanded () const
{
    return m_BaseChar.InvSlotsIncreased;
}

bool Player::IsStorageExpanded () const
{
    return m_BaseChar.StorageSlotsIncreased;
}

void Player::IncInventorySlots (const uint8_t nslots)
{
    if (!m_BaseChar.InvSlotsIncreased)
    {
        m_InvStore->MaxSlotsInc(nslots);

        m_BaseChar.InvSlotsIncreased = true;
    }
}

void Player::IncStorageSlots (const uint8_t nslots)
{
    if (!m_BaseChar.StorageSlotsIncreased)
    {
        m_MainStore->MaxSlotsInc (nslots);

        m_BaseChar.StorageSlotsIncreased = true;
    }
}

bool Player::check_gender (const Item& item) const
{
	if(item.get_gender() != GENDER_ANY && item.get_gender() != get_gender())
		return false;

	return true;
}

bool Player::check_mastery (const Item& item) const
{
	bool valid = false;

	MasteryTree::const_iterator i;
	for (i = m_mastery_tree.begin(); i != m_mastery_tree.end(); ++i)
	{
		if (item.check_mastery(i->second))
		{
			valid = true;
			break;
		}
	}

	return valid;
}

void Player::skill_insert (const uint32_t ID)
{
	m_SkillList.insert(ID);
}

void Player::skill_remove (const uint32_t ID)
{
    m_SkillList.erase(ID);
}

bool Player::skill_exists (const uint32_t ID) const
{
    return m_SkillList.find(ID) != m_SkillList.end();
}

uint32_t Player::get_base_attack () const
{
	Item item = m_InvStore->GetItem(Item::WEAPON_SLOT);

	uint32_t skillid = 0;

	switch(item.get_type())
	{
	case ITEM_CH_SWORD:
	case ITEM_CH_BLADE:
		skillid = SKILL_CH_SWORD_BASE_01;
		break;
	case ITEM_CH_BOW:
		skillid = SKILL_CH_BOW_BASE_01;
		break;
	case ITEM_CH_SPEAR:
	case ITEM_CH_TBLADE:
		skillid = SKILL_CH_SPEAR_BASE_01;
		break;
	case ITEM_EU_SWORD:
		skillid = SKILL_EU_SWORD_BASE_01;
		break;
	case ITEM_EU_TSWORD:
		skillid = SKILL_EU_TSWORD_BASE_01;
		break;
	case ITEM_EU_AXE:
		skillid = SKILL_EU_AXE_BASE_01;
		break;
	case ITEM_EU_DAGGER:
		skillid = SKILL_EU_DAGGER_BASE_01;
		break;
	case ITEM_EU_CROSSBOW:
		skillid = SKILL_EU_CROSSBOW_BASE_01;
		break;
	case ITEM_EU_HARP:
		skillid = SKILL_EU_HARP_BASE_01;
		break;
	case ITEM_EU_STAFF:
		skillid = SKILL_EU_WAND_CLERIC_BASE_01;
		break;
	case ITEM_EU_TSTAFF:
		skillid = SKILL_EU_STAFF_BASE_01;
		break;
	case ITEM_EU_DARKSTAFF:
		skillid = SKILL_EU_WAND_WARLOCK_BASE_01;
		break;
	default:
		skillid = SKILL_PUNCH_01;
		break;
	}

	return skillid;
}

void Player::inc_str ()
{
    if (m_BaseChar.Stats)
    {
        m_BaseChar.Str++;
        m_BaseChar.Stats--;

        calculate_stats();
    }
}

void Player::inc_int ()
{
    if (m_BaseChar.Stats)
    {
        m_BaseChar.Int++;
        m_BaseChar.Stats--;

        calculate_stats();
    }
}

void Player::inc_exp (uint64_t amount)
{
    m_BaseChar.Exp += amount;
}

void Player::inc_spexp (uint32_t amount)
{
    m_BaseChar.SPExp += amount ;

    uint32_t new_sp = m_BaseChar.SPExp / 400;

    m_BaseChar.SPExp %= 400;

    if (new_sp)
    {
        m_BaseChar.SP += new_sp;

        if (!m_sp_changed.empty())
            m_sp_changed();
    }
}

void Player::gain_gold (uint64_t amount)
{
    m_BaseChar.Gold += amount;

    if (!m_gold_changed.empty())
        m_gold_changed();
}

bool Player::spend_gold (uint64_t amount)
{
    if ( amount > m_BaseChar.Gold)
        return false;

    m_BaseChar.Gold -= amount;

    if (!m_gold_changed.empty())
        m_gold_changed();

    return true;
}

bool Player::deposit_gold (const uint64_t amount)
{
    if (!spend_gold(amount))
        return false;

    m_BaseChar.GoldStored += amount;

    return true;
}

bool Player::withdraw_gold (const uint64_t amount)
{
    if ( amount > m_BaseChar.GoldStored)
        return false;

    gain_gold(amount);

    return true;
}

uint64_t Player::get_stored_gold () const
{
    return m_BaseChar.GoldStored;
}

bool Player::spend_sp (uint32_t sp)
{
	if ( sp > m_BaseChar.SP )
		return false;

	m_BaseChar.SP -= sp;

    if (!m_sp_changed.empty())
        m_sp_changed();

	return true;
}

void Player::help_mark (bool mark)
{
	m_help_mark = mark;
}

bool Player::help_mark () const
{
	return m_help_mark;
}

void Player::berserk (bool activate)
{
	if (activate)
	{
	    m_BaseChar.BskCount = 0;
	    m_AttributeState = ATTRIBUTE_ACTION_BERSERK;
	}
	else
	{
        m_AttributeState = ATTRIBUTE_ACTION_NORMAL;
	}

    if (!signal_state.empty())
        signal_state(STATE_ATTRIBUTE,m_AttributeState);

	calculate_stats();
}

bool Player::IsBerserk () const
{
	return m_AttributeState == ATTRIBUTE_ACTION_BERSERK;
}

uint8_t Player::level () const
{
	return m_BaseChar.Level;
}

void Player::set_model (uint32_t refCharID)
{
    m_BaseChar.RefCharID = refCharID;
}

uint32_t Player::get_model () const
{
	return m_BaseChar.RefCharID;
}

RACE Player::race () const
{
    return static_cast<RACE>(m_property->Race);
}

const MasteryTree& Player::get_mastery_tree () const
{
	return m_mastery_tree;
}

MasteryTree& Player::get_mastery_tree ()
{
	return m_mastery_tree;
}

uint8_t Player::GetMasteryBonus (const uint32_t masteryID) const
{
    uint8_t bonus = 0;

    MasteryTree::const_iterator iter = m_mastery_tree.find(masteryID);

    if (iter == m_mastery_tree.end())
        return bonus;

    switch(masteryID)
    {
    case MASTERY_CH_SWORD:
    case MASTERY_CH_SPEAR:
    case MASTERY_CH_BOW:
    case MASTERY_CH_FIRE:
    case MASTERY_CH_COLD:
    case MASTERY_CH_LIGHT:
    case MASTERY_EU_WARLOCK:
        bonus = iter->second.Level();
        break;
    default:
        break;
    }

    return bonus;
}

void Player::set_skill_list (const std::set<uint32_t> &list)
{
    m_SkillList = list;
}

const std::set<uint32_t>& Player::get_skill_list () const
{
    return m_SkillList;
}

void Player::set_mastery_tree (const MasteryTree& tree)
{
	m_mastery_tree = tree;
}

std::vector<Quest> Player::get_completed_quest_list () const
{
	return m_CompletedQuests;
}

std::vector<Quest> Player::get_pending_quest_list () const
{
	return m_PendingQuests;
}

uint32_t Player::aggro () const
{
	return 1;
}

int Player::get_rank() const
{
	return m_BaseChar.Rank;
}

uint8_t Player::get_berserk_count() const
{
	return m_BaseChar.BskCount;
}

void Player::set_return_point (const uint32_t ID)
{
    m_BaseChar.ReturnID = ID;
}

uint32_t Player::get_return_point() const
{
	return m_BaseChar.ReturnID;
}

bool Player::is_stalling () const
{
	return m_Stall != 0;
}

void Player::create_stall (const std::string& name)
{
	boost::shared_ptr<Stall> stall(new Stall(name,m_UniqueID));

	join_stall(stall);
}

void Player::close_stall ()
{
	if (m_Stall)
	{
		if ( m_Stall->get_owner() == m_UniqueID )
			m_Stall->close();
		else
			m_Stall->signal_close.disconnect(boost::bind(&Player::close_stall,this));

		m_Stall.reset();
	}
}

void Player::join_stall (boost::shared_ptr<Stall> stall)
{
    if (stall)
    {
        m_Stall = stall;

        if (m_Stall->get_owner() != m_UniqueID)
        {
            m_Stall->enter(m_UniqueID,m_BaseChar.Name);
            m_Stall->signal_close.connect(boost::bind(&Player::close_stall,this));
        }
    }
}

void Player::leave_stall ()
{
	if (m_Stall && m_Stall->get_owner() != m_UniqueID)
	{
		m_Stall->leave(m_UniqueID);

		close_stall();
	}
}

boost::shared_ptr<Stall> Player::get_stall () const
{
	return m_Stall;
}

void Player::set_stall_avatar (const uint32_t avatarID)
{
    m_BaseChar.StallAvatarID = avatarID;
}

uint32_t Player::get_stall_avatar () const
{
    return m_BaseChar.StallAvatarID;
}

void Player::set_invitation (const INVITATION_TYPE type)
{
	m_Invitation = type;
}

INVITATION_TYPE Player::get_invitation () const
{
	return m_Invitation;
}

void Player::set_invitation_player (const uint32_t ID)
{
	m_invitation_player = ID;
}

uint32_t Player::get_invitation_player () const
{
	return m_invitation_player;
}

boost::shared_ptr<ExchangeWnd> Player::get_exchange_window () const
{
	return m_ExchangeWnd;
}

void Player::set_exchange_approval (bool approve)
{
	m_exchange_approve = approve;
}

bool Player::is_exchange_approved () const
{
	return m_exchange_approve;
}

void Player::close_exchange ()
{
	m_ExchangeWnd->clear();
}

bool Player::levelup ()
{
    ++m_BaseChar.Level;

    if (m_BaseChar.MaxLevel < m_BaseChar.Level)
        m_BaseChar.MaxLevel = m_BaseChar.Level;

    m_BaseChar.Stats += 3;

	m_BaseChar.Str++;
    m_BaseChar.Int++;

    m_BaseChar.Exp = 0;

    return true;
}

void Player::create_party ()
{
	m_Party = boost::shared_ptr<Party>(new Party());

	m_Party->join(shared_from_this());
}

void Player::leave_party ()
{
	if (m_Party->get_master()->GetUniqueID() == m_UniqueID)
		m_Party->dismiss();
	else
		m_Party->leave(shared_from_this());

	m_Party.reset();
}

void Player::clear_party ()
{
	m_Party.reset();
}

boost::shared_ptr<Party> Player::get_party () const
{
	return m_Party;
}

bool Player::in_party () const
{
	return m_Party != 0;
}

void Player::join_party (boost::shared_ptr<Party> party)
{
	m_Party = party;

	m_Party->join(shared_from_this());
}

bool Player::check_item_combination (const Item& item, const Item& new_item) const
{
	if ( item.get_position() < Item::WEAPON_SLOT)
		return item.check_combination(new_item);

	return true;
}

void Player::calculate_stats ()
{
	Item weapon = m_InvStore->GetItem(Item::WEAPON_SLOT);

	m_Str = m_BaseChar.Str;
	m_Int = m_BaseChar.Int;

    m_MA = std::make_pair(0,0);
    m_PA = std::make_pair(0,0);

	m_IGD = 0;
	m_MOL = 0;
	m_ETL = 0;
	m_CPR = 0;
	m_GoldDrop = 0;

	//CALCULATE BASE ATTACK POWER AND DISTANCE
    m_AD = 0;

	//CALCULATE HP AND MP
	m_MaxHP = 0;
	m_HPRgn = 100;
	m_MaxMP = 0;
	m_MPRng = 100;
	m_AbsorbMP = 0;

    // WALL VALUES
    m_WallHP = 0;
    m_WallAbs = 0;
    m_WallSkillID = 0;

	//CALCULATE BASE DEFENSE POWER
	m_PD = 6;
	m_MD = 6;
	m_HR = 10 + m_BaseChar.MaxLevel;
	m_PR = 10 + m_BaseChar.MaxLevel;

    m_BR = 0;
    m_CR = 0;

    float wlkspeed = m_property->WalkSpeed;
    float runspeed = m_property->RunSpeed;
    float bskspeed = m_property->BersekSpeed;

	short speed_percent = 0;
	short hp_percent = 0;
	short mp_percent = 0;
	short pr_percent = 0;
	short hr_percent = 0;
	short pa_percent = 0;
	short ma_percent = 0;
	short pd_percent = 0;
	short md_percent = 0;

	Storage::const_iterator iter;
	for (iter = m_InvStore->begin(); iter != m_InvStore->end(); ++iter)
	{
		if (iter->first < Item::MAX_EQUIPMENT_SLOT)
		{
			if (iter->first < Item::JOB_CAPE_SLOT)
			{
				if (iter->first == Item::WEAPON_SLOT)
				{
					m_HR += iter->second.hit_ratio();
					m_CR += iter->second.critical_ratio();
                    m_AD = weapon.attack_distance();
                    m_PA = weapon.physical_attack();
                    m_MA = weapon.magical_attack();
				}
				else
				{
					m_PD += iter->second.physical_defense();
					m_MD += iter->second.magical_defense();

					m_PR += iter->second.parry_ratio();

					if (iter->first == Item::SHIELD_SLOT)
					{
						m_BR += iter->second.block_ratio();
					}
				}
			}
			else if (iter->first == Item::JOB_CAPE_SLOT)
			{
				//TODO: ADD JOB BLUES!!
			}
			else
			{
				m_PAbs += iter->second.physical_absorb();
				m_MAbs += iter->second.magical_absorb();

				//TODO: ADD BLUES!!
			}

			//m_Str += iter->second.get_str();
			//m_Int += iter->second.get_int();
		}
	}

	m_MPDec = 0;
	m_HealHP = 0;
	m_HealMP = 0;

    for ( std::multimap<uint32_t,Buff>::const_iterator k = m_buff_list.begin(); k != m_buff_list.end(); ++k)
	{
        for (std::vector<BuffEffect>::const_iterator j = k->second.Effects.begin(); j != k->second.Effects.end(); ++j)
		{
			switch(j->ID)
			{
			case BUFF_CRITICAL_INC:
				m_CR += j->Arg[0];
				break;
			case BUFF_SPEED_INC:
			case BUFF_SPEED_INC2:
			case BUFF_SPEED_INC3:
				speed_percent += j->Arg[0];
				break;
			case BUFF_PARRY_INC:
				m_PR += j->Arg[0];
				pr_percent += j->Arg[1];
				break;
			case BUFF_PARRY_DEC:
				m_PR -= j->Arg[0];
				break;
			case BUFF_HIT_INC:
				m_HR += j->Arg[0];
				hr_percent += j->Arg[1];
				break;
			case BUFF_HIT_DEC:
				m_HR -= j->Arg[0];
				break;
			case BUFF_INT_INC:
				if (m_BaseChar.Int*j->Arg[1]/100 >= j->Arg[0])
					m_Int += j->Arg[0];
				break;
			case BUFF_STR_INC:
				if (m_BaseChar.Str*j->Arg[1]/100 >= j->Arg[0])
					m_Str += j->Arg[0];
				break;
			case BUFF_HP_INC:
				m_MaxHP += j->Arg[0];
				hp_percent += j->Arg[1];
				break;
			case BUFF_MP_INC:
				m_MaxMP += j->Arg[0];
				mp_percent += j->Arg[1];
				break;
			case BUFF_RANGE_INC:
				m_AD += j->Arg[0];
				break;
			case BUFF_DAMAGE_INC:
				pa_percent += j->Arg[0];
				ma_percent += j->Arg[0];
				break;
			case BUFF_SHIELD_PWR_4_DAMAGE:
				{
					Item shield = m_InvStore->GetItem(Item::SHIELD_SLOT);

					if (shield.get_id())
					{
						m_PD -= j->Arg[0]*shield.physical_defense()/100;

						m_PA.first += j->Arg[1];
						m_PA.second += j->Arg[1];
					}
				}
				break;
			case BUFF_REINCARNATION:
				m_HPRgn += j->Arg[0];
				m_MPRng += j->Arg[1];
				break;
			case BUFF_IGNORE_DEFENSE:
				m_IGD += j->Arg[0];
				break;
			case BUFF_CRIT_PARRY_INC:
				m_CPR += j->Arg[0];
				break;
			case BUFF_GOLD_DROP_INC:
				m_GoldDrop += j->Arg[0];
				break;
			case BUFF_MAGICOPTION_LUCK_INC:
				m_MOL += j->Arg[0];
				break;
			case BUFF_ENCHANT_LUCK_INC:
				m_ETL += j->Arg[0];
				break;
			case BUFF_MP_ABSORB:
				m_AbsorbMP += j->Arg[0];
				break;
			case BUFF_DAMAGE_PWR_INC:
				m_PA.first += j->Arg[0];
				m_PA.second += j->Arg[0];

				m_MA.first += j->Arg[1];
				m_MA.second += j->Arg[1];
				break;
			case BUFF_DETECT:
			case BUFF_DETECT_V2:
				//(5 Stealth) (6 Invisible) (7 All)
				//Max level
				break;
			case BUFF_REQUIRED_MP_DEC:
				m_MPDec += j->Arg[0];
				break;
			case BUFF_HEAL_INC:
				m_HealHP += j->Arg[0];
				m_HealMP += j->Arg[1];
				break;
			case BUFF_ABSORB_WALL:
                if (j->Arg[0] == WALL_TYPE_PHYSICAL)
                    m_WallType = true;
                else
                    m_WallType = false;

                m_WallHP = j->Arg[1];
                //0
                m_WallAbs = j->Arg[3];
                m_WallSkillID = k->first;
				break;
			case BUFF_BLOCK_INC:
				//BLOCK TYPE +0 (15 ALL) (11 Magic %) (7 Physical %)
				//BLOCK RATE +1
                m_BR += j->Arg[1];
				break;
			case BUFF_DAMAGE_REFLECT:
				//PROBABILITY [%]
				//PHY DAMAGE RETURNED [%]
				//MAG DAMAGE RETURNED [%]
				//RETURN RANGE
				break;
			case BUFF_ABSORB:
				//TYPE (3 - Physical Absorb) (12 & 15 - Absorb all) (4 Physical) (8 Magical) (6 Physical ranged)
				//Amount (%)
				//0
				break;
			case BUFF_DEFENSE_INC:
				m_PD += j->Arg[0];
				m_MD += j->Arg[1];
				//Caster Range
				break;
			case BUFF_DEFENSE_DEC:
				pd_percent -= j->Arg[0];
				md_percent -= j->Arg[1];
				break;
			case BUFF_DAMAGE_DEC:
				pa_percent -= j->Arg[0];
				ma_percent -= j->Arg[1];
				break;
			case BUFF_HP_DEC:
				//Duration
				//0
				hp_percent -= j->Arg[2];
				//2
				break;
			default:
				break;
			}
		}
	}

    if (weapon.get_id())
    {
        m_PA.first += get_str()*(0.5 + weapon.physical_reinforce().first*0.001);
    
        m_PA.second += get_str()*(0.5 + weapon.physical_reinforce().second*0.001);

        m_MA.first += get_int()*(0.5 + weapon.magical_reinforce().first*0.001);

        m_MA.second += get_int()*(0.5 + weapon.magical_reinforce().second*0.001);
    }

    m_PD += (get_str()/2);
    m_MD += (get_int()/2);

	m_MaxHP += std::pow(1.02,level()- 1) * ( get_str() * 10);
	m_MaxMP += std::pow(1.02,level()- 1) * ( get_int() * 10);

	m_MaxHP = linear_regression(m_MaxHP,hp_percent);

	if (m_HP > m_MaxHP)
		m_HP = m_MaxHP;

	m_MaxMP = linear_regression(m_MaxMP,mp_percent);

	if (m_MP > m_MaxMP)
		m_MP = m_MaxMP;

    if (m_Imbue.skillID)
	{
		m_MA.first += m_Imbue.attack.min_pwr;
		m_MA.second += m_Imbue.attack.max_pwr;

		pa_percent += m_Imbue.attack.percent;
	}

	m_PR = linear_regression(m_PR,pr_percent);
	m_HR = linear_regression(m_HR,hr_percent);
	m_PA.first = linear_regression(m_PA.first,pa_percent);
	m_PA.second = linear_regression(m_PA.second,pa_percent);
	m_MA.first = linear_regression(m_MA.first,ma_percent);
	m_MA.second = linear_regression(m_MA.second,ma_percent);
	m_PD = linear_regression(m_PD,pd_percent);
	m_MD = linear_regression(m_MD,md_percent);


	wlkspeed = linear_regression(wlkspeed,speed_percent);
	runspeed = linear_regression(runspeed,speed_percent);
	bskspeed = linear_regression(bskspeed,speed_percent);

	if (IsBerserk())
	{
		wlkspeed *=2;
		runspeed *=2;
		bskspeed *=2;

		m_PA.first *= 2;
		m_PA.second *= 2;
		m_MA.first *= 2;
		m_MA.second *= 2;
	}

    set_speed(wlkspeed,runspeed,bskspeed,false);

    calculate_status_effects();

    if (!signal_stats.empty())
        signal_stats();

    if (!signal_speed.empty())
        signal_speed();
}

uint32_t Player::get_guild_id () const
{
    return m_BaseChar.GuildID;
}

void Player::set_guild (boost::shared_ptr<Guild> guild)
{
    if (guild)
    {
        m_BaseChar.GuildID = guild->get_id();

        m_Guild = guild;
    }
}

boost::shared_ptr<Guild> Player::get_guild () const
{
	return m_Guild;
}

void Player::guild_clear ()
{
    m_BaseChar.GuildID = 0;
    m_Guild.reset();
}

uint32_t Player::TransportID () const
{
    if (m_transport)
        return m_transport->GetUniqueID();

    return 0;
}

void Player::SetTransport (const boost::shared_ptr<Transport> &transport)
{
    m_transport = transport;
}

boost::shared_ptr<Transport> Player::GetTransport () const
{
    return m_transport;
}

void Player::TransportTerminate ()
{
    m_transport.reset();
}

void Player::resurrect (bool set_invulnerable, bool recover_xp, uint8_t recover_percent, bool max_recovery)
{
    m_StatusState = STATUS_ACTION_ALIVE;

    if (!signal_state.empty())
        signal_state(STATE_STATUS,m_StatusState);

    if (set_invulnerable)
    {
        m_AttributeState = ATTRIBUTE_ACTION_INVUNERABLE;

        if (!signal_state.empty())
            signal_state(STATE_ATTRIBUTE,m_AttributeState);
    }

    if (recover_xp)
    {
        ///TODO: RECOVER XP LOST
    }

    if (max_recovery)
    {
        m_HP = max_hp();

        if (!mp())
            m_MP = max_mp();
    }
    else
    {
        m_HP = (max_hp()*0.2);

        if (!mp())
            m_MP = (max_mp()*0.2);
    }
}

void Player::clear_invulnerable ()
{
    m_AttributeState = ATTRIBUTE_ACTION_NORMAL;

    if (!signal_state.empty())
        signal_state(STATE_ATTRIBUTE,m_AttributeState);
}

bool Player::IsJobbing () const
{
    Storage::const_iterator iter = m_InvStore->find(Item::JOB_CAPE_SLOT);

    if (iter == m_InvStore->end() || iter->second.get_type() == ITEM_PVP_CAPE)
        return false;

    return true;
}

bool Player::IsFreePvP () const
{
    Storage::const_iterator iter = m_InvStore->find(Item::JOB_CAPE_SLOT);

    if (iter == m_InvStore->end() || iter->second.get_type() != ITEM_PVP_CAPE)
        return false;

    return true;
}

void Player::change_skin (const uint32_t refCharID, const uint8_t volume)
{
    m_BaseChar.RefCharID = refCharID;
    m_BaseChar.Volume = volume;
}

void Player::EstablishRecallPoint ()
{
    m_RecallPoint = get_position();
}

const Coord& Player::GetRecallPoint () const
{
    return m_RecallPoint;
}

void Player::EstablishDeathPoint ()
{
    m_DeathPoint = get_position();
}

const Coord& Player::GetDeathPoint () const
{
    return m_DeathPoint;
}

bool Player::IsTaskInterruptable () const
{
    if (m_CurrentTask > PLAYER_TASK_ITEM_MOVE)
        return false;

    return true;
}

bool Player::IsItemAvailable (const uint32_t itemType) const
{
    std::map<uint32_t,uint32_t>::const_iterator iter = m_cooldown_itemgroup.find(itemType);

    return iter == m_cooldown_itemgroup.end();
}

void Player::AddItemCooldown (const uint32_t taskID, const uint32_t itemType)
{
    m_cooldown_itemgroup.insert(std::make_pair(itemType,taskID));
}

void Player::RemoveItemCooldown (const uint32_t itemType)
{
    m_cooldown_itemgroup.erase(itemType);
}

const std::map<uint32_t,uint32_t>& Player::GetItemCooldownList() const
{
    return m_cooldown_itemgroup;
}

std::vector<std::pair<uint32_t,uint32_t> > Player::RemoveInvalidatedBuffs ()
{
    std::vector<std::pair<uint32_t,uint32_t> > ret;

    Item weapon = m_InvStore->GetItem(Item::WEAPON_SLOT);
    Item shield = m_InvStore->GetItem(Item::SHIELD_SLOT);

    bool remove = false;

    for (std::map<uint32_t,Buff>::iterator i = m_buff_list.begin(); i != m_buff_list.end();)
    {
        if (!i->second.is_weapon_valid(static_cast<REQ_WEAPON_TYPE>(weapon.get_type())))
            remove = true;

        if (i->second.ReqShield && !shield.get_id())
           remove = true;

        if (remove)
        {
            ret.push_back(std::make_pair(i->second.taskID,i->second.CastID));
            m_buff_list.erase(i++);

            remove = false;
        }
        else
            ++i;
    }

    return ret;
}

void Player::RecalculateStats ()
{
    calculate_stats();
}

bool CanPlayerInteract (const boost::shared_ptr<Player> &p1, const boost::shared_ptr<Player> &p2)
{
    bool allow = true;

    if (p1->IsJobbing())
    {
        if (p2->IsJobbing())
        {
            uint8_t inv_job = p1->get_job().get_type();
            uint8_t player_job = p2->get_job().get_type();

            if (inv_job == JOB_THIEF)
            {
                if (player_job != JOB_THIEF)
                    allow = false;
            }
            else
            {
                if (player_job == JOB_THIEF)
                    allow = false;
            }
        }
        else
            allow = false;
    }
    else
    {
        if (p2->IsJobbing())
            allow = false;
        else
        {
            if (p1->IsFreePvP())
            {
                if (p2->IsFreePvP())
                    allow = true;
                else
                    allow = false;
            }
            else
            {
                if (p2->IsFreePvP())
                    allow = false;
            }
        }
    }

    return allow;
}
