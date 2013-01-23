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

#include "server_interface.hpp"
#include "server_connection_interface.hpp"

#include <boost/bind.hpp>

namespace srv
{

IServer::IServer ()
    :   m_port(0),
        m_counter(0),
        m_acceptor(m_io_service),
        m_locale(0),
        m_client_version(0),
        m_is_encryption_active(false),
        m_connection_timeout(5),
        m_connection_max_count(10)
{
}

IServer::~IServer ()
{
}

bool IServer::IsEncryptionActive () const
{
    return m_is_encryption_active;
}

uint32_t IServer::ConnectionTimeout () const
{
    return m_connection_timeout;
}

uint32_t IServer::ClientVersion () const
{
    return m_client_version;
}

uint8_t IServer::Locale() const
{
    return m_locale;
}

void IServer::RemoveConnection (const uint32_t connID)
{
    boost::mutex::scoped_lock lock(m_conn_mutex);

    OnRemoveConnection(connID);

    m_conn_list.erase(connID);
}

const std::string& IServer::IDS () const
{
    return m_IDS;
}

bool IServer::Initialize (const std::map<std::string,std::string> &config_entries)
{
    OnConfigure(config_entries);

    try
    {
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(),m_port);

        m_acceptor.open(endpoint.protocol());
        m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        m_acceptor.bind(endpoint);

        if(!OnInitialize())
            return false;
    }
    catch (const boost::system::system_error &error)
    {
        m_last_error = error.what();
        return false;
    }
    catch (...)
    {
        m_last_error = "Exception raised. Unhandled error.";
        return false;
    }

    return true;
}

bool IServer::Execute (bool run_service)
{
    m_acceptor.listen();

    CreateConnection();

	m_acceptor.async_accept(m_pending_conn->socket(),boost::bind(&IServer::OnAccept,this,
        boost::asio::placeholders::error));

    if (run_service)
        m_io_service.run();

    return true;
}

void IServer::Stop ()
{
    m_io_service.post(boost::bind(&IServer::OnStop,this));
}

const std::string& IServer::GetLastError () const
{
    return m_last_error;
}

void IServer::Send (const boost::shared_ptr<OPacket> &pkt, const uint32_t ID)
{
    boost::mutex::scoped_lock lock(m_conn_mutex);

    conn_iterator iter = m_conn_list.find(ID);

    if (iter != m_conn_list.end())
        iter->second->Send(pkt);
}

void IServer::Send (const boost::shared_ptr<OPacket> &p1, const boost::shared_ptr<OPacket> &p2,
    const boost::shared_ptr<OPacket> &p3, const uint32_t ID)
{
    boost::mutex::scoped_lock lock(m_conn_mutex);

    conn_iterator iter = m_conn_list.find(ID);

    if (iter != m_conn_list.end())
        iter->second->Send(p1,p2,p3);
}

boost::shared_ptr<srv::IConnection> IServer::FindConnection (const uint32_t connectionID) const
{
    boost::shared_ptr<srv::IConnection> conn;

    boost::mutex::scoped_lock lock(m_conn_mutex);

    conn_const_iterator iter = m_conn_list.find(connectionID);

    if (iter != m_conn_list.end())
        conn = iter->second;

    return conn;
}

void IServer::OnAccept (const boost::system::error_code &error)
{
    if (!error)
    {
        boost::mutex::scoped_lock lock(m_conn_mutex);

        m_conn_list.insert(std::make_pair(m_counter,m_pending_conn));

		m_pending_conn->Execute();

		CreateConnection();

		m_acceptor.async_accept(m_pending_conn->socket(), boost::bind(&IServer::OnAccept,this,
            boost::asio::placeholders::error));
    }
}

void IServer::OnStop ()
{
    boost::mutex::scoped_lock lock(m_conn_mutex);

    m_acceptor.close();

    for (conn_iterator it = m_conn_list.begin(); it != m_conn_list.end(); ++it)
        it->second->Stop();

    m_conn_list.clear();

    m_io_service.stop();
}

}
