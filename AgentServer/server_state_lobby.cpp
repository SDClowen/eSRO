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

#include "server_state_lobby.hpp"
#include "server_connection.hpp"
#include "server.hpp"

/// EPL INCLUDES
#include <opcodes_shard_client.hpp>
#include <packet_lobby.hpp>
#include <packet_player.hpp>

#include "world.hpp"
#include "player.hpp"
#include "storage.hpp"
#include "skill.hpp"
#include "item_factory.hpp"
#include "character_factory.hpp"
#include "skill_manager.hpp"

#include "query/player_query.hpp"
#include "query/item_query.hpp"
#include "query/mastery_query.hpp"
#include "query/skill_query.hpp"
#include "query/job_query.hpp"
#include "query/hotkeys_query.hpp"
#include "query/block_query.hpp"

#define MAX_PLAYER_COUNT 4

StateLobby::StateLobby (Server *srv, srv::Connection *conn)
    : srv::ConnectionState(conn), m_loaded(false), m_server(srv)
{
    m_opcode_table[CLIENT_LOBBY] = boost::bind(&StateLobby::OnHandleCharacter,this,_1);
    m_opcode_table[CLIENT_LOBBY_SELECT] = boost::bind(&StateLobby::OnSelectCharacter,this,_1);
}

StateLobby::~StateLobby()
{
}

void StateLobby::Close()
{
}

int StateLobby::OnHandleCharacter(IPacket &packet)
{
    uint8_t action = packet.Read<uint8_t>();

    int ret;

    switch(action)
    {
    case srv_pkt::LOBBY_ACTION_CREATE:
        ret = OnCreateCharacter(packet);
        break;
    case srv_pkt::LOBBY_ACTION_LIST:
        ret = OnListCharacter(packet);
        break;
    case srv_pkt::LOBBY_ACTION_ERASE:
        ret = OnEraseCharacter(packet);
        break;
    case srv_pkt::LOBBY_ACTION_VALIDATE_NAME:
        ret = OnValidateCharacterName(packet);
        break;
    case srv_pkt::LOBBY_ACTION_RESTORE:
        ret = OnRestoreCharacter(packet);
        break;
    default:
        ret = MSG_ERROR_ARG;
        break;
    }

    return ret;
}

