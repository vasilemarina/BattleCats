#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <vector>
#include <Client.h>
#include <Bullet.h>
#include <thread>

enum class Cell {
    EMPTY,
    BREAKABLE_WALL,
    BOMB_TRIGGER_WALL,
    INDESTRUCTIBLE_WALL,
    PLAYER 

};

class Position {
public:
    Position(int x = 0, int y = 0);
    int GetX() const;
    int GetY() const;
    void SetX(int x);
    void SetY(int y);
    double Distance(const Position& position2) const;

private:
    int m_x, m_y;
};

class MapWidget : public QWidget {
    Q_OBJECT

public:
    MapWidget(const std::vector<std::vector<Cell>>& grid, QWidget* parent = nullptr, Client * client = nullptr, const std::string & currentUsername = "");
    ~MapWidget();
    void paintEvent(QPaintEvent* event) override;
    void SetGrid(const std::vector<std::vector<Cell>>& newGrid);
    void UpdatePlayerPosition(const Position& oldPosition, const Position& newPosition);
    void AddBullet(int id, const QPoint& bulletPos);
    void StartBulletsUpdate();
    void StopBulletsUpdate();
    void UpdateBullets();
    void mousePressEvent(QMouseEvent* event) override;

protected:
    void closeEvent(QCloseEvent* event) override;

signals:
    void fireBullet();
    void updateMap();

private:
    std::vector<std::vector<Cell>> m_grid;
    std::vector<Bullet> m_bullets;
    Client* m_client;
    std::string m_currentUsername;
    std::thread m_updateThread;
    bool m_bulletsUpdateActive;

    QPixmap m_playerImage;
    QPixmap m_breakableWallImage;
    QPixmap m_indestructibleWallImage;
    QPixmap m_bombTriggerWallImage;
    QPixmap m_emptyCellImage;
};

#endif // MAPWIDGET_H
