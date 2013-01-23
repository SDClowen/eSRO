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

#include "networkthread.h"
#include "srvlistclient.h"
#include <iostream>
networkthread::networkthread(const HostDivisionInfo *info, QObject *parent)
        : QThread(parent), m_client(new SrvListClient(m_io_service,info,this))
{
}

networkthread::~networkthread()
{
    if (m_client)
    {
        m_client->Stop();
        delete m_client;
    }

    m_io_service.stop();
}

void networkthread::run()
{   
    try
    {
        m_client->Execute();
        m_io_service.run();
    }
    catch (std::exception &error)
    {
        std::cout << error.what() << std::endl;
    }
}

void networkthread::close_service ()
{
    m_io_service.stop();
}

 const std::vector<Shard>& networkthread::ShardList () const
 {
     return m_client->ShardList();
 }
