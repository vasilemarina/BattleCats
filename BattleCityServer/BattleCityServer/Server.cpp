#include "Server.h"
#include <crow/json.h>
#include "Map.h"
#include "BulletManager.h"
#include "Logger.h"
#include <vector>
#include <cpr/cpr.h>



Server::Server()
    : m_userManager("user.sqlite"),
    m_game(std::make_unique<Game>(1, Difficulty::MEDIUM)) {
    SetupRoutes();
    Logger::Log("Server initialized successfully.");
}

void Server::SetupPlayers() {
    Position initialPosition = m_game->GetMap().GetCorners()[0];
    m_game->AddPlayer(new Player("player1", 100, 3, 0, initialPosition));
}
crow::response Server::HandleStartGame(const crow::request& req) {
    auto body = crow::json::load(req.body);
    if (!body) {
        Logger::Log("HandleStartGame: Invalid JSON received.");
        return crow::response(400, R"({"success": false, "message": "Invalid JSON"})");
    }

    if (!body.has("difficulty")) {
        Logger::Log("HandleStartGame: Difficulty not specified.");
        return crow::response(400, R"({"success": false, "message": "Difficulty not specified"})");
    }

    std::string difficultyStr = body["difficulty"].s();
    Difficulty difficulty;
    uint8_t customBombs = 0;
    double bulletSpeed = 0.0;
    int customLives = 0;
    double explosionRadius = 0.0;
    double bombDamage = 0.0;

    if (difficultyStr == "EASY") {
        difficulty = Difficulty::EASY;
    }
    else if (difficultyStr == "MEDIUM") {
        difficulty = Difficulty::MEDIUM;
    }
    else if (difficultyStr == "HARD") {
        difficulty = Difficulty::HARD;
    }
    else if (difficultyStr == "CUSTOM") {
        difficulty = Difficulty::CUSTOM;
        if (body.has("custom_bombs")) {
            customBombs = static_cast<uint8_t>(body["custom_bombs"].i());
        }
        if (body.has("bullet_speed")) {
            bulletSpeed = body["bullet_speed"].d();
        }
        if (body.has("lives")) {
            customLives = body["lives"].i();
        }
        if (body.has("explosion_radius")) {
            explosionRadius = body["explosion_radius"].d();
        }
        if (body.has("bomb_damage")) {
            bombDamage = body["bomb_damage"].d();
        }
    }
    else {
        Logger::Log("HandleStartGame: Unknown difficulty: ", difficultyStr);
        return crow::response(400, R"({"success": false, "message": "Unknown difficulty"})");
    }

    uint8_t numPlayers = static_cast<uint8_t>(m_connectedUsers.size());
    uint8_t numBombs;

    if (difficulty != Difficulty::CUSTOM) {
        numBombs = Map::GetBombCountForDifficulty(difficultyStr);
    }
    else {
        numBombs = 0;
    }


    Logger::Log("Starting game with difficulty: ", difficultyStr, ", number of bombs: ",(int) numBombs);

    m_game = std::make_unique<Game>(numPlayers, difficulty);
    m_game->SetCustomSettings(customLives, explosionRadius, bombDamage);
    m_game->CreateNewMap(numPlayers, numBombs);

    for (const auto& username : m_connectedUsers) {
        int score;
        double bulletSpeed, reloadTime;
        std::string errorMessage;

        if (!m_userManager.GetStats(username, score, bulletSpeed, reloadTime, errorMessage)) {
            Logger::Log("HandleStartGame: Weapon not found for user '", username, "': ", errorMessage);
            continue;
        }

        const std::vector<Position>& corners = m_game->GetMap().GetCorners();
        if (corners.empty()) {
            Logger::Log("HandleStartGame: No available corners to place player '", username, "'.");
            continue;
        }

        Position initialPosition = corners[m_game->GetPlayers().size() % corners.size()];
        Player* newPlayer = new Player(username, 100, 3, score, initialPosition);
        newPlayer->m_weapon.SetFireDelay(static_cast<int>(reloadTime * 1000));
        m_game->AddPlayer(newPlayer);

        Logger::Log("HandleStartGame: Player '", username, "' added to the game at position (",
            initialPosition.GetX(), ", ", initialPosition.GetY(), ").");
    }

    crow::json::wvalue response;
    response["success"] = true;
    response["message"] = "Game started successfully";
    response["difficulty"] = difficultyStr;
    response["number_of_bombs"] = static_cast<int>(numBombs);
    response["map_dimensions"] = {
        {"height", static_cast<int>(m_game->GetMap().GetHeight())},
        {"width", static_cast<int>(m_game->GetMap().GetWidth())}
    };

    return crow::response(200, response);
}

