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

#ifndef NETWORKTHREAD_H
#define NETWORKTHREAD_H

#include <shard.hpp>

#include <QThread>
#include <boost/asio.hpp>

class SrvListClient;
class HostDivisionInfo;

class networkthread : public QThread
{
    Q_OBJECT

public:

    friend class SrvListClient;

    networkthread(const HostDivisionInfo *info, QObject *parent = 0);
    virtual ~networkthread();

    virtual void run();

    void close_service ();

    const std::vector<Shard>& ShardList () const;

private:

    boost::asio::io_service m_io_service;
    SrvListClient *m_client;

signals:
    void OnShardList ();
};

#endif // NETWORKTHREAD_H
