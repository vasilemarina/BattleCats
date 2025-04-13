#include "GameWindow.h"
#include <QKeyEvent>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "ui_GameWindow.h"

GameWindow::GameWindow(Client* client, const std::string& username, QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::GameWindow),
    m_client(client),
    m_username(username),
    m_mapWidget(nullptr) 
{
    ui->setupUi(this);
    this->setFixedSize(800, 600);

    connect(m_client, &Client::gameFinished, this, &GameWindow::onGameFinished);

    std::string errorMessage;
    std::string mapData;
    if (m_client->GetMap(mapData, errorMessage)) {
        qDebug() << "GameWindow: Successfully retrieved map data.";
        auto grid = ParseMap(mapData);
        m_mapWidget = std::make_unique<MapWidget>(grid, this, m_client, m_username);
        m_mapWidget->setWindowTitle("Game Map");
        m_mapWidget->resize(600, 600);
        m_mapWidget->setAttribute(Qt::WA_DeleteOnClose);
        m_mapWidget->show(); 

        if (m_mapWidget) {
            connect(m_mapWidget.get(), &MapWidget::fireBullet, this, &GameWindow::onFireBullet);
            connect(m_mapWidget.get(), &MapWidget::updateMap, this, &GameWindow::UpdateMap);
            qDebug() << "GameWindow: Connected fireBullet signal.";
        }
        else {
            qDebug() << "GameWindow: m_mapWidget is null after initialization.";
            QMessageBox::critical(this, "Critical Error", "Failed to initialize the game map. The application will close.");
            this->close(); 
        }
    }
    else {
        qDebug() << "GameWindow: Failed to retrieve map data. Error:" << QString::fromStdString(errorMessage);
        QMessageBox::warning(this, "Error", QString::fromStdString(errorMessage));
    }

}





GameWindow::~GameWindow()
{
    delete ui; 
}

void GameWindow::keyPressEvent(QKeyEvent* event)
{
    std::string direction;

    switch (event->key()) {
    case Qt::Key_W:
        direction = "UP";
        break;
    case Qt::Key_S:
        direction = "DOWN";
        break;
    case Qt::Key_A:
        direction = "LEFT";
        break;
    case Qt::Key_D:
        direction = "RIGHT";
        break;
    default:
        return; 
    }

    std::string errorMessage;
    if (m_client->MovePlayer(m_username, direction, errorMessage)) {
        UpdateMap();
    }
    else {
        QMessageBox::warning(this, "Error", QString::fromStdString(errorMessage));
    }
}

void GameWindow::UpdateMap()
{
    std::string errorMessage;
    std::string mapData;

    if (!m_client->GetMap(mapData, errorMessage)) {
        QMessageBox::warning(this, "Error", QString::fromStdString(errorMessage));
        return;
    }

    auto grid = ParseMap(mapData);
    m_mapWidget->SetGrid(grid);
    update();
}

std::vector<std::vector<Cell>> GameWindow::ParseMap(const std::string& mapData)
{
    std::vector<std::vector<Cell>> grid;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(QString::fromStdString(mapData).toUtf8());
    QJsonObject jsonObj = jsonDoc.object();

    QJsonArray gridArray = jsonObj.value("grid").toArray();
    for (const auto& row : gridArray) {
        std::vector<Cell> gridRow;
        for (const auto& cell : row.toArray()) {
            gridRow.push_back(static_cast<Cell>(cell.toInt()));
        }
        grid.push_back(gridRow);
    }

    return grid;
}

void GameWindow::onGameFinished(const QVector<QPair<QString, int>>& stats) {
    emit gameFinished("Game Over", stats);
    this->close();
}

void GameWindow::closeEvent(QCloseEvent* event)
{
    std::string errorMessage;
    if (!m_client->Logout(m_username, errorMessage)) {
        qDebug() << "Failed to notify server about logout: " << QString::fromStdString(errorMessage);
    }

    if (m_mapWidget)
        m_mapWidget->StopBulletsUpdate();

    QMainWindow::close();
}

void GameWindow::onUpdateMap()
{
    UpdateMap();
}

void GameWindow::onFireBullet()
{
    QJsonObject bulletData;
    if (m_client->SendFireEvent(m_username, bulletData))
    {
        qDebug() << "Received bulletData:" << QString::fromUtf8(QJsonDocument(bulletData).toJson(QJsonDocument::Compact));
        if (bulletData["destroyed"].toBool() == false) {
            int bulletId = bulletData["id"].toInt();
            int x = bulletData["x"].toInt();
            int y = bulletData["y"].toInt();
            qDebug() << "Bullet ID:" << bulletId << "Position:" << x << "," << y;

            QPoint newBulletPos(x, y);
            if (m_mapWidget) {
                m_mapWidget->AddBullet(bulletId, newBulletPos);
                qDebug() << "Added bullet to mapWidget.";
            }
            else {
                qDebug() << "Error: m_mapWidget is null!";
            }
        }
        else {
            qDebug() << "Bullet was destroyed. Updating map.";
            UpdateMap();
        }
    }
    else {
        qDebug() << "SendFireEvent failed.";
    }
}
