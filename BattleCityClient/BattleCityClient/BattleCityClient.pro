TEMPLATE = app
QT += core network
QT += core gui widgets
CONFIG += qt widgets c++17
SOURCES += \
            Bullet.cpp\
            Client.cpp\
            CreateAccountDialog.cpp\
            DifficultySelectionWindow.cpp \
            FinishGameWindow.cpp \
            GameMenuWindow.cpp \
            GameWindow.cpp \
            LoginDialog.cpp\
            MultiplayerModeSelectionWindow.cpp \
            PlayerStatsWindow.cpp \
            main.cpp\
            mainWindow.cpp\
            Map.cpp\
            MapWidget.cpp\
            Player.cpp\
            Position.cpp


HEADERS += \
            Bullet.h\
            Client.h\
            CreateAccountDialog.h\
            DifficultySelectionWindow.h \
            Direction.h\
            FinishGameWindow.h \
            GameMenuWindow.h \
            GameWindow.h \
            LoginDialog.h\
            MultiplayerModeSelectionWindow.h \
            PlayerStatsWindow.h \
            mainWindow.h\
            Map.h\
            MapWidget.h\
            Player.h\
            Position.cppm

 

FORMS +=\
            CreateAccountDialog.ui\
            DifficultySelectionWindow.ui \
    FinishGameWindow.ui \
            GameMenuWindow.ui \
            GameWindow.ui \
            LoginDialog.ui\
            MultiplayerModeSelectionWindow.ui \
            PlayerStatsWindow.ui \
            mainWindow.ui \



