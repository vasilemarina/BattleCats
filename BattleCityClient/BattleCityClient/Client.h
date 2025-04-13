#pragma once
#include <string>
#include <QJsonObject>
#include <QJsonArray>
#include <QPoint>
#include <QString>
#include <QObject>

class Client : public QObject {
    Q_OBJECT
public:
    Client(const std::string& serverUrl, QObject* parent = nullptr);

    bool Login(const std::string& username, const std::string& password, std::string& errorMessage);
    bool Register(const std::string& username, const std::string& password, std::string& errorMessage);
    bool GetMap(std::string& mapData, std::string& errorMessage);
    bool GetNewMap(std::string& mapData, std::string& errorMessage);
    bool MovePlayer(const std::string& username, const std::string& direction, std::string& errorMessage);
    bool SendFireEvent(const std::string& username, QJsonObject& bulletData);
    std::unordered_map<int, QPoint>  GetBulletPositions();
    bool Logout(const std::string& username, std::string& errorMessage);
    std::string GetUsername() const;
    std::string GetServerUrl() const;
    void sendGameSettings(const std::string& settings); 
    bool CreateMultiplayerGame(std::string& message, std::string& errorMessage);
    bool JoinMultiplayerGame(std::string& message, std::string& errorMessage);
    bool PollMultiplayerGameStatus(bool& gameStarted, std::string& message, std::string& errorMessage);
    
    void ProcessServerMessage(const std::string& message);

signals:
    void gameFinished(const QVector<QPair<QString, int>>& stats);

private:
    std::string m_serverUrl;
    std::string m_currentUsername;
};
