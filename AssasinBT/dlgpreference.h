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

#ifndef DLGPREFERENCE_H
#define DLGPREFERENCE_H

#include <host_info.hpp>
#include <boost/asio.hpp>
#include <QtGui/QDialog>

namespace Ui {
    class DlgPreference;
}

class DlgPreference : public QDialog {
    Q_OBJECT
public:
    DlgPreference(QWidget *parent = 0);
    ~DlgPreference();

    void SetHostInfo (const HostInfo &info);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DlgPreference *m_ui;
    HostInfo m_hostInfo;
    HostDivisionInfo m_hostDiv;
    boost::asio::io_service m_io_service;

private slots:
    void on_cbx_div_currentIndexChanged(int index);
};

#endif // DLGPREFERENCE_H
