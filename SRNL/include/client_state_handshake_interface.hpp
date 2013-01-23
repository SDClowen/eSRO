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

#ifndef __SRNL_STATE_HANDSHAKE_INTERFACE_HPP__
#define __SRNL_STATE_HANDSHAKE_INTERFACE_HPP__

#include "client_connection_state.hpp"

namespace clt
{

class IStateHandshake : public clt::ConnectionState
{
public:

    IStateHandshake (const std::string &srvID, const std::string &cltID, clt::IConnection *conn);

    virtual ~IStateHandshake ();

    virtual void Close ();

    void SendID ();

protected:

    virtual void OnConfig () = 0;

protected:

    int OnHandshake (IPacket &packet);

    int OnHandshake_Clean (IPacket &packet);

    int OnHandshake_Key (IPacket &packet);

    int OnHandshake_Signature (IPacket &packet);

    int OnWhoIs (IPacket &packet);

protected:

    std::string m_SrvID;
    std::string m_CltID;

    uint32_t m_S;
    uint32_t m_base_key[2];
    uint32_t m_srv_signature[2];
};

}

#endif //__SRNL_STATE_HANDSHAKE_INTERFACE_HPP__