int StateLobby::OnCreateCharacter (IPacket &packet)
{
    /**
     *
     *  BASIC ITEMS ID'S
     *  (THE ID MIGHT CHANGE ON FUTURE UPDATES SO IT NEEDS TO BE CHECKED)
     *
     **/

    enum
    {
        /**
         *
         *  CHINESE BASIC ITEMS.
         *
         **/

        ITEM_CH_SWORD_01_A_DEF	= 3632,
        ITEM_CH_BLADE_01_A_DEF,
        ITEM_CH_SPEAR_01_A_DEF,
        ITEM_CH_TBLADE_01_A_DEF,
        ITEM_CH_BOW_01_A_DEF,

        ITEM_CH_M_HEAVY_01_BA_A_DEF,
        ITEM_CH_M_HEAVY_01_LA_A_DEF,
        ITEM_CH_M_HEAVY_01_FA_A_DEF,
        ITEM_CH_M_LIGHT_01_BA_A_DEF,
        ITEM_CH_M_LIGHT_01_LA_A_DEF,
        ITEM_CH_M_LIGHT_01_FA_A_DEF,
        ITEM_CH_M_CLOTHES_01_BA_A_DEF,
        ITEM_CH_M_CLOTHES_01_LA_A_DEF,
        ITEM_CH_M_CLOTHES_01_FA_A_DEF,
        ITEM_CH_W_HEAVY_01_BA_A_DEF,
        ITEM_CH_W_HEAVY_01_LA_A_DEF,
        ITEM_CH_W_HEAVY_01_FA_A_DEF,
        ITEM_CH_W_LIGHT_01_BA_A_DEF,
        ITEM_CH_W_LIGHT_01_LA_A_DEF,
        ITEM_CH_W_LIGHT_01_FA_A_DEF,
        ITEM_CH_W_CLOTHES_01_BA_A_DEF,
        ITEM_CH_W_CLOTHES_01_LA_A_DEF,
        ITEM_CH_W_CLOTHES_01_FA_A_DEF,
        ITEM_CH_SHIELD_01_A_DEF = 10488,

        ITEM_ETC_AMMO_ARROW_01_DEF = 3655,
        ITEM_ETC_AMMO_BOLT_01_DEF = 10487,

        /**
         *
         *  EUROPEAN BASIC ITEMS.
         *
         **/

        ITEM_EU_DAGGER_01_A_DEF = 10489,
        ITEM_EU_SWORD_01_A_DEF,
        ITEM_EU_TSWORD_01_A_DEF,
        ITEM_EU_AXE_01_A_DEF,
        ITEM_EU_CROSSBOW_01_A_DEF,
        ITEM_EU_DARKSTAFF_01_A_DEF,
        ITEM_EU_TSTAFF_01_A_DEF,
        ITEM_EU_HARP_01_A_DEF,
        ITEM_EU_STAFF_01_A_DEF,
        ITEM_EU_SHIELD_01_A_DEF,

        ITEM_EU_M_HEAVY_01_BA_A_DEF = 11219,
        ITEM_EU_M_HEAVY_01_LA_A_DEF,
        ITEM_EU_M_HEAVY_01_FA_A_DEF,
        ITEM_EU_M_LIGHT_01_BA_A_DEF,
        ITEM_EU_M_LIGHT_01_LA_A_DEF,
        ITEM_EU_M_LIGHT_01_FA_A_DEF,
        ITEM_EU_M_CLOTHES_01_BA_A_DEF,
        ITEM_EU_M_CLOTHES_01_LA_A_DEF,
        ITEM_EU_M_CLOTHES_01_FA_A_DEF,
        ITEM_EU_W_HEAVY_01_BA_A_DEF,
        ITEM_EU_W_HEAVY_01_LA_A_DEF,
        ITEM_EU_W_HEAVY_01_FA_A_DEF,
        ITEM_EU_W_LIGHT_01_BA_A_DEF,
        ITEM_EU_W_LIGHT_01_LA_A_DEF,
        ITEM_EU_W_LIGHT_01_FA_A_DEF,
        ITEM_EU_W_CLOTHES_01_BA_A_DEF,
        ITEM_EU_W_CLOTHES_01_LA_A_DEF,
        ITEM_EU_W_CLOTHES_01_FA_A_DEF
    };

    enum
    {
        POS_CHEST,
        POS_LEG,
        POS_FOOT,
        POS_WEAPON
    };

    if (!m_loaded || m_player_list.size() >= MAX_PLAYER_COUNT)
    {
        boost::shared_ptr<OPacket> pkt(new OPacket);
        
        if (!pkt)
            return MSG_ERROR_MEMORY;

        srv_pkt::Lobby(pkt,srv_pkt::LOBBY_ACTION_CREATE,srv_pkt::LOBBY_ERROR_MAX_CHARACTERS);
        m_connection->Send(pkt);
        return MSG_SUCCESS;
    }

    std::string name = packet.Read();

    uint32_t model = packet.Read<uint32_t>();

    uint8_t volume = packet.Read<uint8_t>();

    uint32_t items[4];

    for (int i = 0; i < 4; ++i)
        items[i] = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    /// Validate data sent

    if (!IsNameValid(name))
        return MSG_SUCCESS;

    ///CHECK IF THE ARMOR IS OF THE SAME TYPE
    if (items[0]+1 != items[1] || items[1] +1 != items[2])
        return MSG_ERROR_ARG;

    World *world = m_server->GetWorld();

    ///CHECK THAT THE ITEMS CORRESPOND TO THE RACE,GENDER AND WEAPON
    if ( model >= 1907 && model <= 1932 )
    {
        ///CHINESE MODEL
        if (!world->IsChineseAllowed())
        {
            syslog(LOG_INFO,"This server dont allow creating ch characters.");
            return MSG_ERROR;
        }

        ///CHECK FOR VALID CHINESE WEAPONS
        switch(items[POS_WEAPON])
        {
        case ITEM_CH_SWORD_01_A_DEF:
        case ITEM_CH_BLADE_01_A_DEF:
        case ITEM_CH_SPEAR_01_A_DEF:
        case ITEM_CH_TBLADE_01_A_DEF:
        case ITEM_CH_BOW_01_A_DEF:
            break;
        default:
            return MSG_ERROR_ARG;
        }

        if (model >= 1907 && model <= 1919)
        {
            ///CHECK FOR MEN MODEL AND ITEMS
            if (items[0] > ITEM_CH_M_CLOTHES_01_FA_A_DEF)
                return MSG_ERROR_ARG;
        }
        else
        {
            ///CHECK FOR WOMEN MODEL AND ITEMS
            if (items[0] < ITEM_CH_W_HEAVY_01_BA_A_DEF)
                return MSG_ERROR_ARG;
        }
    }
    else if ( model >= 14477 && model <= 14502 )
    {
        if (!world->IsEuropeanAllowed())
        {
            syslog(LOG_INFO,"This server dont allow creating eu characters.");
            return MSG_ERROR;
        }

         ///CHECK FOR VALID CHINESE WEAPONS
        switch(items[POS_WEAPON])
        {
        case ITEM_EU_DAGGER_01_A_DEF:
        case ITEM_EU_SWORD_01_A_DEF:
        case ITEM_EU_TSWORD_01_A_DEF:
        case ITEM_EU_AXE_01_A_DEF:
        case ITEM_EU_CROSSBOW_01_A_DEF:
        case ITEM_EU_DARKSTAFF_01_A_DEF:
        case ITEM_EU_TSTAFF_01_A_DEF:
        case ITEM_EU_HARP_01_A_DEF:
        case ITEM_EU_STAFF_01_A_DEF:
            break;
        default:
            return MSG_ERROR_ARG;
        }

        if (model >= 14477 && model <= 14489)
        {
            ///CHECK FOR MEN MODEL AND ITEMS
            if (items[0] > ITEM_EU_M_CLOTHES_01_FA_A_DEF)
                return MSG_ERROR_ARG;
        }
        else
        {
            ///CHECK FOR WOMEN MODEL AND ITEMS
            if (items[0] < ITEM_EU_W_HEAVY_01_BA_A_DEF)
                return MSG_ERROR_ARG;
        }
    }
    else
        return MSG_ERROR_ARG;

    std::vector<Item> item_list;
    for (int i = 0; i < 4; ++i)
    {
        Item itm = world->GetItemFactory()->Create(items[i]);
        item_list.push_back(itm);
    }

    ///Look if the initial weapon requires a shield or bolt or arrows.
    uint32_t extraID = 0;

    switch(items[POS_WEAPON])
    {
    case ITEM_CH_SWORD_01_A_DEF:
    case ITEM_CH_BLADE_01_A_DEF:
        extraID = ITEM_CH_SHIELD_01_A_DEF;
        break;
    case ITEM_EU_SWORD_01_A_DEF:
    case ITEM_EU_DARKSTAFF_01_A_DEF:
    case ITEM_EU_STAFF_01_A_DEF:
        extraID = ITEM_EU_SHIELD_01_A_DEF;
        break;
    case ITEM_EU_CROSSBOW_01_A_DEF:
        extraID = ITEM_ETC_AMMO_BOLT_01_DEF;
        break;
    case ITEM_CH_BOW_01_A_DEF:
        extraID = ITEM_ETC_AMMO_ARROW_01_DEF;
        break;
    default:
        break;
    }

    if (extraID)
    {
        try
        {
            Item extra = world->GetItemFactory()->Create(extraID);

            if (extraID == ITEM_ETC_AMMO_ARROW_01_DEF || extraID == ITEM_ETC_AMMO_BOLT_01_DEF)
                extra.set_quantity(extra.get_max_stack());

            item_list.push_back(extra);
        }
        catch (const Exception::InvalidItemID &error)
        {
            syslog(LOG_INFO,"Failed to create item with ID = %i",extraID);
        }
    }

    std::for_each(item_list.begin(),item_list.end(),boost::bind(&set_default_position,_1));

    boost::shared_ptr<Player> player = world->GetCharacterFactory()->Create(model, volume, name, item_list);

    if (!player)
    {
        syslog(LOG_INFO,"Failed to create player with model %i",model);
        return MSG_ERROR;
    }

    m_player_list.push_back(player);

    DB::PLAYER::Create create;
    create(m_server->DBConnection(),static_cast<srv::Connection*>(m_connection)->AccountID(),player,name);

    boost::shared_ptr<OPacket> pkt(new OPacket);

    if (!pkt)
        return MSG_ERROR_MEMORY;

    srv_pkt::Lobby(pkt,srv_pkt::LOBBY_ACTION_CREATE);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateLobby::OnListCharacter (IPacket &packet)
{
    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    if (!m_loaded)
    {
        LoadCharacters();
        m_loaded = true;
    }

    boost::shared_ptr<OPacket> pkt(new OPacket);
    srv_pkt::Lobby(pkt,m_player_list);
    m_connection->Send(pkt);

    return MSG_SUCCESS;
}

int StateLobby::OnEraseCharacter (IPacket &packet)
{
    if (!m_loaded)
        return MSG_ERROR_ORDER;

    std::string name = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    player_iterator i = FindCharacterIndex(name);

    if ( i != m_player_list.end() )
    {
        (*i)->erase();
        (*i)->set_timeleft(10080);

        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::Lobby(pkt,srv_pkt::LOBBY_ACTION_ERASE);
        m_connection->Send(pkt);

        DB::PLAYER::Erase query;
        query(m_server->DBConnection(),(*i)->ID());
    }
    else
        return MSG_ERROR_ARG;

    return MSG_SUCCESS;
}

int StateLobby::OnRestoreCharacter (IPacket &packet)
{
    if (!m_loaded)
        return MSG_ERROR_ORDER;

    std::string name = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    player_iterator i = FindCharacterIndex(name);

    if ( i != m_player_list.end() )
    {
        (*i)->restore();
        (*i)->set_timeleft(0);

        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::Lobby(pkt,srv_pkt::LOBBY_ACTION_RESTORE);
        m_connection->Send(pkt);

        DB::PLAYER::Restore query;
        query(m_server->DBConnection(),(*i)->ID());
    }
    else
        return MSG_ERROR_ARG;

    return MSG_SUCCESS;
}

int StateLobby::OnValidateCharacterName (IPacket &packet)
{
    std::string name = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    if (IsNameValid(name))
    {
        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::Lobby(pkt,srv_pkt::LOBBY_ACTION_VALIDATE_NAME);
        m_connection->Send(pkt);
    }

    return MSG_SUCCESS;
}

int StateLobby::OnSelectCharacter (IPacket &packet)
{
    std::string name = packet.Read();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    World *world = m_server->GetWorld();

    player_iterator i = FindCharacterIndex(name);

    if ( i != m_player_list.end() )
    {
        boost::shared_ptr<Player> player = *i;

        if (!player)
            return MSG_ERROR;

        DB::MASTERY::Select mselect;
        MasteryTree tree = mselect(m_server->DBConnection(),player->ID(),player->level(),player->race());

        player->set_mastery_tree(tree);

        DB::SKILL::Select sselect;
        std::set<uint32_t> skill_list = sselect(m_server->DBConnection(),player->ID());

        player->set_skill_list(skill_list);

        boost::shared_ptr<Storage> ml = player->get_storage(STORAGE_MAIN);

        DB::ITEM::Select iobj;
        std::vector<Item::extended_type> item_list = iobj(m_server->DBConnection(),player->ID(),STORAGE_MAIN);

        for (std::vector<Item::extended_type>::const_iterator i = item_list.begin(); i != item_list.end(); ++i)
        {
            try
            {
                ml->InsertItem(world->GetItemFactory()->Create(i->id,*i));
            }
            catch (Exception::InvalidItemID &error)
            {
                syslog(LOG_INFO,"Lobby::LoadCharacters() - Error while cloning item with ID = %i",i->id);
            }
        }

        DB::JOB::Select jb_sl_query;
        Job job = jb_sl_query(m_server->DBConnection(),player->ID());

        player->set_job(job);

        boost::shared_ptr<OPacket> pkt(new OPacket);
        srv_pkt::RequestIngame(pkt);
        m_connection->Send(pkt);

        DB::PLAYER::State st_query;
        st_query(m_server->DBConnection(),player->ID(),true);

        pkt.reset(new OPacket);

        srv_pkt::InitScreen(pkt);
        m_connection->Send(pkt);

        DB::HOTKEY::Select hotkey_query;
        player->m_Hotkeys = hotkey_query(m_server->DBConnection(),player->ID());

        DB::BLOCK::Select block_query;
        player->m_BlockList = block_query(m_server->DBConnection(),player->ID());

        DB::PLAYER::Autopot autopot_query;
        player->m_Autopot = autopot_query(m_server->DBConnection(),player->ID());

        if (player->IsDead())
        {
            player->resurrect(false,false,0,false);

            Teleport tlp = m_server->GetWorld()->FindTeleport(player->get_return_point());

            if (!tlp.ID)
            {
                syslog(LOG_INFO,"Invalid Return Point - %i",player->get_return_point());
                return MSG_ERROR;
            }

            player->set_position(Coord(tlp.x,tlp.y,tlp.z,tlp.Zone));
        }

        pkt.reset(new OPacket);

        srv_pkt::PlayerInfo(pkt,player,static_cast<srv::Connection*>(m_connection)->AccountID());
        m_connection->Send(pkt);

        pkt.reset(new OPacket);

        srv_pkt::EndScreen(pkt);
        m_connection->Send(pkt);

        pkt.reset(new OPacket);

        srv_pkt::PlayerID(pkt,player);
        m_connection->Send(pkt);

        /// ADD SKILL PASSIVES

        boost::shared_ptr<Skill> skill;
        for (std::set<uint32_t>::const_iterator it = skill_list.begin(); it != skill_list.end(); ++it)
        {
            skill = m_server->GetWorld()->GetSkillManager()->find(*it);

            if (skill && skill->type() == SKT_PASSIVE)
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

                if (player->is_buff_stackable(buff))
                {
                    player->RemoveLesserBuffs(buff);
                    player->insert_buff(buff);
                }
            }
        }

        m_connection->SetState(srv::STATE_GAME);
        static_cast<srv::Connection*>(m_connection)->SetPlayer(player);
    }
    else
        return MSG_ERROR_ARG;

    return MSG_SUCCESS;
}

