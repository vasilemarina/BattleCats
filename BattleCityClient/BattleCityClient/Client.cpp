#include "Client.h"
#include <cpr/cpr.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
Client::Client(const std::string& serverUrl, QObject* parent)
    : QObject(parent), m_serverUrl(serverUrl) {
}

bool Client::CreateMultiplayerGame(std::string& message, std::string& errorMessage)
{
    if (m_currentUsername.empty()) {
        errorMessage = "User is not logged in.";
        return false;
    }

    QJsonObject json;
    json["username"] = QString::fromStdString(m_currentUsername);
    QJsonDocument doc(json);
    std::string body = doc.toJson(QJsonDocument::Compact).toStdString();

    cpr::Response response = cpr::Post(
        cpr::Url{ m_serverUrl + "/create_game" },
        cpr::Header{ {"Content-Type", "application/json"} },
        cpr::Body{ body }
    );

    if (response.status_code == 200) {
        QJsonDocument responseJson = QJsonDocument::fromJson(QString::fromStdString(response.text).toUtf8());
        QJsonObject obj = responseJson.object();
        if (obj["success"].toBool()) {
            if (obj.contains("message") && obj["message"].isString()) {
                message = obj["message"].toString().toStdString();
            }
            else {
                message = "Multiplayer game created successfully.";
            }
            return true;
        }
        else {
            if (obj.contains("message") && obj["message"].isString()) {
                errorMessage = obj["message"].toString().toStdString();
            }
            else {
                errorMessage = "An unknown error occurred.";
            }
            return false;
        }
    }
    else {
        errorMessage = "Failed to create game: HTTP " + std::to_string(response.status_code);
        return false;
    }
}

bool Client::JoinMultiplayerGame(std::string& message, std::string& errorMessage)
{
    if (m_currentUsername.empty()) {
        errorMessage = "User is not logged in.";
        return false;
    }

    QJsonObject json;
    json["username"] = QString::fromStdString(m_currentUsername);
    QJsonDocument doc(json);
    std::string body = doc.toJson(QJsonDocument::Compact).toStdString();

    cpr::Response response = cpr::Post(
        cpr::Url{ m_serverUrl + "/join_game" },
        cpr::Header{ {"Content-Type", "application/json"} },
        cpr::Body{ body }
    );

    if (response.status_code == 200) {
        QJsonDocument responseJson = QJsonDocument::fromJson(QString::fromStdString(response.text).toUtf8());
        QJsonObject obj = responseJson.object();
        if (obj["success"].toBool()) {
            if (obj.contains("message") && obj["message"].isString()) {
                message = obj["message"].toString().toStdString();
            }
            else {
                message = "Joined the game successfully.";
            }
            return true;
        }
        else {
            if (obj.contains("message") && obj["message"].isString()) {
                errorMessage = obj["message"].toString().toStdString();
            }
            else {
                errorMessage = "An unknown error occurred.";
            }
            return false;
        }
    }
    else {
        errorMessage = "Failed to join game: HTTP " + std::to_string(response.status_code);
        return false;
    }
}

bool Client::PollMultiplayerGameStatus(bool& gameStarted, std::string& message, std::string& errorMessage)
{
    if (m_currentUsername.empty()) {
        errorMessage = "User is not logged in.";
        return false;
    }

    QJsonObject json;
    json["username"] = QString::fromStdString(m_currentUsername);
    QJsonDocument doc(json);
    std::string body = doc.toJson(QJsonDocument::Compact).toStdString();

    cpr::Response response = cpr::Post(
        cpr::Url{ m_serverUrl + "/poll_game_status" },
        cpr::Header{ {"Content-Type", "application/json"} },
        cpr::Body{ body }
    );

    if (response.status_code == 200) {
        QJsonDocument responseJson = QJsonDocument::fromJson(QString::fromStdString(response.text).toUtf8());
        QJsonObject obj = responseJson.object();
        if (obj["success"].toBool()) {
            if (obj.contains("game_started") && obj["game_started"].isBool()) {
                gameStarted = obj["game_started"].toBool();
            }
            else {
                gameStarted = false;
            }

            if (obj.contains("message") && obj["message"].isString()) {
                message = obj["message"].toString().toStdString();
            }
            else {
                message = "No message provided.";
            }

            return true;
        }
        else {
            if (obj.contains("message") && obj["message"].isString()) {
                errorMessage = obj["message"].toString().toStdString();
            }
            else {
                errorMessage = "An unknown error occurred.";
            }
            return false;
        }
    }
    else {
        errorMessage = "Failed to poll game status: HTTP " + std::to_string(response.status_code);
        return false;
    }
}

void Client::ProcessServerMessage(const std::string& message) {
    QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(message).toUtf8());
    QJsonObject obj = doc.object();

    QString type = obj["type"].toString();
    if (type == "gameFinished") {
        QVector<QPair<QString, int>> stats;
        QJsonArray statsArray = obj["stats"].toArray();
        for (const auto& stat : statsArray) {
            QJsonObject statObj = stat.toObject();
            QString player = statObj["player"].toString();
            int score = statObj["score"].toInt();
            stats.append({ player, score });
        }

        emit gameFinished(stats);
    }
}



bool Client::GetNewMap(std::string& mapData, std::string& errorMessage) {
    try {
        cpr::Response response = cpr::Get(
            cpr::Url{ m_serverUrl + "/get_new_map" }
        );

        if (response.status_code == 200) {
            mapData = response.text; 
            return true;
        }
        else {
            errorMessage = "Failed to retrieve new map from server. HTTP Status: " + std::to_string(response.status_code);
            return false;
        }
    }
    catch (const std::exception& ex) {
        errorMessage = ex.what();
        return false;
    }
}



