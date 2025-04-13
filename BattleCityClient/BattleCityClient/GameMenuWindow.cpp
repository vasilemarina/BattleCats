#include "GameMenuWindow.h"
#include <QMessageBox>
#include "ui_GameMenuWindow.h"
#include <QDebug>
#include <QPainter>
#include <QMessageBox>
#include "PlayerStatsWindow.h"
#include "mainWindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QJsonValue>
#include <cpr/cpr.h>
#include <iostream>
#include "GameWindow.h"
#include "MultiplayerModeSelectionWindow.h"



GameMenuWindow::GameMenuWindow(Client* client, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::GameMenuWindow)
    , m_client(client)
    , m_currentMode(GameMode::SINGLEPLAYER) 
{
    ui->setupUi(this);
    this->setFixedSize(800, 600);

}


GameMenuWindow::~GameMenuWindow()
{
    delete ui;
}

void GameMenuWindow::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QImage backgroundImage(":/resources/resources/background2.jpg");
    if (backgroundImage.isNull()) {
        qDebug() << "Nu s-a putut incarca imaginea!";
    }
    else {
        QPixmap background = QPixmap::fromImage(backgroundImage);
        painter.drawPixmap(0, 0, width(), height(), background);
    }
}

void GameMenuWindow::closeEvent(QCloseEvent* event)
{
    qDebug() << "GameMenuWindow: Window is closing. Initiating logout.";
    logoutPlayer();
    event->accept(); 

    std::string errorMessage;
    if (!m_client->Logout(m_client->GetUsername(), errorMessage)) {
        qDebug() << "Failed to notify server about logout: " << QString::fromStdString(errorMessage);
    }
}

void GameMenuWindow::on_singleplayerButton_clicked()
{
    if (!m_client) {
        QMessageBox::warning(this, "Error", "Client-ul nu este inițializat.");
        return;
    }
    m_currentMode = GameMode::SINGLEPLAYER;
    m_difficultyWindow = std::make_unique<DifficultySelectionWindow>(m_client, this);
    m_difficultyWindow->setAttribute(Qt::WA_DeleteOnClose); 

    connect(m_difficultyWindow.get(), &DifficultySelectionWindow::difficultySelected,
        this, &GameMenuWindow::handleDifficultySelected);
    connect(m_difficultyWindow.get(), &DifficultySelectionWindow::windowClosed,
        this, &GameMenuWindow::handleDifficultyWindowClosed);

    m_difficultyWindow->show();
    this->hide();
}


void GameMenuWindow::handleDifficultyWindowClosed()
{
    this->show();
    m_difficultyWindow = nullptr;
}

void GameMenuWindow::startGame(Difficulty difficulty, uint8_t customBombs)
{
    QJsonObject json;
    switch (difficulty) {
    case Difficulty::EASY:
        json["difficulty"] = "EASY";
        break;
    case Difficulty::MEDIUM:
        json["difficulty"] = "MEDIUM";
        break;
    case Difficulty::HARD:
        json["difficulty"] = "HARD";
        break;
    case Difficulty::CUSTOM:
        json["difficulty"] = "CUSTOM";
        json["custom_bombs"] = static_cast<int>(customBombs);
        break;
    default:
        json["difficulty"] = "MEDIUM"; 
        break;
    }

    QJsonDocument doc(json);
    QString jsonString = doc.toJson(QJsonDocument::Compact);
    std::string body = jsonString.toStdString();

    cpr::Response response = cpr::Post(
        cpr::Url{ "http://localhost:18080/start_game" },
        cpr::Header{ {"Content-Type", "application/json"} },
        cpr::Body{ body }
    );

    if (response.status_code == 200) {
        cpr::Response mapResponse = cpr::Get(
            cpr::Url{ "http://localhost:18080/get_map" },
            cpr::Header{ {"Content-Type", "application/json"} }
        );

        if (mapResponse.status_code == 200) {
            QJsonDocument jsonResponse = QJsonDocument::fromJson(QString::fromStdString(mapResponse.text).toUtf8());
            QJsonObject jsonObject = jsonResponse.object();

            if (jsonObject.contains("grid")) {
                auto grid = GameWindow::ParseMap(mapResponse.text);

                GameWindow* gameWindow = new GameWindow(m_client, m_client->GetUsername());
                gameWindow->setAttribute(Qt::WA_DeleteOnClose); 

                connect(gameWindow, &QObject::destroyed, this, &GameMenuWindow::show);

                gameWindow->show();

                this->hide(); 
            }
            else {
                QMessageBox::warning(this, "Error", "Map data missing in server response.");
                this->show(); 
            }
        }
        else {
            QMessageBox::warning(this, "Error", "Failed to fetch map: " + QString::fromStdString(mapResponse.error.message));
            this->show(); 
        }
    }
    else {
        QMessageBox::warning(this, "Error", "Failed to start game: " + QString::fromStdString(response.error.message));
        this->show(); 
    }
}






