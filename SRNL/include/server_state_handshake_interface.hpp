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

#ifndef __SRV_CONNECTION_STATE_HANDSHAKE_INTERFACE_HPP__
#define __SRV_CONNECTION_STATE_HANDSHAKE_INTERFACE_HPP__

#include "server_connection_state.hpp"

///HANDSHAKE PROCESS AND VALIDATION

class IStateHandshake : public srv::ConnectionState
{
public:

    IStateHandshake (srv::IConnection *conn);

    virtual ~IStateHandshake ();

    virtual void Close ();

    void SetBaseKey (const uint32_t key[2]);

    void SetSharedData (uint32_t a, uint32_t P, uint32_t G, uint32_t A);

protected:

    int OnHandshake (IPacket &packet);

    int OnHandshakeValidated (IPacket &packet);

    int OnWhoIs (IPacket &packet);

    virtual bool OnClientID (const std::string &ID) = 0;

private:

    bool m_authenticated;

    uint32_t m_a;   ///PRIVATE EXPONENT
    uint32_t m_p;   ///PUBLIC PRIME
    uint32_t m_g;   ///PUBLIC BASE
    uint32_t m_A;   ///CACHED VALUE FOR MODEXP(P,a,G)

    uint32_t m_base_key[2];
};

#endif  //__SRV_CONNECTION_STATE_HANDSHAKE_INTERFACE_HPP__