void StateLobby::LoadCharacters ()
{
    World *world = m_server->GetWorld();

    DB::PLAYER::Select select;
    std::vector<PlayerProperty>  base_list = select(m_server->DBConnection(),
        static_cast<srv::Connection*>(m_connection)->AccountID());

    boost::shared_ptr<Player> new_player;
    for (size_t i = 0; i < base_list.size(); ++i)
    {
        base_list[i].GM = static_cast<srv::Connection*>(m_connection)->SecurityLevel();

        new_player = world->GetCharacterFactory()->Create(base_list[i]);

        if (!new_player)
        {
            syslog(LOG_INFO,"StateLobby::LoadCharacters - Failed to create character - Name %s.",
                base_list[i].Name.c_str());
            continue;
        }

        m_player_list.push_back(new_player);
     }

    Item new_item;
    for (size_t i = 0; i < m_player_list.size(); ++i)
    {
        DB::ITEM::Select iobj;
        std::vector<Item::extended_type> item_list = iobj(m_server->DBConnection(),m_player_list[i]->ID(),
                                                          STORAGE_INVENTORY);

        boost::shared_ptr<Storage> il = m_player_list[i]->get_storage(STORAGE_INVENTORY);

        for (std::vector<Item::extended_type>::const_iterator j= item_list.begin(); j != item_list.end(); ++j)
        {
            try
            {
                new_item = world->GetItemFactory()->Create(j->id,*j);
                il->InsertItem(new_item);
            }
            catch (Exception::InvalidItemID &error)
            {
                syslog(LOG_INFO,"Lobby::LoadCharacters() - Error while cloning item with ID = %i",j->id);
            }
        }

        item_list = iobj(m_server->DBConnection(),m_player_list[i]->ID(),STORAGE_AVATAR);

        boost::shared_ptr<Storage> al = m_player_list[i]->get_storage(STORAGE_AVATAR);

        for (std::vector<Item::extended_type>::const_iterator j= item_list.begin(); j != item_list.end(); ++j)
        {
            try
            {
                new_item = world->GetItemFactory()->Create(j->id,*j);
                al->InsertItem(new_item);
            }
            catch (Exception::InvalidItemID &error)
            {
                syslog(LOG_INFO,"Lobby::LoadCharacters() - Error while cloning item with ID = %i",j->id);
            }
        }

        m_player_list[i]->RecalculateStats();
    }
}

