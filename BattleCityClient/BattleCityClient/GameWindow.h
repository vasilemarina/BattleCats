#pragma once

#include <QMainWindow>
#include <memory>
#include <vector>
#include "Client.h"
#include "MapWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class GameWindow;
}
QT_END_NAMESPACE



class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GameWindow(Client* client, const std::string& username, QWidget* parent = nullptr);
    ~GameWindow();

    static std::vector<std::vector<Cell>> ParseMap(const std::string& mapData);

    void onGameFinished(const QVector<QPair<QString, int>>& stats);
  
signals:
    void gameFinished(const QString& result, const QVector<QPair<QString, int>>& stats);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onFireBullet();
    void onUpdateMap();

private:
    void UpdateMap();

    Ui::GameWindow* ui;
    Client* m_client;
    std::string m_username;
    std::unique_ptr<MapWidget> m_mapWidget; 
};
