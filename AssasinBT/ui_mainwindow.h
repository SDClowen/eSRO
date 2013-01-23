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
** Form generated from reading ui file 'mainwindow.ui'
**
** Created: Thu Apr 15 10:32:34 2010
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStackedWidget>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_Quit;
    QAction *actionPreferences;
    QAction *actionAbout;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_4;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *label_srv_status;
    QLabel *label_3;
    QLabel *label_sro_status;
    QLabel *label;
    QLabel *label_12;
    QLabel *label_srv_name;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_5;
    QLabel *label_char_name;
    QLabel *label_10;
    QLabel *label_char_level;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label_7;
    QProgressBar *hp_percent;
    QLabel *label_8;
    QProgressBar *mp_percent;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_9;
    QProgressBar *xp_percent;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_5;
    QTabWidget *configuration_widget;
    QWidget *tab_4;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_9;
    QRadioButton *chat_filter_all;
    QRadioButton *chat_filter_public;
    QRadioButton *chat_filter_private;
    QRadioButton *chat_filter_party;
    QRadioButton *chat_filter_guild;
    QRadioButton *chat_filter_union;
    QRadioButton *chat_filter_stall;
    QRadioButton *chat_filter_notice;
    QFrame *line;
    QPlainTextEdit *chat_history;
    QLineEdit *chat_entry;
    QWidget *tab_3;
    QHBoxLayout *horizontalLayout_8;
    QTreeWidget *settings_view;
    QStackedWidget *stackedWidget;
    QWidget *page;
    QWidget *page_2;
    QWidget *tab;
    QHBoxLayout *horizontalLayout_7;
    QPlainTextEdit *log_history;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *bt_server;
    QPushButton *bt_service;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menu_Edit;
    QMenu *menu_Help;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(613, 495);
        action_Quit = new QAction(MainWindow);
        action_Quit->setObjectName(QString::fromUtf8("action_Quit"));
        actionPreferences = new QAction(MainWindow);
        actionPreferences->setObjectName(QString::fromUtf8("actionPreferences"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setMargin(11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setSizeConstraint(QLayout::SetNoConstraint);
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setSpacing(6);
        gridLayout->setMargin(11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetNoConstraint);
        label_srv_status = new QLabel(groupBox);
        label_srv_status->setObjectName(QString::fromUtf8("label_srv_status"));
        label_srv_status->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_srv_status, 0, 1, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        label_sro_status = new QLabel(groupBox);
        label_sro_status->setObjectName(QString::fromUtf8("label_sro_status"));
        label_sro_status->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_sro_status, 2, 1, 1, 1);

        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        label_12 = new QLabel(groupBox);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        gridLayout->addWidget(label_12, 3, 0, 1, 1);

        label_srv_name = new QLabel(groupBox);
        label_srv_name->setObjectName(QString::fromUtf8("label_srv_name"));
        label_srv_name->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_srv_name, 3, 1, 1, 1);


        horizontalLayout_4->addWidget(groupBox);

        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        verticalLayout_3 = new QVBoxLayout(groupBox_2);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setMargin(11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setSizeConstraint(QLayout::SetNoConstraint);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_3->addWidget(label_5);

        label_char_name = new QLabel(groupBox_2);
        label_char_name->setObjectName(QString::fromUtf8("label_char_name"));

        horizontalLayout_3->addWidget(label_char_name);

        label_10 = new QLabel(groupBox_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        horizontalLayout_3->addWidget(label_10);

        label_char_level = new QLabel(groupBox_2);
        label_char_level->setObjectName(QString::fromUtf8("label_char_level"));

        horizontalLayout_3->addWidget(label_char_level);


        verticalLayout_3->addLayout(horizontalLayout_3);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_7 = new QLabel(groupBox_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        horizontalLayout->addWidget(label_7);

        hp_percent = new QProgressBar(groupBox_2);
        hp_percent->setObjectName(QString::fromUtf8("hp_percent"));
        hp_percent->setValue(24);

        horizontalLayout->addWidget(hp_percent);

        label_8 = new QLabel(groupBox_2);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout->addWidget(label_8);

        mp_percent = new QProgressBar(groupBox_2);
        mp_percent->setObjectName(QString::fromUtf8("mp_percent"));
        mp_percent->setValue(24);

        horizontalLayout->addWidget(mp_percent);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_9 = new QLabel(groupBox_2);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        horizontalLayout_2->addWidget(label_9);

        xp_percent = new QProgressBar(groupBox_2);
        xp_percent->setObjectName(QString::fromUtf8("xp_percent"));
        xp_percent->setValue(24);

        horizontalLayout_2->addWidget(xp_percent);


        verticalLayout_2->addLayout(horizontalLayout_2);


        verticalLayout_3->addLayout(verticalLayout_2);


        horizontalLayout_4->addWidget(groupBox_2);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        configuration_widget = new QTabWidget(centralWidget);
        configuration_widget->setObjectName(QString::fromUtf8("configuration_widget"));
        configuration_widget->setMovable(true);
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        configuration_widget->addTab(tab_4, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        verticalLayout_4 = new QVBoxLayout(tab_2);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setMargin(11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        chat_filter_all = new QRadioButton(tab_2);
        chat_filter_all->setObjectName(QString::fromUtf8("chat_filter_all"));

        horizontalLayout_9->addWidget(chat_filter_all);

        chat_filter_public = new QRadioButton(tab_2);
        chat_filter_public->setObjectName(QString::fromUtf8("chat_filter_public"));

        horizontalLayout_9->addWidget(chat_filter_public);

        chat_filter_private = new QRadioButton(tab_2);
        chat_filter_private->setObjectName(QString::fromUtf8("chat_filter_private"));

        horizontalLayout_9->addWidget(chat_filter_private);

        chat_filter_party = new QRadioButton(tab_2);
        chat_filter_party->setObjectName(QString::fromUtf8("chat_filter_party"));

        horizontalLayout_9->addWidget(chat_filter_party);

        chat_filter_guild = new QRadioButton(tab_2);
        chat_filter_guild->setObjectName(QString::fromUtf8("chat_filter_guild"));

        horizontalLayout_9->addWidget(chat_filter_guild);

        chat_filter_union = new QRadioButton(tab_2);
        chat_filter_union->setObjectName(QString::fromUtf8("chat_filter_union"));

        horizontalLayout_9->addWidget(chat_filter_union);

        chat_filter_stall = new QRadioButton(tab_2);
        chat_filter_stall->setObjectName(QString::fromUtf8("chat_filter_stall"));

        horizontalLayout_9->addWidget(chat_filter_stall);

        chat_filter_notice = new QRadioButton(tab_2);
        chat_filter_notice->setObjectName(QString::fromUtf8("chat_filter_notice"));

        horizontalLayout_9->addWidget(chat_filter_notice);


        verticalLayout_4->addLayout(horizontalLayout_9);

        line = new QFrame(tab_2);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout_4->addWidget(line);

        chat_history = new QPlainTextEdit(tab_2);
        chat_history->setObjectName(QString::fromUtf8("chat_history"));

        verticalLayout_4->addWidget(chat_history);

        chat_entry = new QLineEdit(tab_2);
        chat_entry->setObjectName(QString::fromUtf8("chat_entry"));

        verticalLayout_4->addWidget(chat_entry);

        configuration_widget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        horizontalLayout_8 = new QHBoxLayout(tab_3);
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setMargin(11);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        settings_view = new QTreeWidget(tab_3);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem(settings_view);
        new QTreeWidgetItem(__qtreewidgetitem);
        new QTreeWidgetItem(__qtreewidgetitem);
        new QTreeWidgetItem(__qtreewidgetitem);
        new QTreeWidgetItem(__qtreewidgetitem);
        QTreeWidgetItem *__qtreewidgetitem1 = new QTreeWidgetItem(settings_view);
        new QTreeWidgetItem(__qtreewidgetitem1);
        new QTreeWidgetItem(__qtreewidgetitem1);
        QTreeWidgetItem *__qtreewidgetitem2 = new QTreeWidgetItem(settings_view);
        new QTreeWidgetItem(__qtreewidgetitem2);
        new QTreeWidgetItem(__qtreewidgetitem2);
        new QTreeWidgetItem(__qtreewidgetitem2);
        new QTreeWidgetItem(__qtreewidgetitem2);
        new QTreeWidgetItem(__qtreewidgetitem2);
        new QTreeWidgetItem(__qtreewidgetitem2);
        new QTreeWidgetItem(__qtreewidgetitem2);
        QTreeWidgetItem *__qtreewidgetitem3 = new QTreeWidgetItem(settings_view);
        new QTreeWidgetItem(__qtreewidgetitem3);
        QTreeWidgetItem *__qtreewidgetitem4 = new QTreeWidgetItem(settings_view);
        new QTreeWidgetItem(__qtreewidgetitem4);
        settings_view->setObjectName(QString::fromUtf8("settings_view"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(settings_view->sizePolicy().hasHeightForWidth());
        settings_view->setSizePolicy(sizePolicy);
        settings_view->setMinimumSize(QSize(150, 0));
        settings_view->setMaximumSize(QSize(150, 16777215));
        settings_view->setColumnCount(1);
        settings_view->header()->setVisible(false);

        horizontalLayout_8->addWidget(settings_view);

        stackedWidget = new QStackedWidget(tab_3);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        page = new QWidget();
        page->setObjectName(QString::fromUtf8("page"));
        stackedWidget->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName(QString::fromUtf8("page_2"));
        stackedWidget->addWidget(page_2);

        horizontalLayout_8->addWidget(stackedWidget);

        configuration_widget->addTab(tab_3, QString());
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        horizontalLayout_7 = new QHBoxLayout(tab);
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setMargin(11);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        log_history = new QPlainTextEdit(tab);
        log_history->setObjectName(QString::fromUtf8("log_history"));

        horizontalLayout_7->addWidget(log_history);

        configuration_widget->addTab(tab, QString());

        horizontalLayout_5->addWidget(configuration_widget);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_2);

        bt_server = new QPushButton(centralWidget);
        bt_server->setObjectName(QString::fromUtf8("bt_server"));

        horizontalLayout_6->addWidget(bt_server);

        bt_service = new QPushButton(centralWidget);
        bt_service->setObjectName(QString::fromUtf8("bt_service"));
        bt_service->setEnabled(false);

        horizontalLayout_6->addWidget(bt_service);


        verticalLayout->addLayout(horizontalLayout_6);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 613, 25));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menu_Edit = new QMenu(menuBar);
        menu_Edit->setObjectName(QString::fromUtf8("menu_Edit"));
        menu_Help = new QMenu(menuBar);
        menu_Help->setObjectName(QString::fromUtf8("menu_Help"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menu_Edit->menuAction());
        menuBar->addAction(menu_Help->menuAction());
        menuFile->addAction(action_Quit);
        menu_Edit->addAction(actionPreferences);
        menu_Help->addAction(actionAbout);

        retranslateUi(MainWindow);

        configuration_widget->setCurrentIndex(2);
        stackedWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        action_Quit->setText(QApplication::translate("MainWindow", "&Quit", 0, QApplication::UnicodeUTF8));
        actionPreferences->setText(QApplication::translate("MainWindow", "Preferences", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("MainWindow", "About", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MainWindow", "Connection Stats", 0, QApplication::UnicodeUTF8));
        label_srv_status->setText(QApplication::translate("MainWindow", "Disconnected", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Silkroad:", 0, QApplication::UnicodeUTF8));
        label_sro_status->setText(QApplication::translate("MainWindow", "Disconnected", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Service:", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("MainWindow", "Server:", 0, QApplication::UnicodeUTF8));
        label_srv_name->setText(QApplication::translate("MainWindow", "-", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Character Stats", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "Name:", 0, QApplication::UnicodeUTF8));
        label_char_name->setText(QString());
        label_10->setText(QApplication::translate("MainWindow", "Level:", 0, QApplication::UnicodeUTF8));
        label_char_level->setText(QString());
        label_7->setText(QApplication::translate("MainWindow", "HP:", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("MainWindow", "MP:", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("MainWindow", "XP:", 0, QApplication::UnicodeUTF8));
        configuration_widget->setTabText(configuration_widget->indexOf(tab_4), QApplication::translate("MainWindow", "Statistics", 0, QApplication::UnicodeUTF8));
        chat_filter_all->setText(QApplication::translate("MainWindow", "All", 0, QApplication::UnicodeUTF8));
        chat_filter_public->setText(QApplication::translate("MainWindow", "Public", 0, QApplication::UnicodeUTF8));
        chat_filter_private->setText(QApplication::translate("MainWindow", "Private", 0, QApplication::UnicodeUTF8));
        chat_filter_party->setText(QApplication::translate("MainWindow", "Party", 0, QApplication::UnicodeUTF8));
        chat_filter_guild->setText(QApplication::translate("MainWindow", "Guild", 0, QApplication::UnicodeUTF8));
        chat_filter_union->setText(QApplication::translate("MainWindow", "Union", 0, QApplication::UnicodeUTF8));
        chat_filter_stall->setText(QApplication::translate("MainWindow", "Stall", 0, QApplication::UnicodeUTF8));
        chat_filter_notice->setText(QApplication::translate("MainWindow", "Notice/GM", 0, QApplication::UnicodeUTF8));
        configuration_widget->setTabText(configuration_widget->indexOf(tab_2), QApplication::translate("MainWindow", "Chat", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = settings_view->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("MainWindow", "1", 0, QApplication::UnicodeUTF8));

        const bool __sortingEnabled = settings_view->isSortingEnabled();
        settings_view->setSortingEnabled(false);
        QTreeWidgetItem *___qtreewidgetitem1 = settings_view->topLevelItem(0);
        ___qtreewidgetitem1->setText(0, QApplication::translate("MainWindow", "Planner", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem2 = ___qtreewidgetitem1->child(0);
        ___qtreewidgetitem2->setText(0, QApplication::translate("MainWindow", "Stats", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem3 = ___qtreewidgetitem1->child(1);
        ___qtreewidgetitem3->setText(0, QApplication::translate("MainWindow", "Mastery", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem4 = ___qtreewidgetitem1->child(2);
        ___qtreewidgetitem4->setText(0, QApplication::translate("MainWindow", "Skill", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem5 = ___qtreewidgetitem1->child(3);
        ___qtreewidgetitem5->setText(0, QApplication::translate("MainWindow", "Farm", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem6 = settings_view->topLevelItem(1);
        ___qtreewidgetitem6->setText(0, QApplication::translate("MainWindow", "Hunting Area", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem7 = ___qtreewidgetitem6->child(0);
        ___qtreewidgetitem7->setText(0, QApplication::translate("MainWindow", "Area", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem8 = ___qtreewidgetitem6->child(1);
        ___qtreewidgetitem8->setText(0, QApplication::translate("MainWindow", "Waypoints", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem9 = settings_view->topLevelItem(2);
        ___qtreewidgetitem9->setText(0, QApplication::translate("MainWindow", "Items", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem10 = ___qtreewidgetitem9->child(0);
        ___qtreewidgetitem10->setText(0, QApplication::translate("MainWindow", "Stock", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem11 = ___qtreewidgetitem9->child(1);
        ___qtreewidgetitem11->setText(0, QApplication::translate("MainWindow", "Policy", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem12 = ___qtreewidgetitem9->child(2);
        ___qtreewidgetitem12->setText(0, QApplication::translate("MainWindow", "Equipment", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem13 = ___qtreewidgetitem9->child(3);
        ___qtreewidgetitem13->setText(0, QApplication::translate("MainWindow", "Transport", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem14 = ___qtreewidgetitem9->child(4);
        ___qtreewidgetitem14->setText(0, QApplication::translate("MainWindow", "Grow Pet", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem15 = ___qtreewidgetitem9->child(5);
        ___qtreewidgetitem15->setText(0, QApplication::translate("MainWindow", "Ability Pet", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem16 = ___qtreewidgetitem9->child(6);
        ___qtreewidgetitem16->setText(0, QApplication::translate("MainWindow", "Alchemy", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem17 = settings_view->topLevelItem(3);
        ___qtreewidgetitem17->setText(0, QApplication::translate("MainWindow", "Party", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem18 = ___qtreewidgetitem17->child(0);
        ___qtreewidgetitem18->setText(0, QApplication::translate("MainWindow", "Party", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem19 = settings_view->topLevelItem(4);
        ___qtreewidgetitem19->setText(0, QApplication::translate("MainWindow", "Autopot", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem20 = ___qtreewidgetitem19->child(0);
        ___qtreewidgetitem20->setText(0, QApplication::translate("MainWindow", "Autopot", 0, QApplication::UnicodeUTF8));
        settings_view->setSortingEnabled(__sortingEnabled);

        configuration_widget->setTabText(configuration_widget->indexOf(tab_3), QApplication::translate("MainWindow", "Settings", 0, QApplication::UnicodeUTF8));
        configuration_widget->setTabText(configuration_widget->indexOf(tab), QApplication::translate("MainWindow", "Log", 0, QApplication::UnicodeUTF8));
        bt_server->setText(QApplication::translate("MainWindow", "Connect", 0, QApplication::UnicodeUTF8));
        bt_service->setText(QApplication::translate("MainWindow", "Run", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));
        menu_Edit->setTitle(QApplication::translate("MainWindow", "&Edit", 0, QApplication::UnicodeUTF8));
        menu_Help->setTitle(QApplication::translate("MainWindow", "&Help", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
