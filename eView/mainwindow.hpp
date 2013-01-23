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

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <boost/shared_ptr.hpp>

namespace Ui {
    class MainWindow;
}

class GLWorldWidget;
class QTreeWidgetItem;
class MapManager;
class ModelManager;
class TextureManager;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool Initialize ();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    GLWorldWidget *m_WorldWidget;

    boost::shared_ptr<MapManager> m_MapMgr;
    boost::shared_ptr<ModelManager> m_ModelMgr;
    boost::shared_ptr<TextureManager> m_TextureMgr;

private slots:
    void on_actionCollision_Mesh_triggered(bool checked);
    void on_actionGo_to_Region_triggered();
    void on_actionResolve_Motion_triggered();
    void on_actionNavigation_Mesh_triggered(bool checked);
    void on_actionBounding_Box_triggered(bool checked);
    void on_treeObjects_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
};

#endif // MAINWINDOW_HPP
