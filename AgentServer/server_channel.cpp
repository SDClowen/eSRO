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

#include "server_channel.hpp"
#include "server_connection.hpp"

Channel::Channel (const int32_t channelID)
    : m_ID(channelID)
{
}

void Channel::Send (const boost::shared_ptr<OPacket> &pkt, bool priv)
{
    boost::mutex::scoped_lock lock(m_mutex);

    boost::shared_ptr<srv::Connection> conn;
    for (connection_iterator it = m_conn_list.begin(); it != m_conn_list.end(); ++it)
    {
        if (priv)
        {
            conn = boost::shared_dynamic_cast<srv::Connection,srv::IConnection>(it->second);

            if (conn->AreaID() != m_ID)
                continue;
        }

        it->second->Send(pkt,false,false);
    }
}

void Channel::Send (const boost::shared_ptr<OPacket> &p1, const boost::shared_ptr<OPacket> &p2,
        const boost::shared_ptr<OPacket> &p3, bool priv)
{
    boost::mutex::scoped_lock lock(m_mutex);

    boost::shared_ptr<srv::Connection> conn;
    for (connection_iterator it = m_conn_list.begin(); it != m_conn_list.end(); ++it)
    {
        if (priv)
        {
            conn = boost::shared_dynamic_cast<srv::Connection,srv::IConnection>(it->second);

            if (conn->AreaID() != m_ID)
                continue;
        }

        it->second->Send(p1,p2,p3,false,false);
    }
}

void Channel::InsertConnection (const boost::shared_ptr<srv::IConnection> &conn)
{
    boost::mutex::scoped_lock lock(m_mutex);

    m_conn_list.insert(std::make_pair(conn->ID(),conn));
}

void Channel::RemoveConnection (const uint32_t connectionID)
{
    boost::mutex::scoped_lock lock(m_mutex);

    m_conn_list.erase(connectionID);
}

size_t Channel::size () const
{
    boost::mutex::scoped_lock lock(m_mutex);

    return m_conn_list.size();
}