crow::response Server::HandleRegister(const crow::request& req) {
    auto body = crow::json::load(req.body);
    if (!body) {
        Logger::Log("HandleRegister: Invalid JSON received.");
        return crow::response(400, R"({"success": false, "message": "Invalid JSON"})");
    }

    std::string username = body["username"].s();
    std::string password = body["password"].s();
    std::string errorMessage;

    if (m_userManager.UserExists(username, errorMessage)) {
        return crow::response(400, R"({"success": false, "message": "User already exists"})");
    }

    if (!m_userManager.RegisterUser(username, password, errorMessage)) {
        Logger::Log("HandleRegister: Error registering user '", username, "': ", errorMessage);
        return crow::response(500, R"({"success": false, "message": ")" + errorMessage + R"("})");
    }

    if (!m_userManager.RegisterWeapon(username, 0.25, 4.0, errorMessage)) {
        Logger::Log("HandleRegister: Error registering weapon for user '", username, "': ", errorMessage);
        return crow::response(500, R"({"success": false, "message": ")" + errorMessage + R"("})");
    }

    Logger::Log("HandleRegister: User registered successfully: '", username, "'.");
    return crow::response(201, R"({"success": true, "message": "User registered successfully"})");
}


crow::response Server::HandleLogin(const crow::request& req) {
    auto body = crow::json::load(req.body);
    if (!body) {
        Logger::Log("HandleLogin: Invalid JSON received.");
        return crow::response(400, R"({"success": false, "message": "Invalid JSON"})");
    }

    std::string username = body["username"].s();
    std::string password = body["password"].s();
    std::string errorMessage;

    if (!m_userManager.LoginUser(username, password, errorMessage)) {
        Logger::Log("HandleLogin: Authentication failed for user '", username, "': ", errorMessage);
        return crow::response(401, R"({"success": false, "message": "Invalid username or password"})");
    }

    if (std::find(m_connectedUsers.begin(), m_connectedUsers.end(), username) != m_connectedUsers.end()) {
        Logger::Log("HandleLogin: User '", username, "' is already logged in.");
        return crow::response(400, R"({"success": false, "message": "User already logged in"})");
    }

    const std::vector<Player*>& players = m_game->GetPlayers();

    auto playerIt = std::find_if(players.begin(), players.end(), [&](Player* player) {
        return player->GetName() == username;
        });

    if (playerIt == players.end() || *playerIt == nullptr) {
        const std::vector<Position>& corners = m_game->GetMap().GetCorners();
        if (corners.empty()) {
            Logger::Log("HandleLogin: No available corners to place the new player.");
            return crow::response(500, R"({"success": false, "message": "Game map is not properly initialized."})");
        }

        Position initialPosition = corners[players.size() % corners.size()];

        int score;
        double bulletSpeed, reloadTime;
        if (!m_userManager.GetStats(username, score, bulletSpeed, reloadTime, errorMessage)) {
            Logger::Log("HandleLogin: Weapon not found for user '", username, "': ", errorMessage);
            return crow::response(404, R"({"success": false, "message": "Weapon not found"})");
        }

        Player* newPlayer = new Player(username, 100, 3, score, initialPosition);
        newPlayer->m_weapon.SetFireDelay(static_cast<int>(reloadTime * 1000));
        m_game->AddPlayer(newPlayer);
        m_connectedUsers.push_back(username);

        Logger::Log("HandleLogin: New player '", username, "' added to the game at position (",
            initialPosition.GetX(), ", ", initialPosition.GetY(), ").");
    }
    else {
        Logger::Log("HandleLogin: Player '", username, "' logged in successfully.");
        m_connectedUsers.push_back(username);
    }

    crow::json::wvalue response;
    response["success"] = true;
    response["message"] = "Login successful";

    return crow::response(200, response);
}


