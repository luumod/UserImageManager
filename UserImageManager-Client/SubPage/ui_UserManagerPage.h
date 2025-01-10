/********************************************************************************
** Form generated from reading UI file 'UserManagerPage.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USERMANAGERPAGE_H
#define UI_USERMANAGERPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_User_ManagerPage
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QWidget *widget_29;
    QHBoxLayout *horizontalLayout;
    QPushButton *userAddBtn;
    QSpacerItem *horizontalSpacer;
    QLineEdit *searchEdit;
    QPushButton *searchBtn;
    QTableView *tableView;
    QWidget *widget_11;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *batchEnableBtn;
    QPushButton *batchDisableBtn;
    QPushButton *batchDeleteBtn;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QWidget *User_ManagerPage)
    {
        if (User_ManagerPage->objectName().isEmpty())
            User_ManagerPage->setObjectName("User_ManagerPage");
        User_ManagerPage->resize(695, 542);
        verticalLayout_2 = new QVBoxLayout(User_ManagerPage);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        widget_29 = new QWidget(User_ManagerPage);
        widget_29->setObjectName("widget_29");
        horizontalLayout = new QHBoxLayout(widget_29);
        horizontalLayout->setObjectName("horizontalLayout");
        userAddBtn = new QPushButton(widget_29);
        userAddBtn->setObjectName("userAddBtn");

        horizontalLayout->addWidget(userAddBtn);

        horizontalSpacer = new QSpacerItem(367, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        searchEdit = new QLineEdit(widget_29);
        searchEdit->setObjectName("searchEdit");
        searchEdit->setMinimumSize(QSize(200, 0));
        searchEdit->setMaximumSize(QSize(200, 16777215));

        horizontalLayout->addWidget(searchEdit);

        searchBtn = new QPushButton(widget_29);
        searchBtn->setObjectName("searchBtn");

        horizontalLayout->addWidget(searchBtn);


        verticalLayout->addWidget(widget_29);

        tableView = new QTableView(User_ManagerPage);
        tableView->setObjectName("tableView");
        tableView->setMaximumSize(QSize(16777215, 16777215));

        verticalLayout->addWidget(tableView);

        widget_11 = new QWidget(User_ManagerPage);
        widget_11->setObjectName("widget_11");
        horizontalLayout_2 = new QHBoxLayout(widget_11);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        batchEnableBtn = new QPushButton(widget_11);
        batchEnableBtn->setObjectName("batchEnableBtn");

        horizontalLayout_2->addWidget(batchEnableBtn);

        batchDisableBtn = new QPushButton(widget_11);
        batchDisableBtn->setObjectName("batchDisableBtn");

        horizontalLayout_2->addWidget(batchDisableBtn);

        batchDeleteBtn = new QPushButton(widget_11);
        batchDeleteBtn->setObjectName("batchDeleteBtn");

        horizontalLayout_2->addWidget(batchDeleteBtn);

        horizontalSpacer_2 = new QSpacerItem(411, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout->addWidget(widget_11);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(User_ManagerPage);

        QMetaObject::connectSlotsByName(User_ManagerPage);
    } // setupUi

    void retranslateUi(QWidget *User_ManagerPage)
    {
        User_ManagerPage->setWindowTitle(QCoreApplication::translate("User_ManagerPage", "Form", nullptr));
        userAddBtn->setText(QCoreApplication::translate("User_ManagerPage", "\346\267\273\345\212\240\347\224\250\346\210\267", nullptr));
        searchEdit->setPlaceholderText(QCoreApplication::translate("User_ManagerPage", "\350\276\223\345\205\245ID/\347\224\250\346\210\267\345\220\215/\351\202\256\347\256\261", nullptr));
        searchBtn->setText(QCoreApplication::translate("User_ManagerPage", "\346\220\234\347\264\242", nullptr));
        batchEnableBtn->setText(QCoreApplication::translate("User_ManagerPage", "\346\211\271\351\207\217\345\220\257\347\224\250", nullptr));
        batchDisableBtn->setText(QCoreApplication::translate("User_ManagerPage", "\346\211\271\351\207\217\347\246\201\347\224\250", nullptr));
        batchDeleteBtn->setText(QCoreApplication::translate("User_ManagerPage", "\346\211\271\351\207\217\345\210\240\351\231\244", nullptr));
    } // retranslateUi

};

namespace Ui {
    class User_ManagerPage: public Ui_User_ManagerPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERMANAGERPAGE_H
