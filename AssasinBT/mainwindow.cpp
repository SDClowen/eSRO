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

#include "mainwindow.h"
#include "dlgpreference.h"
#include "ui_mainwindow.h"

/// BLOWFISH INCLUDES
#include <blowfish.hpp>

/// PKFM INCLUDES
#include <pk2Reader.h>

/// SRNL INCLUDES
#include "host_info.hpp"

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),m_connected(false),m_running(false)
{
    ui->setupUi(this);

    m_dlgpreference = new DlgPreference(this);

    m_reader = new IPK2;
    m_hostInfo = new HostInfo;

    ReadConfiguration();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_dlgpreference;
    delete m_reader;
    delete m_hostInfo;
}

bool MainWindow::ReadHostInfo()
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

//    std::cout << "Locale = " << (int)m_hostInfo->locale << std::endl;
//    std::cout << "Gateway Port = " << m_hostInfo->port << std::endl;
//    std::cout << "Version = " << m_hostInfo->version << std::endl;
//
//    for (auto dit = m_hostInfo->divs.begin(); dit != m_hostInfo->divs.end(); ++dit)
//    {
//        std::cout << "Division = " << dit->name << std::endl;
//
//        for (auto git = dit->gws.begin(); git != dit->gws.end(); ++git)
//            std::cout << "\tGateway = " << *git << std::endl;
//    }

    m_dlgpreference->SetHostInfo(*m_hostInfo);

    return true;
}

void MainWindow::ReadConfiguration()
{
    m_filename = "/home/npcdoom/.wine/drive_c/Program Files/Silkroad/Media.pk2";
}

void MainWindow::on_bt_server_clicked()
{
    if (!m_connected)
    {
        ui->bt_server->setText("Disconnect");
        ui->bt_service->setEnabled(true);
    }
    else
    {
        ui->bt_server->setText("Connect");

        if (m_running)
        {
            ui->bt_service->setText("Run");
            m_running = false;
        }

        ui->bt_service->setEnabled(false);
    }

    m_connected = !m_connected;
}

void MainWindow::on_bt_service_clicked()
{
    if (!m_running)
    {
        ui->bt_service->setText("Stop");
    }
    else
    {
        ui->bt_service->setText("Run");
    }

    m_running = !m_running;
}

void MainWindow::on_actionPreferences_triggered()
{
    m_dlgpreference->show();
}

void MainWindow::on_action_Quit_triggered()
{
    this->close();
}

void MainWindow::on_actionAbout_triggered()
{

}
