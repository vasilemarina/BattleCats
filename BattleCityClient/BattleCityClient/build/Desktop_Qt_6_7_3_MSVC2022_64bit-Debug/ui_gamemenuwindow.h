/********************************************************************************
** Form generated from reading UI file 'GameMenuWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GAMEMENUWINDOW_H
#define UI_GAMEMENUWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GameMenuWindow
{
public:
    QWidget *centralwidget;
    QPushButton *singleplayerButton;
    QPushButton *multiplayerButton;
    QPushButton *statsButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *GameMenuWindow)
    {
        if (GameMenuWindow->objectName().isEmpty())
            GameMenuWindow->setObjectName("GameMenuWindow");
        GameMenuWindow->resize(800, 600);
        centralwidget = new QWidget(GameMenuWindow);
        centralwidget->setObjectName("centralwidget");
        singleplayerButton = new QPushButton(centralwidget);
        singleplayerButton->setObjectName("singleplayerButton");
        singleplayerButton->setGeometry(QRect(320, 325, 175, 35));
        multiplayerButton = new QPushButton(centralwidget);
        multiplayerButton->setObjectName("multiplayerButton");
        multiplayerButton->setGeometry(QRect(320, 365, 175, 35));
        statsButton = new QPushButton(centralwidget);
        statsButton->setObjectName("statsButton");
        statsButton->setGeometry(QRect(320, 405, 175, 35));
        GameMenuWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(GameMenuWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 25));
        GameMenuWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(GameMenuWindow);
        statusbar->setObjectName("statusbar");
        GameMenuWindow->setStatusBar(statusbar);

        retranslateUi(GameMenuWindow);

        QMetaObject::connectSlotsByName(GameMenuWindow);
    } // setupUi

    void retranslateUi(QMainWindow *GameMenuWindow)
    {
        GameMenuWindow->setWindowTitle(QCoreApplication::translate("GameMenuWindow", "MainWindow", nullptr));
        singleplayerButton->setText(QCoreApplication::translate("GameMenuWindow", "Singleplayer", nullptr));
        multiplayerButton->setText(QCoreApplication::translate("GameMenuWindow", "Multiplayer", nullptr));
        statsButton->setText(QCoreApplication::translate("GameMenuWindow", "Stats", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GameMenuWindow: public Ui_GameMenuWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GAMEMENUWINDOW_H