bool Client::Login(const std::string& username, const std::string& password, std::string& errorMessage) {
    QJsonObject json;
    json["username"] = QString::fromStdString(username);
    json["password"] = QString::fromStdString(password);
    QJsonDocument doc(json);

    cpr::Response response = cpr::Post(
        cpr::Url{ m_serverUrl + "/login" },
        cpr::Header{ {"Content-Type", "application/json"} },
        cpr::Body{ doc.toJson().toStdString() }
    );

    if (response.status_code == 200) {
        QJsonDocument responseJson = QJsonDocument::fromJson(QString::fromStdString(response.text).toUtf8());
        if (responseJson["success"].toBool()) {
            m_currentUsername = username;

          

            return true;
        }
    }

    errorMessage = "Login failed: " + response.text;
    return false;
}


bool Client::Register(const std::string& username, const std::string& password, std::string& errorMessage) {
    QJsonObject json;
    json["username"] = QString::fromStdString(username);
    json["password"] = QString::fromStdString(password);
    QJsonDocument doc(json);

    cpr::Response response = cpr::Post(
        cpr::Url{ m_serverUrl + "/register" },
        cpr::Header{ {"Content-Type", "application/json"} },
        cpr::Body{ doc.toJson().toStdString() }
    );

    if (response.status_code == 201) {
        return true;
    }
    else {
        errorMessage = "Registration failed: " + response.text;
        return false;
    }
}

bool Client::GetMap(std::string& mapData, std::string& errorMessage) {
    try {
        cpr::Response response = cpr::Get(
            cpr::Url{ m_serverUrl + "/get_map" }
        );

        if (response.status_code == 200) {
            mapData = response.text; 
            return true;
        }
        else {
            errorMessage = "Failed to retrieve map from server. HTTP Status: " + std::to_string(response.status_code);
            return false;
        }
    }
    catch (const std::exception& ex) {
        errorMessage = ex.what();
        return false;
    }
}


bool Client::Logout(const std::string& username, std::string& errorMessage)
{
    if (username.empty()) {
        errorMessage = "Username is empty.";
        return false;
    }

    cpr::Response response = cpr::Post(
        cpr::Url{ m_serverUrl + "/logout" },
        cpr::Header{ {"Content-Type", "application/json"} },
        cpr::Body{ R"({"username": ")" + username + R"("})" }
    );

    if (response.status_code != 200) {
        errorMessage = response.error.message;
        return false;
    }

    return true;
}


bool Client::MovePlayer(const std::string& username, const std::string& direction, std::string& errorMessage) {
    QJsonObject json;
    json["username"] = QString::fromStdString(username);
    json["direction"] = QString::fromStdString(direction);
    QJsonDocument doc(json);

    cpr::Response response = cpr::Post(
        cpr::Url{ m_serverUrl + "/move_player" },
        cpr::Header{ {"Content-Type", "application/json"} },
        cpr::Body{ doc.toJson().toStdString() }
    );

    if (response.status_code == 200) {
        return true;
    }
    else {
        errorMessage = response.text;
        return false;
    }
}

bool Client::SendFireEvent(const std::string& username, QJsonObject& bulletData) {
    QJsonObject json;
    json["username"] = QString::fromStdString(username);

    json["x"] = bulletData["x"];
    json["y"] = bulletData["y"];
    json["directionX"] = bulletData["directionX"];
    json["directionY"] = bulletData["directionY"];

    cpr::Response response = cpr::Post(
        cpr::Url{ m_serverUrl + "/shoot" },
        cpr::Header{ {"Content-Type", "application/json"} },
        cpr::Body{ QJsonDocument(json).toJson().toStdString() }
    );

    if (response.status_code == 200) {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response.text.c_str());
        if (jsonDoc.isObject() && jsonDoc.object().contains("bullet")) {
            bulletData = jsonDoc.object()["bullet"].toObject();
            return true;
        }
        else {
            bulletData["error"] = "Missing 'bullet' field in response.";
            return false;
        }
    }
    else {
        bulletData["error"] = QString::fromStdString(response.error.message);
        return false;
    }
}



std::unordered_map<int, QPoint> Client::GetBulletPositions()
{
    std::unordered_map<int, QPoint> bulletPositions;

    cpr::Response response = cpr::Get(cpr::Url{ m_serverUrl + "/get_bullet_positions" });
    if (response.status_code == 200)
    {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response.text.c_str());
        QJsonArray bulletsArray = jsonDoc.object()["bullets"].toArray();

        for (int i = 0; i < bulletsArray.size(); i++)
        {
            QJsonObject bulletObj = bulletsArray[i].toObject();
            bulletPositions.emplace(bulletObj["id"].toInt(), QPoint(bulletObj["x"].toDouble(), bulletObj["y"].toDouble()));
        }
    }

    return bulletPositions;
}

std::string Client::GetUsername() const
{
    return m_currentUsername;
}

std::string Client::GetServerUrl() const
{
    return m_serverUrl;
}

void Client::sendGameSettings(const std::string& settings)
{
    cpr::Response response = cpr::Post(
        cpr::Url{ "http://localhost:18080/start_game" },
        cpr::Body{ settings },
        cpr::Header{ {"Content-Type", "application/json"} }
    );

    if (response.status_code == 200) {
        qDebug() << "Game settings sent successfully.";
    }
    else {
        qDebug() << "Failed to send game settings. Status code:" << response.status_code;
    }
}

