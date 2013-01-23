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

#include "mainwindow.hpp"
#include "glworldwidget.hpp"
#include "ui_mainwindow.h"
#include "map_manager.hpp"
#include "zone.hpp"

#include <model.hpp>
#include <model_mesh.hpp>
#include <model_mesh_collision.hpp>
#include <model_manager.hpp>
#include <navigation_mesh.hpp>
#include <texture_manager.hpp>

#include <QDebug>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_MapMgr(new MapManager("/home/ralph/PK2Tools")),
    m_ModelMgr(new ModelManager("/home/ralph/PK2Tools/Data/")),
    m_TextureMgr(new TextureManager("/home/ralph/PK2Tools/Map/"))
{
    ui->setupUi(this);

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom,ui->ViewWidget);
    m_WorldWidget = new GLWorldWidget(m_ModelMgr,m_TextureMgr,ui->ViewWidget);
    layout->addWidget(m_WorldWidget);

    ui->ViewWidget->setLayout(layout);
    m_WorldWidget->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::Initialize ()
{
    if (!m_ModelMgr->Initialize())
        return false;

    m_ModelMgr->LoadModels();

    if (!m_TextureMgr->Initialize())
        return false;

    return true;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_treeObjects_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    if (current)
    {
        boost::shared_ptr<Zone> zone = m_WorldWidget->getCurrentZone();

        if (!zone)
            return;

        boost::shared_ptr<NavigationMesh> mesh = zone->getNavigationMesh();

        if (!mesh)
            return;

        ui->treeProperty->clear();

        size_t objIdx = current->data(2,0).toUInt();

        NavigationEntity entity = mesh->getEntityAt(objIdx);

        QTreeWidgetItem *position = new QTreeWidgetItem((QTreeWidget*)0,QStringList(QString("Position & Orientation")));
        ui->treeProperty->addTopLevelItem(position);

        position->setExpanded(true);

        QStringList list;
        list.append(QString("X"));
        list.append(QString("%1").arg(entity.x));
        list.append(QString("Y"));
        list.append(QString("%1").arg(entity.y));
        list.append(QString("Z"));
        list.append(QString("%1").arg(entity.z));
        list.append(QString("Angle"));
        list.append(QString("%1").arg(entity.angle));

        QStringList clist;
        QTreeWidgetItem *child;
        for (int i = 0; i < list.size()/2; ++i)
        {
            clist.append(list.at(2*i));
            clist.append(list.at(2*i+1));

            child = new QTreeWidgetItem(position,clist);
            ui->treeProperty->addTopLevelItem(child);

            clist.clear();
        }

        list.clear();

        QTreeWidgetItem *model = new QTreeWidgetItem((QTreeWidget*)0,QStringList(QString("Model")));
        ui->treeProperty->addTopLevelItem(model);

        model->setExpanded(true);

        list.append("Name");
        list.append("xxx");
        list.append("Path");
        list.append("/home/npcdoom/xxx");

        for (int i = 0; i < list.size()/2; ++i)
        {
            clist.append(list.at(2*i));
            clist.append(list.at(2*i+1));

            child = new QTreeWidgetItem(model,clist);
            ui->treeProperty->addTopLevelItem(child);

            clist.clear();
        }
    }
}

void MainWindow::on_actionBounding_Box_triggered(bool checked)
{
    m_WorldWidget->setRenderAABBs(checked);
}

void MainWindow::on_actionResolve_Motion_triggered()
{
    m_WorldWidget->resolveMotion();
}

void MainWindow::on_actionGo_to_Region_triggered()
{
    QString filename;

//    filename = QFileDialog::getOpenFileName(this,tr("Open Zone"),"/home/npcdoom/PK2Tools/data/navmesh",
//        tr("Navigation Mesh Files (*.nvm)"));
//
//    filename.remove("/home/npcdoom/PK2Tools/data/navmesh/");

//    filename = "nv_5b95.nvm";
//    filename = "nv_5b94.nvm";
//	filename = "nv_60a8.nvm";
//	filename = "nv_5c83.nvm";
//    filename = "nv_5e87.nvm";
    filename = "nv_61a5.nvm";
//    filename = "nv_6c6d.nvm";

    if(m_MapMgr->LoadZone(filename.toStdString().c_str(),NavigationMesh::MODE_ALL))
    {
        int16_t zoneID = strtol(filename.toStdString().c_str()+3,NULL,16);

        boost::shared_ptr<Zone> zone = m_MapMgr->Find(zoneID);

        if (zone)
        {
            ui->treeObjects->clear();
            ui->treeProperty->clear();

            QStringList slist;
            QTreeWidgetItem *item;

            boost::shared_ptr<NavigationMesh> mesh = zone->getNavigationMesh();

            /// LOAD ZONE TEXTURES
            for (int z = 0; z < NavigationMesh::TEXTURE_SIZE; ++z)
            {
                for (int x = 0; x < NavigationMesh::TEXTURE_SIZE; ++x)
                {
                    if (!m_TextureMgr->LoadTexture(mesh->getTextureAt(x,z)))
                        qDebug() << "Failed to load texture at (" << x << "," << z << ") = " << mesh->getTextureAt(x,z);
                }
            }

            /// LOAD ZONE OBJECTS
            ObjectInfo info;
            std::vector<NavigationEntity> entities = mesh->getEntities();

            for (NavigationMesh::entity_const_iterator it = entities.begin(); it != entities.end(); ++it)
            {
                if (m_ModelMgr->LoadModel(it->refID))
                {
                    boost::shared_ptr<Model> model = m_ModelMgr->FindModel(it->refID);

                    assert(model);

                    slist.append(QString(model->getName().c_str()));
                    slist.append(QString("Static Object"));
                    slist.append(QString("%1").arg(it-entities.begin()));

                    item = new QTreeWidgetItem((QTreeWidgetItem*)NULL,slist);

                    if (item)
                        ui->treeObjects->addTopLevelItem(item);

                    slist.clear();
                }
                else
                    qDebug() << "Failed to load model with refID = " << it->refID;
            }

            m_WorldWidget->setCurrentZone(zone);
        }
    }
}

void MainWindow::on_actionNavigation_Mesh_triggered(bool checked)
{
    m_WorldWidget->setRenderNavMesh(checked);
}

void MainWindow::on_actionCollision_Mesh_triggered(bool checked)
{
    m_WorldWidget->setRenderCollisionMesh(checked);
}
