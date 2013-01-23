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

#include "serverwindow.h"
#include "networkthread.h"
#include "ui_serverwindow.h"

///BLOWFISH INCLUDES
#include <blowfish.hpp>

///SRNL INCLUDES
#include <host_info.hpp>

///PKFM INCLUDES
#include <pk2Reader.h>

#include <QTextCodec>
#include <iostream>

ServerWindow::ServerWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::ServerWindow), m_codec(QTextCodec::codecForName("UTF-16"))
{
    ui->setupUi(this);

    m_reader = new IPK2;
    m_hostInfo = new HostInfo;
    m_hostDiv = new HostDivisionInfo;

    m_filename = "/home/npcdoom/.wine/drive_c/Program Files/Silkroad/Media.pk2";
}

ServerWindow::~ServerWindow()
{
    if (m_netThread->isRunning())
    {
        m_netThread->close_service();
        m_netThread->quit();
    }

    delete m_reader;
    delete m_hostInfo;
    delete m_hostDiv;
    delete m_netThread;
    delete ui;
}

bool ServerWindow::ReadMediaInfo()
{
    if (!m_reader->initialize())
    {
        std::cerr << m_reader->get_last_error() << std::endl;
        return 1;
    }

    if (!m_reader->open(m_filename.c_str()))
    {
        std::cerr << m_reader->get_last_error() << std::endl;
        return 1;
    }

    ///READ DIVISIONINFO.TXT FILE FOR AVAILABLE GATEWAYS
    IPK2::const_iterator i = m_reader->find("DIVISIONINFO.TXT");

    if (i == m_reader->end())
    {
        std::cerr << "Couldnt find DIVISIONINFO.TXT check your Media.pk2 file might be corrupted." << std::endl;
        return 1;
    }

    std::ifstream file(m_filename.c_str(),std::ios::in);

    file.seekg(i->second->position,std::ios::beg);

    uint8_t div_count = 0;
    file.read((char*)&m_hostInfo->locale,sizeof(m_hostInfo->locale));
    file.read((char*)&div_count,sizeof(div_count));

    char div[1024];
    char gw[1024];
    uint8_t gw_count = 0;
    uint32_t len = 0;

    DivisionInfo divinfo;
    for (int i = 0; i < div_count; ++i)
    {
        memset(div,0,sizeof(div));

        file.read((char*)&len,sizeof(len));

        if (len > 1024)
            return 1;

        file.read(div,len+1);

        divinfo.name = std::string(div,div+len);
        file.read((char*)&gw_count,sizeof(gw_count));

        for (int j = 0; j < gw_count; ++j)
        {
            memset(gw,0,sizeof(gw));

            file.read((char*)&len,sizeof(len));

            if (len > 1024)
                return false;

            file.read(gw,len+1);

            divinfo.gws.push_back(std::string(gw,gw+len));
        }

        m_hostInfo->divs.push_back(divinfo);

        divinfo.gws.clear();
    }

    ///READ GATEPORT.TXT FOR GATEWAY LISTENING PORT
    i = m_reader->find("GATEPORT.TXT");

    if (i == m_reader->end())
    {
        std::cerr << "Couldnt find GATEPORT.TXT check your Media.pk2 file might be corrupted." << std::endl;
        return false;
    }

    if (i->second->size < 8)
    {
        std::cerr << "Corrupted GATEPORT.TXT file check your Media.pk2" << std::endl;
        return false;
    }

    file.seekg(i->second->position,std::ios::beg);

    char port[8] = { 0 };

    file.read(port,8);

    m_hostInfo->port= std::string(port,port+5);

    ///READ SV.T FOR CLIENT VERSION
    i = m_reader->find("SV.T");

    if (i == m_reader->end())
    {
        std::cerr << "Couldnt find SV.T check your Media.pk2 file might be corrupted." << std::endl;
        return false;
    }

    uint32_t size = 0;
    file.seekg(i->second->position,std::ios::beg);

    file.read((char*)&size,sizeof(size));

    if (!size)
    {
        std::cerr << "Corrupted SV.T file" << std::endl;
        return false;
    }

    char *buffer = new char[size];
    file.read(buffer,size);

    cBlowFish svt_bf;
    svt_bf.Initialize((uint8_t*)"SILKROADVERSION",8);   ///ONLY USE AS KEY SILKROAD
    svt_bf.Decode((uint8_t*)buffer,(uint8_t*)buffer,size);

    m_hostInfo->version = atoi(buffer);
    delete buffer;
    buffer = 0;

    m_hostDiv->locale = m_hostInfo->locale;
    m_hostDiv->version = m_hostInfo->version;
    m_hostDiv->port = m_hostInfo->port;

    for (auto it = m_hostInfo->divs.begin(); it != m_hostInfo->divs.end(); ++it)
        ui->cbx_div->addItem(QString(it->name.c_str()));

    m_netThread = new networkthread(m_hostDiv);

    connect(m_netThread,SIGNAL(OnShardList()),this,SLOT(on_shard_list()));

    m_netThread->start();

    return true;
}

void ServerWindow::on_action_Quit_triggered()
{
    m_netThread->close_service();
    m_netThread->exit();

    this->close();
}

void ServerWindow::on_cbx_div_currentIndexChanged(int index)
{
    m_hostDiv->div = m_hostInfo->divs[index].name;
    m_hostDiv->hosts = m_hostInfo->divs[index].gws;
}

void ServerWindow::on_shard_list ()
{
    std::vector<Shard> list = m_netThread->ShardList();

    QTreeWidgetItem *item;
    QStringList args;
    QList<QTreeWidgetItem*> items;

    for (auto it = list.begin(); it != list.end(); ++it)
    {
        args.append(m_codec->toUnicode(it->name.c_str(),it->name.length()));
        args.append(QString::number(it->usr_cnt));
        args.append(QString::number(it->usr_max));
        args.append(QString::number(it->state));

        item = new QTreeWidgetItem(args);
        items.append(item);

        args.clear();
    }

    ui->tree_srv_list->addTopLevelItems(items);
}
