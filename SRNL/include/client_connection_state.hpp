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

#ifndef __CLIENT_CONNECTION_STATE_HPP__
#define __CLIENT_CONNECTION_STATE_HPP__

//#include <syslog.h>

#include <packet.hpp>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <syslog.h>

enum MSG_PARSE_RESULT
{
    MSG_SUCCESS,
    MSG_ERROR,
    MSG_ERROR_MEMORY,
    MSG_ERROR_OPCODE,
    MSG_ERROR_SIZE,
    MSG_ERROR_ARG,
    MSG_ERROR_ORDER
};

namespace clt
{

class IConnection;

class ConnectionState
{
public:

	typedef boost::function<int (IPacket&)> opcode_handler;
    typedef std::map<uint32_t,opcode_handler>::iterator opcode_iterator;
    typedef std::map<uint32_t,opcode_handler>::const_iterator opcode_const_iterator;

    ConnectionState (IConnection *conn);

	virtual ~ConnectionState ();

    int Parse (IPacket &packet);

    virtual void Close () = 0;

protected:

    IConnection *m_connection;
	std::map<uint32_t,opcode_handler> m_opcode_table;
};

}

#endif //__CLIENT_CONNECTION_STATE_HPP__

