#include "MapWidget.h"
#include <QPainter>

MapWidget::MapWidget(const std::vector<std::vector<Cell>>& grid, QWidget* parent, Client* client, const std::string& currentUsername)
    : QWidget(parent), m_grid(grid), m_client(client), m_currentUsername(currentUsername) 
{
    setAttribute(Qt::WA_TranslucentBackground);
    m_playerImage = QPixmap(":/resources/resources/player.png");
    m_breakableWallImage = QPixmap(":/resources/resources/breakable_wall.png");
    m_indestructibleWallImage = QPixmap(":/resources/resources/indestructible_wall.png");
    m_emptyCellImage = QPixmap(":/resources/resources/empty_cell.png");
    
    StartBulletsUpdate();
}

MapWidget::~MapWidget()
{
    if(m_bulletsUpdateActive)
        StopBulletsUpdate();
}

void MapWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    int rows = m_grid.size();
    int cols = m_grid[0].size();

    int cellWidth = width() / cols;
    int cellHeight = height() / rows;
    int cellSize = std::min(cellWidth, cellHeight);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            QRect rect(j * cellSize, i * cellSize, cellSize, cellSize);
            
            switch (m_grid[i][j]) {
            case Cell::EMPTY:
                painter.drawPixmap(rect, m_emptyCellImage.scaled(cellSize, cellSize));
                break;
            case Cell::PLAYER:
                painter.drawPixmap(rect, m_playerImage.scaled(cellSize, cellSize));
                break;
            case Cell::INDESTRUCTIBLE_WALL:
                painter.drawPixmap(rect, m_indestructibleWallImage.scaled(cellSize, cellSize));
                break;
            case Cell::BREAKABLE_WALL:
                painter.drawPixmap(rect, m_breakableWallImage.scaled(cellSize, cellSize));
                break;
            case Cell::BOMB_TRIGGER_WALL:
                painter.drawPixmap(rect, m_breakableWallImage.scaled(cellSize, cellSize));
                break;
            }
            painter.drawRect(rect);

        }
    }

    for (const auto& bullet : m_bullets)
        bullet.Draw(painter, cellSize);
}

void MapWidget::SetGrid(const std::vector<std::vector<Cell>>& newGrid) {
    m_grid = newGrid;
    update();
}

void MapWidget::UpdatePlayerPosition(const Position& oldPosition, const Position& newPosition) {
	m_grid[oldPosition.GetX()][oldPosition.GetY()] = Cell::EMPTY; 
	m_grid[newPosition.GetX()][newPosition.GetY()] = Cell::PLAYER;
	update(); 
}

void MapWidget::AddBullet(int id, const QPoint& bulletPos)
{
    m_bullets.emplace_back(id, bulletPos);
    update();
}

void MapWidget::StartBulletsUpdate()
{
    m_bulletsUpdateActive = true;
    m_updateThread = std::thread(&MapWidget::UpdateBullets, this);
}

void MapWidget::StopBulletsUpdate()
{
    m_bulletsUpdateActive = false;
    if (m_updateThread.joinable())
        m_updateThread.join();
}

void MapWidget::UpdateBullets()
{
    while (m_bulletsUpdateActive)
    {
        std::unordered_map<int, QPoint> updatedBullets = m_client->GetBulletPositions();

        for (int i = m_bullets.size() - 1; i >= 0; i--)
            if (auto it = updatedBullets.find(m_bullets[i].GetId()); it == updatedBullets.end())
                m_bullets.erase(m_bullets.begin() + i);
            else
                m_bullets[i].SetPosition(it->second);

        emit updateMap();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void MapWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        emit fireBullet();
}

void MapWidget::closeEvent(QCloseEvent* event)
{
    if (m_client && !m_currentUsername.empty()) {
        std::string errorMessage;
        qDebug() << "Attempting to log out user:" << QString::fromStdString(m_currentUsername);

        if (!m_client->Logout(m_currentUsername, errorMessage)) {
            qDebug() << "Logout failed:" << QString::fromStdString(errorMessage);
        }
        else {
            qDebug() << "User logged out successfully.";
        }
    }

    event->accept();
}



