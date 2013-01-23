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

#include "srvlistclient.h"
#include "networkthread.h"
#include "client_state_handshake.hpp"
#include "client_state_version.hpp"
#include "client_state_login.hpp"

SrvListClient::SrvListClient(boost::asio::io_service &io_service, const HostDivisionInfo *info, networkthread *thread)
        : clt::IConnection(io_service,info->hosts[0],info->port), m_thread(thread)
{
    m_version = info->version;
    m_locale = info->locale;
    m_gw_list = info->hosts;

    ///REGISTER AVAILABLE STATES
    m_State.reset(new StateLogin(this));
    m_state_table.insert(std::make_pair(STATE_LOGIN,m_State));

    m_State.reset(new StateVersion("SR_Client",this));
    m_state_table.insert(std::make_pair(STATE_VERSION,m_State));

    ///SET INITIAL STATE
    m_State.reset(new StateHandshake("GatewayServer","SR_Client",this));
}

SrvListClient::~SrvListClient()
{
}

void SrvListClient::SetShardList(const std::vector<Shard> &list)
{
    m_shard_list = list;
    emit m_thread->OnShardList();
}

const std::vector<Shard>& SrvListClient::ShardList () const
{
    return m_shard_list;
}
