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

#ifndef __ASSASIN_BT_STATE_VERSION_HPP__
#define __ASSASIN_BT_STATE_VERSION_HPP__

#include <client_connection_state.hpp>

class SrvListClient;

class StateVersion : public clt::ConnectionState
{
public:

    StateVersion (const std::string &clientID, SrvListClient *conn);

    virtual ~StateVersion ();

    virtual void Close ();

protected:

    int OnVersionCheck (IPacket &packet);

    int OnVersionCheckA (IPacket &packet);

    int OnVersionCheckB (IPacket &packet);

    int OnVersionCheckC (IPacket &packet);

    int OnVersionCheckD (IPacket &packet);

    int OnVersionCheckE (IPacket &packet);

    int OnVersionCheckF (IPacket &packet);

private:

    int m_count;
    std::string m_clientID;
};

#endif
