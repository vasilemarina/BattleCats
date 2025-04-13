/********************************************************************************
** Form generated from reading UI file 'PlayerStatsWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLAYERSTATSWINDOW_H
#define UI_PLAYERSTATSWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlayerStatsWindow
{
public:
    QWidget *centralwidget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *usernameLabel;
    QLabel *scoreLabel;
    QLabel *bulletSpeedLabel;
    QLabel *reloadTimeLabel;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *PlayerStatsWindow)
    {
        if (PlayerStatsWindow->objectName().isEmpty())
            PlayerStatsWindow->setObjectName("PlayerStatsWindow");
        PlayerStatsWindow->resize(800, 600);
        centralwidget = new QWidget(PlayerStatsWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayoutWidget = new QWidget(centralwidget);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(90, 160, 571, 251));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        usernameLabel = new QLabel(verticalLayoutWidget);
        usernameLabel->setObjectName("usernameLabel");

        verticalLayout->addWidget(usernameLabel);

        scoreLabel = new QLabel(verticalLayoutWidget);
        scoreLabel->setObjectName("scoreLabel");

        verticalLayout->addWidget(scoreLabel);

        bulletSpeedLabel = new QLabel(verticalLayoutWidget);
        bulletSpeedLabel->setObjectName("bulletSpeedLabel");

        verticalLayout->addWidget(bulletSpeedLabel);

        reloadTimeLabel = new QLabel(verticalLayoutWidget);
        reloadTimeLabel->setObjectName("reloadTimeLabel");

        verticalLayout->addWidget(reloadTimeLabel);

        PlayerStatsWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(PlayerStatsWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 25));
        PlayerStatsWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(PlayerStatsWindow);
        statusbar->setObjectName("statusbar");
        PlayerStatsWindow->setStatusBar(statusbar);

        retranslateUi(PlayerStatsWindow);

        QMetaObject::connectSlotsByName(PlayerStatsWindow);
    } // setupUi

    void retranslateUi(QMainWindow *PlayerStatsWindow)
    {
        PlayerStatsWindow->setWindowTitle(QCoreApplication::translate("PlayerStatsWindow", "MainWindow", nullptr));
        usernameLabel->setText(QCoreApplication::translate("PlayerStatsWindow", "username", nullptr));
        scoreLabel->setText(QCoreApplication::translate("PlayerStatsWindow", "score", nullptr));
        bulletSpeedLabel->setText(QCoreApplication::translate("PlayerStatsWindow", "bullet speed", nullptr));
        reloadTimeLabel->setText(QCoreApplication::translate("PlayerStatsWindow", "reload time", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PlayerStatsWindow: public Ui_PlayerStatsWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLAYERSTATSWINDOW_H
