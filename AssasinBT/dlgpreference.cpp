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

#include "dlgpreference.h"
#include "ui_dlgpreference.h"
#include "srvlistclient.h"
#include <iostream>

DlgPreference::DlgPreference(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::DlgPreference)
{
    m_ui->setupUi(this);
}

DlgPreference::~DlgPreference()
{
    delete m_ui;
}

void DlgPreference::SetHostInfo(const HostInfo &info)
{
    m_hostInfo = info;
    m_hostDiv.locale = m_hostInfo.locale;
    m_hostDiv.version = m_hostInfo.version;
    m_hostDiv.port = m_hostInfo.port;

    for (auto it = m_hostInfo.divs.begin(); it != m_hostInfo.divs.end(); ++it)
        m_ui->cbx_div->addItem(QString(it->name.c_str()));
}

void DlgPreference::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void DlgPreference::on_cbx_div_currentIndexChanged(int index)
{
    m_hostDiv.div = m_hostInfo.divs[index].name;
    m_hostDiv.hosts = m_hostInfo.divs[index].gws;

    std::cout << "Division = " << m_hostDiv.div << std::endl;
    std::cout << "Port = " << m_hostDiv.port << std::endl;

    for (size_t i = 0; i < m_hostDiv.hosts.size(); ++i)
        std::cout << "\t" << m_hostDiv.hosts[i] << std::endl;

    try
    {
        SrvListClient client(m_io_service,m_hostDiv);
        client.Execute();
        m_io_service.run();
    }
    catch (std::exception &error)
    {
        std::cout << error.what() << std::endl;
    }
}
