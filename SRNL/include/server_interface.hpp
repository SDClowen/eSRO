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

#ifndef __SERVER_INTERFACE_HPP__
#define __SERVER_INTERFACE_HPP__

/// GLOBAL INCLUDES
#include <map>
#include <string>
#include <stdint.h>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>

class OPacket;

namespace srv
{

class IConnection;

class IServer
{
public:

    typedef std::map<uint32_t,boost::shared_ptr<srv::IConnection> >::iterator conn_iterator;
    typedef std::map<uint32_t,boost::shared_ptr<srv::IConnection> >::const_iterator conn_const_iterator;

    IServer ();

    virtual ~IServer ();

    bool Initialize (const std::map<std::string,std::string> &config_entries);

    bool Execute (bool run_service = true);

    void Stop ();

    const std::string& GetLastError () const;

    bool IsEncryptionActive () const;

    uint32_t ConnectionTimeout () const;

    uint32_t ClientVersion () const;

    uint8_t Locale () const;

    void RemoveConnection (const uint32_t connID);

    const std::string& IDS () const;

    void Send (const boost::shared_ptr<OPacket> &p, const uint32_t ID);

    void Send (const boost::shared_ptr<OPacket> &p1, const boost::shared_ptr<OPacket> &p2,
        const boost::shared_ptr<OPacket> &p3, const uint32_t ID);

    boost::shared_ptr<srv::IConnection> FindConnection (const uint32_t connectionID) const;

protected:

    virtual bool OnInitialize () = 0;

    virtual void OnConfigure (const std::map<std::string,std::string> &config_entries) = 0;

    virtual void OnRemoveConnection (const uint32_t ID) = 0;

    virtual void CreateConnection () = 0;

    void OnStop ();

    void OnAccept (const boost::system::error_code &error);

protected:

    uint32_t m_port;
    uint32_t m_counter;
    std::string m_last_error;

    boost::asio::io_service m_io_service;
    boost::asio::ip::tcp::acceptor m_acceptor;

    boost::shared_ptr<srv::IConnection> m_pending_conn;
    std::map<uint32_t,boost::shared_ptr<srv::IConnection> > m_conn_list;

    uint8_t m_locale;
    uint32_t m_client_version;
    bool m_is_encryption_active;
    uint32_t m_connection_timeout;
    uint32_t m_connection_max_count;

    std::string m_IDS;
    std::string m_LogPath;

    mutable boost::mutex m_conn_mutex;
};

}

#endif
