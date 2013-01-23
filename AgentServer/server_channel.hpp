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

#ifndef SERVER_CHANNEL_HPP
#define SERVER_CHANNEL_HPP

#include <stdint.h>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

class OPacket;

namespace srv
{
    class IConnection;
}

class Channel
{
public:

    typedef std::map<uint32_t,boost::shared_ptr<srv::IConnection> >::iterator connection_iterator;
    typedef std::map<uint32_t,boost::shared_ptr<srv::IConnection> >::const_iterator connection_const_iterator;

    Channel (const int32_t channelID);

    void Send (const boost::shared_ptr<OPacket> &pkt, bool priv);

    void Send (const boost::shared_ptr<OPacket> &p1, const boost::shared_ptr<OPacket> &p2,
        const boost::shared_ptr<OPacket> &p3, bool priv);

    void InsertConnection (const boost::shared_ptr<srv::IConnection> &conn);

    void RemoveConnection (const uint32_t connectionID);

    size_t size () const;

private:

    int32_t m_ID;

    mutable boost::mutex m_mutex;
    std::map<uint32_t,boost::shared_ptr<srv::IConnection> > m_conn_list;
};

#endif // SERVER_CHANNEL_HPP
