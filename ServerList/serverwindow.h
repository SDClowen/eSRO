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

#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QtGui/QMainWindow>

namespace Ui
{
    class ServerWindow;
}

class IPK2;
class HostInfo;
class HostDivisionInfo;
class networkthread;

class ServerWindow : public QMainWindow
{
    Q_OBJECT

public:
    ServerWindow(QWidget *parent = 0);
    ~ServerWindow();

    bool ReadMediaInfo ();

private:
    Ui::ServerWindow *ui;

    IPK2 *m_reader;
    std::string m_filename;
    HostInfo *m_hostInfo;
    HostDivisionInfo *m_hostDiv;
    networkthread *m_netThread;

    QTextCodec *m_codec;

private slots:
    void on_cbx_div_currentIndexChanged(int index);
    void on_action_Quit_triggered();
    void on_shard_list ();
};

#endif // SERVERWINDOW_H
