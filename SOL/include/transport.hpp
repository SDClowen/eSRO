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

#ifndef __SRLF_TRANSPORT_HPP__
#define __SRLF_TRANSPORT_HPP__

#include "npc.hpp"

class Player;
class Storage;

class Transport : public NPC
{
public:

    Transport (const boost::shared_ptr<EntityProperty> &property);

    void SetOwner (const boost::shared_ptr<Player> &owner);

    const boost::shared_ptr<Player>& GetOwner () const;

    virtual uint8_t level () const;

    const boost::shared_ptr<Storage>& GetStorage () const;

    bool has_goods () const;

    bool IsMounted () const;

    void Mount ();

    void Dismount ();

    uint32_t SpawnTaskID () const;

    void SpawnTaskID (const uint32_t taskID);

    uint8_t TradeScale () const;

    void CalculateTradeScale ();

private:

    virtual void calculate_stats ();

private:

    bool m_Mounted;
    boost::shared_ptr<Player> m_Owner;       ///TRANSPORT OWNER
    boost::shared_ptr<Storage> m_Storage;   ///ONLY JOB TRANSPORTS HAVE THIS!!

    uint8_t m_TradeScale;
    uint32_t m_SpawnTaskID;
};

#endif
