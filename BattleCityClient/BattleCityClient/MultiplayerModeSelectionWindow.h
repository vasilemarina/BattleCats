#ifndef MULTIPLAYERMODESELECTIONWINDOW_H
#define MULTIPLAYERMODESELECTIONWINDOW_H

#include "MultiplayerModeSelectionWindow.h"
#include "ui_MultiplayerModeSelectionWindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QPainter>
#include "Client.h"
class GameMenuWindow;




namespace Ui {
class MultiplayerModeSelectionWindow;
}

class MultiplayerModeSelectionWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MultiplayerModeSelectionWindow(Client* client, GameMenuWindow* parentWindow, QWidget* parent = nullptr);
    ~MultiplayerModeSelectionWindow();

protected:
    void paintEvent(QPaintEvent* event) override;

signals:
    void windowClosed();

private slots:
    void on_btnCreateGame_clicked();
    void on_btnJoinGame_clicked();
    void on_btnBack_clicked();
    void handleDifficultyWindowClosed();
    void handleGameStart();

private:
    Ui::MultiplayerModeSelectionWindow *ui;
    Client* m_client;
    GameMenuWindow* m_parentWindow;
    QString m_gameID;
};

#endif // MULTIPLAYERMODESELECTIONWINDOW_H