crow::response Server::HandleUpgradeWeapon(const crow::request& req) {
    auto body = crow::json::load(req.body);
    if (!body) {
        return crow::response(400, R"({"success": false, "message": "Invalid JSON"})");
    }

    std::string username = body["username"].s();
    std::string errorMessage;

    int score;
    double bulletSpeed, reloadTime;

    if (!m_userManager.GetStats(username, score, bulletSpeed, reloadTime, errorMessage)) {
        return crow::response(404, R"({"success": false, "message": "User or weapon not found"})");
    }
    int maxUpgrades = 4;
    if (score < 500 || maxUpgrades <= 0) {
        return crow::response(400, R"({"success": false, "message": "Not enough points or max upgrades reached"})");
    }

    score -= 500;
    if (reloadTime > 0.0) {
        reloadTime -= 1.0;
    }

    if (!m_userManager.UpdateScore(username, score, errorMessage) ||
        !m_userManager.UpdateWeapon(username, bulletSpeed, reloadTime, errorMessage)) {
        return crow::response(500, R"({"success": false, "message": ")" + errorMessage + R"("})");
    }

    return crow::response(200, R"({"success": true, "message": "Weapon upgraded", "reload_time": )" + std::to_string(reloadTime) + "}");
}


crow::response Server::HandleFinishMatch(const crow::request& req) {
    auto body = crow::json::load(req.body);
    if (!body) {
        return crow::response(400, R"({"success": false, "message": "Invalid JSON"})");
    }

    std::string winner = body["winner"].s();
    std::string second_place = body["second_place"].s();
    std::string errorMessage;

    int winnerScore, secondPlaceScore;
    double winnerBulletSpeed, winnerReloadTime;
    double secondPlaceBulletSpeed, secondPlaceReloadTime;

    if (!m_userManager.GetStats(winner, winnerScore, winnerBulletSpeed, winnerReloadTime, errorMessage)) {
        return crow::response(404, R"({"success": false, "message": "Winner not found"})");
    }

    if (!m_userManager.GetStats(second_place, secondPlaceScore, secondPlaceBulletSpeed, secondPlaceReloadTime, errorMessage)) {
        return crow::response(404, R"({"success": false, "message": "Second place not found"})");
    }

    winnerScore += 2;
    secondPlaceScore += 1;

    if (!m_userManager.UpdateScore(winner, winnerScore, errorMessage) ||
        !m_userManager.UpdateScore(second_place, secondPlaceScore, errorMessage)) {
        return crow::response(500, R"({"success": false, "message": ")" + errorMessage + R"("})");
    }

    if (winnerScore >= 10) {
        winnerBulletSpeed *= 2;
        if (!m_userManager.UpdateWeapon(winner, winnerBulletSpeed, winnerReloadTime, errorMessage)) {
            return crow::response(500, R"({"success": false, "message": ")" + errorMessage + R"("})");
        }
    }

    if (secondPlaceScore >= 10) {
        secondPlaceBulletSpeed *= 2;
        if (!m_userManager.UpdateWeapon(second_place, secondPlaceBulletSpeed, secondPlaceReloadTime, errorMessage)) {
            return crow::response(500, R"({"success": false, "message": ")" + errorMessage + R"("})");
        }
    }

    crow::json::wvalue message;
    message["type"] = "gameFinished";

    crow::json::wvalue::list stats;
    stats.push_back(crow::json::wvalue{ {"player", winner}, {"score", winnerScore} });
    stats.push_back(crow::json::wvalue{ {"player", second_place}, {"score", secondPlaceScore} });
    message["stats"] = std::move(stats);

    
    for (const auto& client : m_connectedUsers) {
        SendToClient(client, message);
    }

    return crow::response(200, R"({"success": true, "message": "Match results processed"})");
}


crow::response Server::HandleGetNewMap(const crow::request& req)
{
    m_game->CreateNewMap(1, 10); 

    auto grid = m_game->GetMap().GetGrid();

    crow::json::wvalue jsonMap;
    jsonMap["height"] = m_game->GetMap().GetHeight();
    jsonMap["width"] = m_game->GetMap().GetWidth();

    crow::json::wvalue::list gridList;

    for (const auto& row : grid) {
        crow::json::wvalue::list rowList;
        for (const auto& cell : row) {
            rowList.emplace_back(static_cast<int>(cell.GetType()));
        }
        gridList.emplace_back(std::move(rowList));
    }

    jsonMap["grid"] = std::move(gridList);

    return crow::response(jsonMap);
}

