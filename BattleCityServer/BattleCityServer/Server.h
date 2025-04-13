#pragma once
#include "crow.h"
#include <string>
#include "Player.h"
#include "Map.h"
#include "Game.h"
#include <memory>
#include "../UserManagerDLL/UserManager.h"

class Server {

public:
    Server();
    void Run();

private:
    void SetupRoutes();
    void SetupPlayers();
    bool InitializeGame(Difficulty difficulty, uint8_t numBombs, std::string& errorMessage);
    void SendToClient(const std::string& client, const crow::json::wvalue& message);

    crow::response HandleRegister(const crow::request& req);
    crow::response HandleLogin(const crow::request& req);
    crow::response HandleUpgradeWeapon(const crow::request& req);
    crow::response HandleFinishMatch(const crow::request& req);
    crow::response HandleGetNewMap(const crow::request& req);
    crow::response HandleGetMap(const crow::request& req);
    crow::response HandleMovePlayer(const crow::request& req);
    crow::response HandleGetStats(const crow::request& req);
    crow::response HandleShoot(const crow::request& req);
    crow::response HandleGetBulletPositions(const crow::request& req);
    crow::response HandleLogout(const crow::request& req);
    crow::response HandleCheckPlayers(const crow::request& req);
    crow::response HandleStartGame(const crow::request& req);
    crow::response HandleStartMultiplayerGame(const crow::request& req);
    crow::response HandleCreateGame(const crow::request& req);
    crow::response HandleJoinGame(const crow::request& req);
    crow::response HandlePollGameStatus(const crow::request& req);

    crow::SimpleApp m_app;
    std::unique_ptr<Game> m_game;
    std::vector<std::string> m_connectedUsers;
    bool m_multiplayerGameActive = true;
    std::vector<std::string> m_multiplayerPlayers;

    UserManager m_userManager;
};