StateLobby::player_iterator StateLobby::FindCharacterIndex(const std::string& name)
{
    player_iterator i;
    for (i = m_player_list.begin(); i < m_player_list.end(); ++i)
    {
        if ((*i)->get_name() == name)
            break;
    }

    return i;
}

bool StateLobby::IsNameValid (const std::string &name)
{
    ///CHECK THE NAME LENGTH DONT EXCEEDS THE MAX
    if (name.length() > MAX_PLAYER_NAME_LENTH)
        return false;

    boost::shared_ptr<OPacket> pkt(new OPacket);

    ///CHECK FOR INVALID CHARACTER
    if (name.find_first_of(m_ch_filter) != std::string::npos)
    {
        srv_pkt::Lobby(pkt,srv_pkt::LOBBY_ACTION_VALIDATE_NAME,srv_pkt::LOBBY_ERROR_NAME_EXIST);
        m_connection->Send(pkt);
        return false;
    }

    std::vector<std::string>::const_iterator i;
    for ( i = m_substr_filter_list.begin(); i != m_substr_filter_list.end(); ++i)
    {
        if (name.find(*i) != std::string::npos)
        {
            srv_pkt::Lobby(pkt,srv_pkt::LOBBY_ACTION_VALIDATE_NAME,srv_pkt::LOBBY_ERROR_NAME_EXIST);
            m_connection->Send(pkt);
            return false;
        }
    }

    ///CHECK THAT THE NAME ISNT ALREDY IN USE
    DB::PLAYER::Check check;

    if (!check(m_server->DBConnection(),name))
    {
        srv_pkt::Lobby(pkt,srv_pkt::LOBBY_ACTION_VALIDATE_NAME,srv_pkt::LOBBY_ERROR_NAME_EXIST);
        m_connection->Send(pkt);
        return false;
    }

    return true;
}