crow::response Server::HandleGetMap(const crow::request& req)
{
    if (!m_game) {
        return crow::response(400, R"({"success": false, "message": "Game not initialized"})");
    }

    auto grid = m_game->GetMap().GetGrid();

    crow::json::wvalue jsonMap;
    jsonMap["height"] = m_game->GetMap().GetHeight();
    jsonMap["width"] = m_game->GetMap().GetWidth();

    crow::json::wvalue::list gridList;

    for (const auto& row : grid) {
        crow::json::wvalue::list rowList;
        for (const auto& cell : row) {
            rowList.emplace_back(static_cast<int>(cell.GetType()));
        }
        gridList.emplace_back(std::move(rowList));
    }

    jsonMap["grid"] = std::move(gridList);

    return crow::response(jsonMap);
}
crow::response Server::HandleMovePlayer(const crow::request& req)
{

    auto body = crow::json::load(req.body);
    if (!body) {
        std::cerr << "HandleMovePlayer: Invalid JSON received." << std::endl;
        return crow::response(400, R"({"success": false, "message": "Invalid JSON"})");
    }

    std::string username = body["username"].s();
    std::string direction = body["direction"].s();

    const auto& players = m_game->GetPlayers();
    auto playerIt = std::find_if(players.begin(), players.end(), [&](Player* player) {
        return player->GetName() == username;
        });

    if (playerIt == players.end() || *playerIt == nullptr) {
        std::cerr << "HandleMovePlayer: Player '" << username << "' not found." << std::endl;
        return crow::response(404, R"({"success": false, "message": "Player not found"})");
    }

    Player* player = *playerIt;
    if (!player) {
        std::cerr << "HandleMovePlayer: Player '" << username << "' is null." << std::endl;
        return crow::response(500, R"({"success": false, "message": "Internal server error"})");
    }

    Direction dir;
    if (direction == "UP") dir = Direction::UP;
    else if (direction == "DOWN") dir = Direction::DOWN;
    else if (direction == "LEFT") dir = Direction::LEFT;
    else if (direction == "RIGHT") dir = Direction::RIGHT;
    else {
        std::cerr << "HandleMovePlayer: Invalid direction '" << direction << "' received." << std::endl;
        return crow::response(400, R"({"success": false, "message": "Invalid direction"})");
    }

    player->SetDirection(dir);

    Position newPosition = player->GetPosition() + m_game->GetMovement().at(dir);
    const Map& map = m_game->GetMap();
    if (map.IsWithinBounds(newPosition.GetX(), newPosition.GetY()) &&
        map.GetGrid()[newPosition.GetX()][newPosition.GetY()].IsWalkable()) {
        m_game->UpdatePlayerPosition(player->GetPosition(), newPosition);
        player->SetPosition(newPosition);
        std::cout << "HandleMovePlayer: Player '" << username << "' moved to ("
            << newPosition.GetX() << ", " << newPosition.GetY() << ")." << std::endl;
    }
    else {
        std::cerr << "HandleMovePlayer: Invalid move by player '" << username << "' to ("
            << newPosition.GetX() << ", " << newPosition.GetY() << ")." << std::endl;
    }

    crow::json::wvalue response;
    response["success"] = true;
    response["new_position"] = {
        {"x", player->GetPosition().GetX()},
        {"y", player->GetPosition().GetY()}
    };

    return crow::response(response);
}
crow::response Server::HandleGetStats(const crow::request& req) {
    auto body = crow::json::load(req.body);
    if (!body) {
        return crow::response(400, R"({"success": false, "message": "Invalid JSON"})");
    }

    std::string username = body["username"].s();
    int score;
    double bulletSpeed, reloadTime;
    std::string errorMessage;

    if (!m_userManager.GetStats(username, score, bulletSpeed, reloadTime, errorMessage)) {
        return crow::response(404, R"({"success": false, "message": ")" + errorMessage + R"("})");
    }

    crow::json::wvalue response;
    response["success"] = true;
    response["username"] = username;
    response["score"] = score;
    response["bullet_speed"] = bulletSpeed;
    response["reload_time"] = reloadTime;

    return crow::response(response);
}

