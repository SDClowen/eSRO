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

#include "character_factory.hpp"

/// SRLF INCLUDES
#include <mob.hpp>
#include <player.hpp>
#include <transport.hpp>
#include <interactive_npc.hpp>
#include <entity_property.hpp>

CharacterFactory::CharacterFactory (const property_map &property_list)
    : m_property_list(property_list)
{
}

CharacterFactory::CharacterFactory (const property_map &property_list, const quality_map &quality_chart)
    : m_quality_chart(quality_chart), m_property_list(property_list)
{
}

boost::shared_ptr<NPC> CharacterFactory::Create (const uint32_t ID) const
{
    boost::mutex::scoped_lock lock(m_mutex);

    boost::shared_ptr<NPC> obj;
    property_map::const_iterator iter = m_property_list.find(ID);

    if ( iter != m_property_list.end())
    {
        int t4 = iter->second->type_list[3];
        int t5 = iter->second->type_list[4];
        int t6 = iter->second->type_list[5];

        if (t4 == 1 && t5 == 0 && t6 == 0)
        {
            obj = boost::shared_ptr<NPC>(new Player(iter->second));
        }
        else if (t4 == 2)
        {
            if (t5 == 1)
            {
                switch(t6)
                {
                case 1: ///MOBS
                case 2: ///THIEF MOBS
                case 3: ///HUNTER MOBS
                case 4: ///QUEST MOBS
                case 5: ///PANDORA MOBS OR EVENT
                    {
                        std::pair<quality_map::const_iterator,quality_map::const_iterator> iter_range;
                        iter_range = m_quality_chart.equal_range(ID);

                        int prob = rand()%100;
                        uint8_t quality = iter->second->Quality;

                        for (quality_map::const_iterator niter = iter_range.first; niter != iter_range.second; ++niter)
                        {
                            if (prob < niter->second.second)
                            {
                                quality = niter->second.first;
                                break;
                            }

                            prob -= niter->second.second;
                        }

                        obj = boost::shared_ptr<NPC>(new Mob(iter->second,quality));
                    }
                    break;
                default:
                    break;
                }
            }
            else if ( t5 == 2)
            {
                switch(t6)
                {
                case 0:
                case 1: ///NPCS
                    obj = boost::shared_ptr<NPC>(new InteractiveNPC(iter->second));
                    break;
                case 2: ///GATE PULLEY
                    obj = boost::shared_ptr<NPC>(new InteractiveNPC(iter->second));
                    break;
                default:
                    break;
                }
            }
            else if (t5 == 3)
            {
                switch(t6)
                {
                case 1: ///NORMAL TRANSPORT
                case 2: ///JOB TRANSPORT
                    obj = boost::shared_ptr<NPC>(new Transport(iter->second));
                    break;
                case 3: ///GROW PET
                    break;
                case 4: ///ABILITY PET
                    break;
                case 5: ///MERCENARIES
                    break;
                case 6: ///CAPTURED MOBS
                    break;
                case 7: ///NPC_CH_QT_MOONSHADOW_COS
                case 8: ///NPC_CH_QT_FLAMEMASTER_COS
                    break;
                default:
                    break;
                }
            }
            else
            {
            }

        }
	}

	return obj;
}

boost::shared_ptr<NPC> CharacterFactory::Create (const uint32_t refCharID, uint8_t quality) const
{
    boost::mutex::scoped_lock lock(m_mutex);

    boost::shared_ptr<NPC> npc;

    property_map::const_iterator iter = m_property_list.find(refCharID);

    if ( iter != m_property_list.end())
    {
        npc = boost::shared_ptr<Mob>(new Mob(iter->second,quality));
    }

    return npc;
}

boost::shared_ptr<Player> CharacterFactory::Create (const PlayerProperty &property) const
{
    boost::mutex::scoped_lock lock(m_mutex);

    boost::shared_ptr<Player> player;

    property_map::const_iterator i = m_property_list.find(property.RefCharID);

    if (i != m_property_list.end())
        player = boost::shared_ptr<Player>(new Player(i->second,property));

    return player;
}

boost::shared_ptr<Player> CharacterFactory::Create (const uint32_t ID, const uint8_t volume,
    const std::string &name, const std::vector<Item> &items) const
{
    boost::mutex::scoped_lock lock(m_mutex);

    boost::shared_ptr<Player> player;

    property_map::const_iterator i = m_property_list.find(ID);

    if (i != m_property_list.end())
        player = boost::shared_ptr<Player>(new Player(i->second,items,name,volume));

    return player;
}

boost::shared_ptr<EntityProperty> CharacterFactory::GetProperty (const uint32_t refCharID) const
{
    boost::shared_ptr<EntityProperty> pr;

    property_map::const_iterator i = m_property_list.find(refCharID);

    if (i != m_property_list.end())
        pr = i->second;

    return pr;
}
