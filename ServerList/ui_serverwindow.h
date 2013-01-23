/********************************************************************************
** Form generated from reading ui file 'serverwindow.ui'
**
** Created: Tue Feb 2 17:45:38 2010
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_SERVERWINDOW_H
#define UI_SERVERWINDOW_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ServerWindow
{
public:
    QAction *action_Quit;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *cbx_div;
    QSpacerItem *horizontalSpacer;
    QTreeWidget *tree_srv_list;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ServerWindow)
    {
        if (ServerWindow->objectName().isEmpty())
            ServerWindow->setObjectName(QString::fromUtf8("ServerWindow"));
        ServerWindow->resize(432, 293);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ServerWindow->sizePolicy().hasHeightForWidth());
        ServerWindow->setSizePolicy(sizePolicy);
        action_Quit = new QAction(ServerWindow);
        action_Quit->setObjectName(QString::fromUtf8("action_Quit"));
        centralWidget = new QWidget(ServerWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setMargin(11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        cbx_div = new QComboBox(centralWidget);
        cbx_div->setObjectName(QString::fromUtf8("cbx_div"));

        horizontalLayout->addWidget(cbx_div);

        horizontalSpacer = new QSpacerItem(260, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        tree_srv_list = new QTreeWidget(centralWidget);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setTextAlignment(3, Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        __qtreewidgetitem->setTextAlignment(2, Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        __qtreewidgetitem->setTextAlignment(1, Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        __qtreewidgetitem->setTextAlignment(0, Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tree_srv_list->setHeaderItem(__qtreewidgetitem);
        tree_srv_list->setObjectName(QString::fromUtf8("tree_srv_list"));
        tree_srv_list->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        tree_srv_list->setIndentation(10);
        tree_srv_list->header()->setDefaultSectionSize(90);

        verticalLayout->addWidget(tree_srv_list);

        ServerWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(ServerWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        ServerWindow->setStatusBar(statusBar);

        retranslateUi(ServerWindow);

        QMetaObject::connectSlotsByName(ServerWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ServerWindow)
    {
        ServerWindow->setWindowTitle(QApplication::translate("ServerWindow", "ServerWindow", 0, QApplication::UnicodeUTF8));
        action_Quit->setText(QApplication::translate("ServerWindow", "&Quit", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ServerWindow", "Division:", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = tree_srv_list->headerItem();
        ___qtreewidgetitem->setText(3, QApplication::translate("ServerWindow", "Status", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(2, QApplication::translate("ServerWindow", "Capacity", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(1, QApplication::translate("ServerWindow", "Users", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(0, QApplication::translate("ServerWindow", "Name", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(ServerWindow);
    } // retranslateUi

};

namespace Ui {
    class ServerWindow: public Ui_ServerWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVERWINDOW_H
