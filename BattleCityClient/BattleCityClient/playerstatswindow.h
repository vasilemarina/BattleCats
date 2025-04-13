#ifndef PLAYERSTATSWINDOW_H
#define PLAYERSTATSWINDOW_H

#include <QMainWindow>
#include <QString>
#include "Client.h"

namespace Ui {
class PlayerStatsWindow;
}

class PlayerStatsWindow : public QMainWindow
{
    Q_OBJECT

public:
    PlayerStatsWindow(Client* client, QWidget *parent = nullptr);
    ~PlayerStatsWindow();

    void setPlayerStats(const QString& nickname, int score, double bulletSpeed, double reloadTime);

private slots:
    void on_btnClose_clicked();
    void on_btnUpgradeReloadTime_clicked();

private:
    Ui::PlayerStatsWindow* ui;
    Client* m_client;

signals:
    void statsUpdated();
};

#endif // PLAYERSTATSWINDOW_H