void GameMenuWindow::on_multiplayerButton_clicked()
{
    if (!m_client) {
        QMessageBox::warning(this, "Error", "Client-ul nu este inițializat.");
        return;
    }

    m_currentMode = GameMode::MULTIPLAYER;

    m_multiplayerModeWindow = std::make_unique<MultiplayerModeSelectionWindow>(m_client, this);
    m_multiplayerModeWindow->setAttribute(Qt::WA_DeleteOnClose);
    m_multiplayerModeWindow->show();

    this->hide();
}

void GameMenuWindow::handleDifficultySelected(Difficulty difficulty)
{
    uint8_t customBombs = 0;

    if (difficulty == Difficulty::CUSTOM && m_difficultyWindow) {
        customBombs = static_cast<uint8_t>(m_difficultyWindow->getNumberOfBombs());
    }

    if (m_currentMode == GameMode::SINGLEPLAYER) {
        startGame(difficulty, customBombs);
    }
    else if (m_currentMode == GameMode::MULTIPLAYER) {
        startMultiplayerGame(difficulty, customBombs);
    }
}

void GameMenuWindow::logoutPlayer()
{
    if (m_client && !m_client->GetUsername().empty()) {
        QJsonObject json;
        json["username"] = QString::fromStdString(m_client->GetUsername());

        QJsonDocument doc(json);
        QString jsonString = doc.toJson(QJsonDocument::Compact);
        std::cout << "Client: Sending logout JSON - " << jsonString.toStdString() << std::endl; 
        std::string body = jsonString.toStdString();

        cpr::Response response = cpr::Post(
            cpr::Url{ "http://localhost:18080/logout" },
            cpr::Header{ {"Content-Type", "application/json"} },
            cpr::Body{ body }
        );

        std::cout << "Client: Received logout response with status code " << response.status_code << std::endl; 

        if (response.status_code == 200) {
            qDebug() << "Client: Successfully logged out.";
        }
        else {
            qDebug() << "Client: Failed to logout. Status code:" << response.status_code;
            QJsonDocument errorDoc = QJsonDocument::fromJson(QString::fromStdString(response.text).toUtf8());
            QString errorMessage = "Unknown error.";
            if (errorDoc.isObject() && errorDoc.object().contains("message")) {
                errorMessage = errorDoc.object()["message"].toString();
            }
            qDebug() << "Client: Logout error message:" << errorMessage;
        }
    }
    else {
        qDebug() << "Client: No user is currently logged in.";
    }
}