crow::response Server::HandleShoot(const crow::request& req)
{
    auto body = crow::json::load(req.body);
    if (!body) {
        return crow::response(400, R"({"success": false, "message": "Invalid JSON"})");
    }

    std::string username = body["username"].s();

    const std::vector<Player*>& players = m_game->GetPlayers();
    auto playerIt = std::find_if(players.begin(), players.end(), [&](Player* player) {
        return player->GetName() == username;
        });

    if (playerIt == players.end() || *playerIt == nullptr) {
        return crow::response(404, R"({"success": false, "message": "Player not found"})");
    }

    Player* player = *playerIt;

    std::optional<std::string> shootingError = player->m_weapon.CanShoot();
    if (shootingError) {
        return crow::response(403, R"({"success": false, "message": ")" + shootingError.value() + R"("})");
    }

    player->m_weapon.Use();  

    Bullet newBullet{ BulletManager::GetNewBulletId(), m_game->GetMovement().at(player->GetDirection()) + player->GetPosition(), player->GetPosition() };

  
    Position bulletPosition = newBullet.GetPosition();

    bool bulletDestroyed = !m_game->AddBullet(std::move(newBullet));

    crow::json::wvalue response;
    response["success"] = true;

    if (bulletDestroyed) {
        response["bullet"] = {
            {"id", newBullet.GetId()},
            {"x", -1},
            {"y", -1},
            {"destroyed", true}
        };
    }
    else {
        response["bullet"] = {
            {"id", newBullet.GetId()},
            {"x", bulletPosition.GetX()},
            {"y", bulletPosition.GetY()},
            {"destroyed", false}
        };
    }

    return crow::response(response);
}




crow::response Server::HandleGetBulletPositions(const crow::request& req)
{
    m_game->Run();  

    crow::json::wvalue response;
    std::vector<crow::json::wvalue> bulletArray;
    const std::vector<std::unique_ptr<Bullet>>& bullets = m_game->GetActiveBullets();

    for (int i = 0; i < bullets.size(); ++i)
        bulletArray.push_back(crow::json::wvalue{
            {"id", bullets[i]->GetId()},
            {"x", bullets[i]->GetPosition().GetX()},
            {"y", bullets[i]->GetPosition().GetY()},
            });

    response["bullets"] = std::move(bulletArray);

    return crow::response(200, response);
}

crow::response Server::HandleLogout(const crow::request& req)
{
    auto body = crow::json::load(req.body);
    if (!body) {
        std::cerr << "HandleLogout: Invalid JSON received." << std::endl;
        return crow::response(400, R"({"success": false, "message": "Invalid JSON"})");
    }

    std::string username = body["username"].s();

    auto it = std::remove_if(m_connectedUsers.begin(), m_connectedUsers.end(),
        [&](const std::string& user) -> bool {
            if (user == username) {
                Logger::Log("Player '", username, "' has been logged out.");
                return true;
            }
            return false;
        });

    bool removed = it != m_connectedUsers.end();
    if (removed) {
        m_connectedUsers.erase(it, m_connectedUsers.end());
        Logger::Log("HandleLogout: Player '", username, "' has been removed from connected users.");
        return crow::response(200, R"({"success": true, "message": "Player logged out successfully"})");
    }
    else {
        std::cerr << "HandleLogout: Player '" << username << "' not found." << std::endl;
        return crow::response(404, R"({"success": false, "message": "Player not found"})");
    }
}

crow::response Server::HandleCheckPlayers(const crow::request& req)
{
    crow::json::wvalue response;
    response["connected_players"] = static_cast<int>(m_game->GetPlayers().size());
    response["required_players"] = 2;

    if (m_game->GetPlayers().size() > 1 && m_game->GetPlayers().size() <= 4) {
        response["status"] = "waiting";
        response["message"] = "Waiting for more players to join.";
    }
    else {
        response["status"] = "ready";
        response["message"] = "All players are ready.";
    }

    return crow::response(response);
}


