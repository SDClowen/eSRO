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
** Form generated from reading ui file 'dlgpreference.ui'
**
** Created: Thu Apr 15 10:32:34 2010
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_DLGPREFERENCE_H
#define UI_DLGPREFERENCE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DlgPreference
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *label;
    QComboBox *cbx_div;
    QLabel *label_2;
    QComboBox *cbx_srv;
    QLabel *label_3;
    QLineEdit *edt_account;
    QLabel *label_4;
    QLineEdit *edt_pass;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DlgPreference)
    {
        if (DlgPreference->objectName().isEmpty())
            DlgPreference->setObjectName(QString::fromUtf8("DlgPreference"));
        DlgPreference->setWindowModality(Qt::ApplicationModal);
        DlgPreference->resize(400, 300);
        DlgPreference->setModal(true);
        verticalLayout = new QVBoxLayout(DlgPreference);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        groupBox = new QGroupBox(DlgPreference);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        cbx_div = new QComboBox(groupBox);
        cbx_div->setObjectName(QString::fromUtf8("cbx_div"));

        gridLayout->addWidget(cbx_div, 0, 1, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        cbx_srv = new QComboBox(groupBox);
        cbx_srv->setObjectName(QString::fromUtf8("cbx_srv"));

        gridLayout->addWidget(cbx_srv, 1, 1, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        edt_account = new QLineEdit(groupBox);
        edt_account->setObjectName(QString::fromUtf8("edt_account"));

        gridLayout->addWidget(edt_account, 2, 1, 1, 1);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 3, 0, 1, 1);

        edt_pass = new QLineEdit(groupBox);
        edt_pass->setObjectName(QString::fromUtf8("edt_pass"));

        gridLayout->addWidget(edt_pass, 3, 1, 1, 1);


        verticalLayout->addWidget(groupBox);

        buttonBox = new QDialogButtonBox(DlgPreference);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(DlgPreference);
        QObject::connect(buttonBox, SIGNAL(accepted()), DlgPreference, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DlgPreference, SLOT(reject()));

        QMetaObject::connectSlotsByName(DlgPreference);
    } // setupUi

    void retranslateUi(QDialog *DlgPreference)
    {
        DlgPreference->setWindowTitle(QApplication::translate("DlgPreference", "Dialog", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("DlgPreference", "Preferences", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DlgPreference", "Division:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("DlgPreference", "Server:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("DlgPreference", "Account:", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("DlgPreference", "Password:", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(DlgPreference);
    } // retranslateUi

};

namespace Ui {
    class DlgPreference: public Ui_DlgPreference {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGPREFERENCE_H
