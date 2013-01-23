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

/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Tue Aug 23 20:11:20 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QTreeWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_Quit;
    QAction *actionPreferences;
    QAction *action_Open;
    QAction *action_Close;
    QAction *actionAbout;
    QAction *actionBounding_Box;
    QAction *actionCollision_Mesh;
    QAction *actionNavigation_Mesh;
    QAction *actionGo_to_Region;
    QAction *actionResolve_Motion;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QSplitter *splitter_2;
    QWidget *ViewWidget;
    QSplitter *splitter;
    QTreeWidget *treeObjects;
    QTreeWidget *treeProperty;
    QMenuBar *menuBar;
    QMenu *menu_File;
    QMenu *menu_Edit;
    QMenu *menu_Help;
    QMenu *menu_View;
    QMenu *menuModels;
    QMenu *menuTerrain;
    QMenu *menu_Tools;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(618, 418);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        action_Quit = new QAction(MainWindow);
        action_Quit->setObjectName(QString::fromUtf8("action_Quit"));
        actionPreferences = new QAction(MainWindow);
        actionPreferences->setObjectName(QString::fromUtf8("actionPreferences"));
        action_Open = new QAction(MainWindow);
        action_Open->setObjectName(QString::fromUtf8("action_Open"));
        action_Close = new QAction(MainWindow);
        action_Close->setObjectName(QString::fromUtf8("action_Close"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionBounding_Box = new QAction(MainWindow);
        actionBounding_Box->setObjectName(QString::fromUtf8("actionBounding_Box"));
        actionBounding_Box->setCheckable(true);
        actionCollision_Mesh = new QAction(MainWindow);
        actionCollision_Mesh->setObjectName(QString::fromUtf8("actionCollision_Mesh"));
        actionCollision_Mesh->setCheckable(true);
        actionNavigation_Mesh = new QAction(MainWindow);
        actionNavigation_Mesh->setObjectName(QString::fromUtf8("actionNavigation_Mesh"));
        actionNavigation_Mesh->setCheckable(true);
        actionGo_to_Region = new QAction(MainWindow);
        actionGo_to_Region->setObjectName(QString::fromUtf8("actionGo_to_Region"));
        actionResolve_Motion = new QAction(MainWindow);
        actionResolve_Motion->setObjectName(QString::fromUtf8("actionResolve_Motion"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        splitter_2 = new QSplitter(centralWidget);
        splitter_2->setObjectName(QString::fromUtf8("splitter_2"));
        splitter_2->setOrientation(Qt::Horizontal);
        ViewWidget = new QWidget(splitter_2);
        ViewWidget->setObjectName(QString::fromUtf8("ViewWidget"));
        ViewWidget->setMinimumSize(QSize(380, 280));
        splitter_2->addWidget(ViewWidget);
        splitter = new QSplitter(splitter_2);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Vertical);
        treeObjects = new QTreeWidget(splitter);
        treeObjects->setObjectName(QString::fromUtf8("treeObjects"));
        splitter->addWidget(treeObjects);
        treeProperty = new QTreeWidget(splitter);
        treeProperty->setObjectName(QString::fromUtf8("treeProperty"));
        splitter->addWidget(treeProperty);
        splitter_2->addWidget(splitter);

        horizontalLayout->addWidget(splitter_2);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 618, 23));
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QString::fromUtf8("menu_File"));
        menu_Edit = new QMenu(menuBar);
        menu_Edit->setObjectName(QString::fromUtf8("menu_Edit"));
        menu_Help = new QMenu(menuBar);
        menu_Help->setObjectName(QString::fromUtf8("menu_Help"));
        menu_View = new QMenu(menuBar);
        menu_View->setObjectName(QString::fromUtf8("menu_View"));
        menuModels = new QMenu(menu_View);
        menuModels->setObjectName(QString::fromUtf8("menuModels"));
        menuTerrain = new QMenu(menu_View);
        menuTerrain->setObjectName(QString::fromUtf8("menuTerrain"));
        menu_Tools = new QMenu(menuBar);
        menu_Tools->setObjectName(QString::fromUtf8("menu_Tools"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menu_File->menuAction());
        menuBar->addAction(menu_Edit->menuAction());
        menuBar->addAction(menu_View->menuAction());
        menuBar->addAction(menu_Tools->menuAction());
        menuBar->addAction(menu_Help->menuAction());
        menu_File->addAction(action_Open);
        menu_File->addAction(action_Close);
        menu_File->addAction(action_Quit);
        menu_Edit->addAction(actionPreferences);
        menu_Help->addAction(actionAbout);
        menu_View->addAction(menuModels->menuAction());
        menu_View->addAction(menuTerrain->menuAction());
        menuModels->addAction(actionBounding_Box);
        menuModels->addAction(actionCollision_Mesh);
        menuTerrain->addAction(actionNavigation_Mesh);
        menu_Tools->addAction(actionGo_to_Region);
        menu_Tools->addAction(actionResolve_Motion);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "eView", 0, QApplication::UnicodeUTF8));
        action_Quit->setText(QApplication::translate("MainWindow", "&Quit", 0, QApplication::UnicodeUTF8));
        actionPreferences->setText(QApplication::translate("MainWindow", "Preferences", 0, QApplication::UnicodeUTF8));
        action_Open->setText(QApplication::translate("MainWindow", "&Open", 0, QApplication::UnicodeUTF8));
        action_Close->setText(QApplication::translate("MainWindow", "&Close", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("MainWindow", "About", 0, QApplication::UnicodeUTF8));
        actionBounding_Box->setText(QApplication::translate("MainWindow", "Bounding Box", 0, QApplication::UnicodeUTF8));
        actionCollision_Mesh->setText(QApplication::translate("MainWindow", "Collision Mesh", 0, QApplication::UnicodeUTF8));
        actionNavigation_Mesh->setText(QApplication::translate("MainWindow", "Navigation Mesh", 0, QApplication::UnicodeUTF8));
        actionGo_to_Region->setText(QApplication::translate("MainWindow", "Go to Region", 0, QApplication::UnicodeUTF8));
        actionResolve_Motion->setText(QApplication::translate("MainWindow", "Resolve Motion", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = treeObjects->headerItem();
        ___qtreewidgetitem->setText(1, QApplication::translate("MainWindow", "Class", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(0, QApplication::translate("MainWindow", "Object", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem1 = treeProperty->headerItem();
        ___qtreewidgetitem1->setText(1, QApplication::translate("MainWindow", "Value", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem1->setText(0, QApplication::translate("MainWindow", "Property", 0, QApplication::UnicodeUTF8));
        menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));
        menu_Edit->setTitle(QApplication::translate("MainWindow", "&Edit", 0, QApplication::UnicodeUTF8));
        menu_Help->setTitle(QApplication::translate("MainWindow", "&Help", 0, QApplication::UnicodeUTF8));
        menu_View->setTitle(QApplication::translate("MainWindow", "&View", 0, QApplication::UnicodeUTF8));
        menuModels->setTitle(QApplication::translate("MainWindow", "Models", 0, QApplication::UnicodeUTF8));
        menuTerrain->setTitle(QApplication::translate("MainWindow", "Terrain", 0, QApplication::UnicodeUTF8));
        menu_Tools->setTitle(QApplication::translate("MainWindow", "&Tools", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