crow::response Server::HandleStartMultiplayerGame(const crow::request& req) {
    auto body = crow::json::load(req.body);
    if (!body) {
        Logger::Log("HandleStartMultiplayerGame: Invalid JSON received.");
        return crow::response(400, R"({"success": false, "message": "Invalid JSON"})");
    }

    if (!body.has("difficulty")) {
        return crow::response(400, R"({"success": false, "message": "Difficulty not specified"})");
    }

    std::string difficultyStr = body["difficulty"].s();
    Difficulty difficulty;
    uint8_t customBombs = 0;

    if (difficultyStr == "EASY") {
        difficulty = Difficulty::EASY;
    }
    else if (difficultyStr == "MEDIUM") {
        difficulty = Difficulty::MEDIUM;
    }
    else if (difficultyStr == "HARD") {
        difficulty = Difficulty::HARD;
    }
    else if (difficultyStr == "CUSTOM") {
        difficulty = Difficulty::CUSTOM;
        if (body.has("custom_bombs")) {
            int bombs = body["custom_bombs"].i();
            if (bombs < 1 || bombs > 100) {
                return crow::response(400, R"({"success": false, "message": "Invalid number of bombs. Must be between 1 and 100."})");
            }
            customBombs = static_cast<uint8_t>(bombs);
        }
        else {
            return crow::response(400, R"({"success": false, "message": "Custom bombs not specified"})");
        }
    }
    else {
        return crow::response(400, R"({"success": false, "message": "Unknown difficulty"})");
    }

    uint8_t numPlayers = static_cast<uint8_t>(m_connectedUsers.size());
    uint8_t numBombs;

    
   
    if (difficulty != Difficulty::CUSTOM) {
        numBombs = Map::GetBombCountForDifficulty(difficultyStr);
    }
    else {
        numBombs = 0;
    }
    if (numPlayers < 2) {
        Logger::Log("HandleStartMultiplayerGame: Not enough players to start multiplayer game.");
        return crow::response(400, R"({"success": false, "message": "Not enough players to start the multiplayer game. Waiting for more players."})");
    }

    Logger::Log("Starting multiplayer game with difficulty: ", difficultyStr, ", number of bombs: ", numBombs);

    m_game = std::make_unique<Game>(numPlayers, difficulty);
    m_game->SetCustomSettings(0, 0.0, 0.0);
    m_game->CreateNewMap(numPlayers, numBombs);

    for (const auto& username : m_connectedUsers) {
        int score;
        double bulletSpeed, reloadTime;
        std::string errorMessage;

        if (!m_userManager.GetStats(username, score, bulletSpeed, reloadTime, errorMessage)) {
            Logger::Log("HandleStartMultiplayerGame: Weapon not found for user '", username, "': ", errorMessage);
            continue;
        }

        const std::vector<Position>& corners = m_game->GetMap().GetCorners();
        if (corners.empty()) {
            Logger::Log("HandleStartMultiplayerGame: No available corners to place player '", username, "'.");
            continue;
        }

        Position initialPosition = corners[m_game->GetPlayers().size() % corners.size()];
        Player* newPlayer = new Player(username, 100, 3, score, initialPosition);
        newPlayer->m_weapon.SetFireDelay(static_cast<int>(reloadTime * 1000));
        m_game->AddPlayer(newPlayer);

        Logger::Log("HandleStartMultiplayerGame: Player '", username, "' added to the game at position (",
            initialPosition.GetX(), ", ", initialPosition.GetY(), ").");
    }

    crow::json::wvalue response;
    response["success"] = true;
    response["message"] = "Multiplayer game started successfully";
    response["difficulty"] = difficultyStr;
    response["number_of_bombs"] = static_cast<int>(numBombs);
    response["map_dimensions"] = {
        {"height", static_cast<int>(m_game->GetMap().GetHeight())},
        {"width", static_cast<int>(m_game->GetMap().GetWidth())}
    };

    return crow::response(200, response);
}


crow::response Server::HandleCreateGame(const crow::request& req)
{
    auto body = crow::json::load(req.body);
    if (!body || !body.has("username")) {
        Logger::Log("HandleCreateGame: Invalid JSON or missing username.");
        return crow::response(400, R"({"success": false, "message": "Invalid JSON or missing username."})");
    }

    std::string username = body["username"].s();

    if (m_multiplayerGameActive) {
        Logger::Log("HandleCreateGame: A multiplayer game is already active or waiting.");
        return crow::response(400, R"({"success": false, "message": "A multiplayer game is already active or waiting for players."})");
    }

    m_multiplayerPlayers.clear();
    m_multiplayerPlayers.push_back(username);
    m_multiplayerGameActive = true;

    Logger::Log("HandleCreateGame: Multiplayer game created by '", username, "'. Waiting for another player to join.");

    crow::json::wvalue response;
    response["success"] = true;
    response["message"] = "Multiplayer game created. Waiting for another player to join.";

    return crow::response(200, response);
}