void GameMenuWindow::startMultiplayerGame(Difficulty difficulty, uint8_t customBombs)
{
    QJsonObject json;
    switch (difficulty) {
    case Difficulty::EASY:
        json["difficulty"] = "EASY";
        break;
    case Difficulty::MEDIUM:
        json["difficulty"] = "MEDIUM";
        break;
    case Difficulty::HARD:
        json["difficulty"] = "HARD";
        break;
    case Difficulty::CUSTOM:
        json["difficulty"] = "CUSTOM";
        json["custom_bombs"] = static_cast<int>(customBombs);
        break;
    default:
        json["difficulty"] = "MEDIUM"; 
        break;
    }

    QJsonDocument doc(json);
    QString jsonString = doc.toJson(QJsonDocument::Compact);
    std::cout << "Client: Sending JSON - " << jsonString.toStdString() << std::endl; 
    std::string body = jsonString.toStdString();

    cpr::Response response = cpr::Post(
        cpr::Url{ "http://localhost:18080/start_multiplayer_game" },
        cpr::Header{ {"Content-Type", "application/json"} },
        cpr::Body{ body }
    );

    std::cout << "Client: Received response with status code " << response.status_code << std::endl; 

    if (response.status_code == 200) {
        cpr::Response mapResponse = cpr::Get(
            cpr::Url{ "http://localhost:18080/get_map" },
            cpr::Header{ {"Content-Type", "application/json"} }
        );

        std::cout << "Client: Received map response with status code " << mapResponse.status_code << std::endl; 

        if (mapResponse.status_code == 200) {
            QJsonDocument jsonResponse = QJsonDocument::fromJson(QString::fromStdString(mapResponse.text).toUtf8());
            QJsonObject jsonObject = jsonResponse.object();

            if (jsonObject.contains("grid")) {
                auto grid = GameWindow::ParseMap(mapResponse.text);

                GameWindow* gameWindow = new GameWindow(m_client, m_client->GetUsername());
                gameWindow->setAttribute(Qt::WA_DeleteOnClose);
                gameWindow->show();

                this->close(); 
            }
            else {
                QMessageBox::warning(this, "Error", "Map data missing in server response.");
                this->show(); 
            }
        }
        else {
            QJsonDocument errorDoc = QJsonDocument::fromJson(QString::fromStdString(mapResponse.text).toUtf8());
            QString errorMessage = "Failed to fetch map.";
            if (errorDoc.isObject()) {
                QJsonObject errorObj = errorDoc.object();
                if (errorObj.contains("message")) {
                    errorMessage = errorObj["message"].toString();
                }
            }
            QMessageBox::warning(this, "Error", "Failed to fetch map: " + errorMessage);
            this->show();
        }
    }
    else {
        QJsonDocument errorDoc = QJsonDocument::fromJson(QString::fromStdString(response.text).toUtf8());
        QString errorMessage = "Failed to start multiplayer game.";
        if (errorDoc.isObject()) {
            QJsonObject errorObj = errorDoc.object();
            if (errorObj.contains("message")) {
                errorMessage = errorObj["message"].toString();
            }
        }
        QMessageBox::warning(this, "Error", "Failed to start multiplayer game: " + errorMessage);
        this->show(); 
    }
}



void GameMenuWindow::on_statsButton_clicked()
{
    if (!m_client) {
        QMessageBox::warning(this, "Error", "Client-ul nu este inițializat.");
        return;
    }

    cpr::Response response = cpr::Post(
        cpr::Url{ "http://localhost:18080/get_stats" },
        cpr::Header{ {"Content-Type", "application/json"} },
        cpr::Body{ R"({"username": ")" + m_client->GetUsername() + R"("})" }
    );

    if (response.status_code == 200) {
        QJsonDocument jsonResponse = QJsonDocument::fromJson(QString::fromStdString(response.text).toUtf8());
        QJsonObject jsonObject = jsonResponse.object();
        if (jsonObject["success"].toBool()) {
            auto statsWindow = new PlayerStatsWindow(m_client, this);
            statsWindow->setPlayerStats(
                jsonObject["username"].toString(),
                jsonObject["score"].toInt(),
                jsonObject["bullet_speed"].toDouble(),
                jsonObject["reload_time"].toDouble()
            );
            statsWindow->show();
        }
        else {
            QMessageBox::warning(this, "Error", jsonObject["message"].toString());
        }
    }
    else {
        QMessageBox::warning(this, "Error", "Failed to fetch stats: " + QString::fromStdString(response.error.message));
    }
}





