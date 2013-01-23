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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

namespace Ui
{
    class MainWindow;
}

class IPK2;
class HostInfo;

///UI FOWARD DECLARATIONS
class DlgPreference;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool ReadHostInfo ();

private:

    void ReadConfiguration ();

private:
    Ui::MainWindow *ui;
    DlgPreference *m_dlgpreference;

    bool m_connected;
    bool m_running;

    IPK2 *m_reader;
    std::string m_filename;
    HostInfo *m_hostInfo;

private slots:
    void on_actionAbout_triggered();
    void on_action_Quit_triggered();
    void on_actionPreferences_triggered();
    void on_bt_service_clicked();
    void on_bt_server_clicked();
};

#endif // MAINWINDOW_H