bool Server::InitializeGame(Difficulty difficulty, uint8_t numBombs, std::string& errorMessage)
{

    if (m_connectedUsers.size() < 1) {
        errorMessage = "Not enough players to start the game.";
        Logger::Log("InitializeGame: ", errorMessage);
        return false;
    }

    Logger::Log("Initializing game with difficulty: ",
        (difficulty == Difficulty::EASY) ? "EASY" :
        (difficulty == Difficulty::MEDIUM) ? "MEDIUM" :
        (difficulty == Difficulty::HARD) ? "HARD" : "CUSTOM",
        ", number of bombs: ", static_cast<int>(numBombs));

    m_game = std::make_unique<Game>(static_cast<int>(m_connectedUsers.size()), difficulty);
    m_game->SetCustomSettings(0, 0.0, 0.0);
    m_game->CreateNewMap(static_cast<int>(m_connectedUsers.size()), numBombs);

    for (const auto& username : m_connectedUsers) {
        int score;
        double bulletSpeed, reloadTime;
        std::string userError;

        if (!m_userManager.GetStats(username, score, bulletSpeed, reloadTime, userError)) {
            Logger::Log("InitializeGame: Weapon not found for user '", username, "': ", userError, ". Skipping player.");
            continue;
        }

        const std::vector<Position>& corners = m_game->GetMap().GetCorners();
        if (corners.empty()) {
            Logger::Log("InitializeGame: No available corners to place player '", username, "'. Skipping player.");
            continue;
        }

        Position initialPosition = corners[m_game->GetPlayers().size() % corners.size()];
        Player* newPlayer = new Player(username, 100, 3, score, initialPosition);
        newPlayer->m_weapon.SetFireDelay(static_cast<int>(reloadTime * 1000));
        m_game->AddPlayer(newPlayer);

        Logger::Log("InitializeGame: Player '", username, "' added to the game at position (",
            initialPosition.GetX(), ", ", initialPosition.GetY(), ").");
    }

    if (m_game->GetPlayers().empty()) {
        errorMessage = "No players were added to the game.";
        Logger::Log("InitializeGame: ", errorMessage);
        return false;
    }

    return true;
}

void Server::SendToClient(const std::string& client, const crow::json::wvalue& message) {
    std::ostringstream os;
    os << message.dump();
    std::string jsonMessage = os.str();

    Logger::Log("Serialized JSON Message: ", jsonMessage);

    cpr::Response response = cpr::Post(
        cpr::Url{ client },
        cpr::Header{ {"Content-Type", "application/json"} },
        cpr::Body{ jsonMessage }
    );

    if (response.status_code != 200) {
        Logger::Log("Failed to send message to client: ", client, " - Status: ", response.status_code);
    }
    else {
        Logger::Log("Message sent to client: ", client);
    }
}



crow::response Server::HandleJoinGame(const crow::request& req)
{
    auto body = crow::json::load(req.body);
    if (!body || !body.has("username")) {
        Logger::Log("HandleJoinGame: Invalid JSON or missing username.");
        return crow::response(400, R"({"success": false, "message": "Invalid JSON or missing username."})");
    }

    std::string username = body["username"].s();

    if (!m_multiplayerGameActive) {
        Logger::Log("HandleJoinGame: No active multiplayer game to join.");
        return crow::response(400, R"({"success": false, "message": "No active multiplayer game to join. Please create one first."})");
    }

    if (m_multiplayerPlayers.size() >= 2) { 
        Logger::Log("HandleJoinGame: Multiplayer game is already full.");
        return crow::response(400, R"({"success": false, "message": "Multiplayer game is already full."})");
    }

    if (std::find(m_multiplayerPlayers.begin(), m_multiplayerPlayers.end(), username) != m_multiplayerPlayers.end()) {
        Logger::Log("HandleJoinGame: User '", username, "' is already in the multiplayer game.");
        return crow::response(400, R"({"success": false, "message": "You are already in the multiplayer game."})");
    }

    m_multiplayerPlayers.push_back(username);
    Logger::Log("HandleJoinGame: User '", username, "' joined the multiplayer game. Starting game.");

    Difficulty difficulty = Difficulty::MEDIUM;
	uint8_t numBombs = Map::GetBombCountForDifficulty("MEDIUM");

    std::string errorMessage;
    if (!InitializeGame(difficulty, numBombs, errorMessage)) {
        return crow::response(500, R"({"success": false, "message": ")" + errorMessage + R"("})");
    }

    m_multiplayerGameActive = false;
    m_multiplayerPlayers.clear();

    crow::json::wvalue response;
    response["success"] = true;
    response["message"] = "Multiplayer game started successfully.";
    response["difficulty"] = "MEDIUM"; 
    response["number_of_bombs"] = 6; 
    response["map_dimensions"] = {
        {"height", static_cast<int>(m_game->GetMap().GetHeight())},
        {"width", static_cast<int>(m_game->GetMap().GetWidth())}
    };

    return crow::response(200, response);
}


