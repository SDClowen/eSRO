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

#ifndef CLIENT_STATE_LOGIN_HPP
#define CLIENT_STATE_LOGIN_HPP

#include <client_connection_state.hpp>

class SrvListClient;

class StateLogin : public clt::ConnectionState
{
public:

    StateLogin (SrvListClient *conn);

    virtual ~StateLogin ();

    virtual void Close ();

private:

    int OnServerList (IPacket &packet);
};

#endif // CLIENT_STATE_LOGIN_HPP
