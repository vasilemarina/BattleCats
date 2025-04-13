#ifndef GAMEMENUWINDOW_H
#define GAMEMENUWINDOW_H

#include <QMainWindow>
#include <memory> 
#include "MapWidget.h"
#include "Client.h"
#include "DifficultySelectionWindow.h"

class MultiplayerModeSelectionWindow;

namespace Ui {
    class GameMenuWindow;
}

enum class GameMode {
    SINGLEPLAYER,
    MULTIPLAYER
};

class GameMenuWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GameMenuWindow(Client* client, QWidget* parent = nullptr);
    ~GameMenuWindow();

protected:
    void paintEvent(QPaintEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private slots:
    void on_singleplayerButton_clicked();
    void on_multiplayerButton_clicked();
    void on_statsButton_clicked();
    void handleDifficultySelected(Difficulty difficulty);
    void handleDifficultyWindowClosed();
    void startMultiplayerGame(Difficulty difficulty, uint8_t customBombs);

private:
    Ui::GameMenuWindow* ui;
    Client* m_client;
    std::unique_ptr<DifficultySelectionWindow> m_difficultyWindow; 
    std::unique_ptr<MultiplayerModeSelectionWindow> m_multiplayerModeWindow; 
    GameMode m_currentMode;

    void startGame(Difficulty difficulty, uint8_t customBombs = 0);
    void logoutPlayer(); 
};

#endif // GAMEMENUWINDOW_H