crow::response Server::HandlePollGameStatus(const crow::request& req) 

{
    auto body = crow::json::load(req.body);
    if (!body || !body.has("username")) {
        Logger::Log("HandlePollGameStatus: Invalid JSON or missing username.");
        return crow::response(400, R"({"success": false, "message": "Invalid JSON or missing username."})");
    }

    std::string username = body["username"].s();

    if (m_game && m_game->GetPlayers().size() >= 2) {
        crow::json::wvalue response;
        response["success"] = true;
        response["game_started"] = true;
        response["message"] = "Game has started.";
        response["difficulty"] = "MEDIUM";
        response["number_of_bombs"] = 6;
        response["map_dimensions"] = {
            {"height", static_cast<int>(m_game->GetMap().GetHeight())},
            {"width", static_cast<int>(m_game->GetMap().GetWidth())}
        };
        return crow::response(200, response);
    }

    if (m_multiplayerGameActive && std::find(m_multiplayerPlayers.begin(), m_multiplayerPlayers.end(), username) != m_multiplayerPlayers.end()) {
        crow::json::wvalue response;
        response["success"] = true;
        response["game_started"] = false;
        response["message"] = "Waiting for another player to join.";
        return crow::response(200, response);
    }
    crow::json::wvalue response;
    response["success"] = false;
    response["message"] = "No active game associated with the user.";
    return crow::response(400, response);
}

void Server::SetupRoutes() {
    CROW_ROUTE(m_app, "/register").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) { return HandleRegister(req); });

    CROW_ROUTE(m_app, "/login").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) { return HandleLogin(req); });

    CROW_ROUTE(m_app, "/upgrade_weapon").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) { return HandleUpgradeWeapon(req); });

    CROW_ROUTE(m_app, "/finish_match").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) { return HandleFinishMatch(req); });

    CROW_ROUTE(m_app, "/get_new_map").methods(crow::HTTPMethod::GET)
        ([this](const crow::request& req) { return HandleGetNewMap(req); });

    CROW_ROUTE(m_app, "/get_map").methods(crow::HTTPMethod::GET)
        ([this](const crow::request& req) { return HandleGetMap(req); });

    CROW_ROUTE(m_app, "/move_player").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) { return HandleMovePlayer(req); });

    CROW_ROUTE(m_app, "/get_stats").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) { return HandleGetStats(req); });

    CROW_ROUTE(m_app, "/shoot").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) { return HandleShoot(req); });

    CROW_ROUTE(m_app, "/get_bullet_positions").methods(crow::HTTPMethod::GET)
        ([this](const crow::request& req) { return HandleGetBulletPositions(req); });

    CROW_ROUTE(m_app, "/logout").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) { return HandleLogout(req); });

    CROW_ROUTE(m_app, "/check_players").methods(crow::HTTPMethod::GET)
        ([this](const crow::request& req) { return HandleCheckPlayers(req); });

    CROW_ROUTE(m_app, "/start_game").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) { return HandleStartGame(req); });
    CROW_ROUTE(m_app, "/start_multiplayer_game").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) { return HandleStartMultiplayerGame(req); });

    CROW_ROUTE(m_app, "/create_game").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) { return HandleCreateGame(req); });

    CROW_ROUTE(m_app, "/join_game").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) { return HandleJoinGame(req); });

    CROW_ROUTE(m_app, "/poll_game_status").methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) { return HandlePollGameStatus(req); });
}


void Server::Run() {
    m_app.port(18080).multithreaded().run();
}