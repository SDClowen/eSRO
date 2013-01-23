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

#ifndef __SRV_CONNECTION_STATE_HANDSHAKE_HPP__
#define __SRV_CONNECTION_STATE_HANDSHAKE_HPP__

#include <server_state_handshake_interface.hpp>

class Connection;

class StateHandshake : public IStateHandshake
{
public:

    StateHandshake (Connection *conn);

    virtual ~StateHandshake ();

    virtual bool OnClientID (const std::string &ID);

private:

    std::map<std::string,uint8_t> m_state_table;
};

#endif //__SRV_CONNECTION_STATE_HANDSHAKE_HPP__
