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

#include "connection.hpp"
#include "server.hpp"
#include "state_handshake.hpp"
#include "state_agent.hpp"
#include "state_gateway.hpp"
#include "state_shard.hpp"
#include "state_download.hpp"
#include "state_emblem.hpp"

#include <iostream>

Connection::Connection (const uint32_t ID, boost::asio::io_service &io_service, Server *srv)
    :   IConnection(ID,io_service,srv)
{
    m_State.reset(new StateHandshake(this));

    boost::shared_ptr<srv::ConnectionState> st;

    ///REGISTER ADDITIONAL STATES FOR THE CONNECTION

    st.reset(new StateAgent(this,srv));

    m_state_table.insert(std::make_pair(STATE_AGENT_GM,st));

    st.reset(new StateGateway(this,srv));

    m_state_table.insert(std::make_pair(STATE_GATEWAY,st));

    st.reset(new StateDownload(this,srv));

    m_state_table.insert(std::make_pair(STATE_DOWNLOAD,st));

    st.reset(new StateShard(this,srv));

    m_state_table.insert(std::make_pair(STATE_SHARD,st));

    st.reset(new StateEmblem(this,srv));

    m_state_table.insert(std::make_pair(STATE_EMBLEM,st));
}

Connection::~Connection ()
{
}

bool Connection::IsGateway () const
{
    return m_current_state == STATE_GATEWAY;
}

bool Connection::IsShard () const
{
    return m_current_state == STATE_SHARD;
}

bool Connection::IsEmblem () const
{
    return m_current_state == STATE_EMBLEM;
}

bool Connection::IsAgent () const
{
    return m_current_state == STATE_AGENT_GM;
}

bool Connection::IsDownload () const
{
    return m_current_state == STATE_DOWNLOAD;
}
