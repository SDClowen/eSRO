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

#ifndef __GATEWAY_SERVER_STATE_VERSION_HPP__
#define __GATEWAY_SERVER_STATE_VERSION_HPP__

/// SRNL INCLUDES
#include <server_connection_state.hpp>

namespace srv
{
    class Connection;
}

class StateVersion : public srv::ConnectionState
{
public:

    StateVersion (const std::string &clientID, const uint8_t globalVersion, srv::Connection *conn);

    virtual ~StateVersion ();

    virtual void Close ();

protected:

    int OnVersionInfo (IPacket &packet);

private:

    std::string m_client_id;
    uint8_t m_global_version;
};

#endif
