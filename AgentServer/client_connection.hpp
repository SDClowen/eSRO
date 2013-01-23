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

#ifndef __ASSASIN_CLIENT_CONNECTION_HPP__
#define __ASSASIN_CLIENT_CONNECTION_HPP__

#include <client_connection_interface.hpp>

class Server;

namespace clt
{

enum CONNECTION_STATES
{
    STATE_HANDSHAKE,
    STATE_MASTER
};

class Connection : public clt::IConnection
{
public:

    Connection (boost::asio::io_service &io_service, const std::string &host, const std::string &port, Server *srv);

    virtual ~Connection ();
};

}

#endif  //__ASSASIN_CLIENT_CONNECTION_HPP__
