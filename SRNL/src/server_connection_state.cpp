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

#include "server_connection_state.hpp"

namespace srv
{

ConnectionState::ConnectionState (IConnection *conn)
	: m_connection(conn)
{
}

ConnectionState::~ConnectionState ()
{
}

int ConnectionState::Parse (IPacket &packet)
{
    opcode_iterator iter = m_opcode_table.find(packet.opcode());

	if (iter == m_opcode_table.end())
    {
        syslog(LOG_INFO,"srv::ConnectionState::Parse - Unknown opcode %X",packet.opcode());
        return MSG_ERROR_OPCODE;
    }

    int ret = MSG_SUCCESS;

    try
    {
        opcode_handler function = iter->second;

        ret = function(packet);
    }
    catch (const Exception::InvalidPacket &error)
    {
        ret = MSG_ERROR_ORDER;
    }
    catch (const Exception::InvalidPacketSize &error)
    {
        ret = MSG_ERROR_SIZE;
    }
    catch (const Exception::InvalidPacketArg &error)
    {
        ret = MSG_ERROR_ARG;
    }
    catch(const std::exception& e)
    {
        syslog(LOG_INFO,"srv::ConnectionState::Parse - Exception: %s",e.what());
        ret = MSG_SUCCESS;
    }
    catch (...)
    {
        syslog(LOG_INFO,"srv::ConnectionState::Parse - Unhandled error.");
        ret = MSG_ERROR;
    }

    return ret;
}

}
