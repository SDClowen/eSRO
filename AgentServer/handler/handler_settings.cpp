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

#include "player.hpp"

/// SRDL INCLUDES
#include <hotkeys_query.hpp>
#include <player_query.hpp>

int StateGame::OnSetting (IPacket &packet)
{
    enum
    {
        SETTING_HOTKEY = 1,
        SETTING_AUTOPOT
    };

    uint8_t type = packet.Read<uint8_t>();

    uint8_t ret;

    switch(type)
    {
    case SETTING_HOTKEY:
        ret = OnSetHotkey(packet);
        break;
    case SETTING_AUTOPOT:
        ret = OnSetAutopot(packet);
        break;
    default:
        ret = MSG_ERROR;
        break;
    }

    return ret;
}

int StateGame::OnSetHotkey (IPacket &packet)
{
    Hotkey hotkey;

    hotkey.pos = packet.Read<uint8_t>();

    hotkey.type = packet.Read<uint8_t>();

    hotkey.id = packet.Read<uint32_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    if (hotkey.type)
    {
        player->m_Hotkeys.push_back(hotkey);

        DB::HOTKEY::Insert query;
        query(m_server->DBConnection(),player->ID(),hotkey);
    }
    else
    {
        for (std::vector<Hotkey>::iterator i = player->m_Hotkeys.begin(); i != player->m_Hotkeys.end(); ++i)
        {
            if (i->pos == hotkey.pos)
            {
                player->m_Hotkeys.erase(i);
                break;
            }
        }

        DB::HOTKEY::Remove query;
        query(m_server->DBConnection(),player->ID(),hotkey.pos);
    }

    return MSG_SUCCESS;
}

int StateGame::OnSetAutopot (IPacket &packet)
{
    uint8_t hp_bar_slot = packet.Read<uint8_t>();
    uint8_t hp_active_value = packet.Read<uint8_t>();

    uint8_t mp_bar_slot = packet.Read<uint8_t>();
    uint8_t mp_active_value = packet.Read<uint8_t>();

    uint8_t pill_bar_slot = packet.Read<uint8_t>();
    uint8_t pill_active = packet.Read<uint8_t>();

    uint8_t delay_active_value = packet.Read<uint8_t>();

    if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

    boost::shared_ptr<Player> player = static_cast<srv::Connection*>(m_connection)->GetPlayer();

    if (!player)
        return MSG_ERROR;

    player->m_Autopot.hp_bar = (hp_bar_slot & 0xF0) >> 4;
    player->m_Autopot.hp_slot = (hp_bar_slot & 0x0F);
    player->m_Autopot.hp_active = (hp_active_value & 0x80) >> 7;
    player->m_Autopot.hp_value = (hp_active_value) & 0x7F;
    player->m_Autopot.mp_bar = (mp_bar_slot & 0xF0) >> 4;
    player->m_Autopot.mp_slot = (mp_bar_slot & 0x0F);
    player->m_Autopot.mp_active = (mp_active_value & 0x80) >> 7;
    player->m_Autopot.mp_value = (mp_active_value) & 0x7F;
    player->m_Autopot.pill_bar = (pill_bar_slot & 0xF0) >> 4;
    player->m_Autopot.pill_slot = (pill_bar_slot & 0x0F);
    player->m_Autopot.pill_active = pill_active >>= 7;
    player->m_Autopot.delay_active = (delay_active_value & 0x80) >> 7;
    player->m_Autopot.delay_value = (delay_active_value) & 0x7F;

    DB::PLAYER::Autopot query;
    query(m_server->DBConnection(),player->ID(),player->m_Autopot);

    return MSG_SUCCESS;
}
