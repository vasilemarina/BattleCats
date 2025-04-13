#include "Game.h"
#include <algorithm> 
#include "Logger.h"

const uint8_t Game::MAX_PLAYERS = 4;
const uint8_t Game::KILL_POINTS = 100;
const uint8_t Game::WIN_POINTS = 200;
const uint8_t Game::HALF_BULLETS_DELAY_PRICE = 500;
const uint8_t Game::HALF_BULLETS_DELAY_LIMIT = 4;
const uint8_t Game::DOUBLE_BULLETS_SPEED_PRICE = 10;
const uint8_t Game::DOUBLE_BULLETS_SPEED_LIMIT = 1;

Game::Game(uint8_t playersNumber, Difficulty gameDifficulty)
	: m_playersNumber{ playersNumber },
	m_initialLivesNum{ 2 }, 
	m_initialHealth{ 25 },  
	m_map{ std::make_unique<Map>(playersNumber, gameDifficulty) }, 
	m_bullets{ std::make_unique<BulletManager>(m_players, *m_map, gameDifficulty) },                
	m_isFinished{ false },
	m_difficulty{ gameDifficulty },
	m_movement{
		{Direction::UP,    Position{-1,  0}},
		{Direction::DOWN,  Position{1,  0}},
		{Direction::LEFT,  Position{0, -1}},
		{Direction::RIGHT, Position{0,  1}}
	}
{
	Logger::Log("Game initialized with ",(int) playersNumber," players.");
	const std::vector<Position>& mapCorners = m_map->GetCorners();
}

Game::~Game()
{
	for (Player* p : m_players)
		if (p != nullptr)
			delete p;
	Logger::Log("Game destructor called. All resources have been cleaned up.");
}

void Game::Run()
{
	m_bullets->UpdateBullets();
}

void Game::AddPlayer(Player* newPlayer)
{
	m_players.push_back(newPlayer);
	Logger::Log("Player '", newPlayer->GetName(), "' added to the game.");
}

void Game::UpdatePlayerPosition(Position oldPosition, Position newPosition)
{
	m_map->UpdatePlayerPosition(oldPosition, newPosition);
}

bool Game::RemovePlayer(const std::string& username)
{
	auto playerIt = std::find_if(m_players.begin(), m_players.end(), [&](Player* player) {
		return player->GetName() == username;
		});

	if (playerIt != m_players.end()) 
	{
		Position position = (*playerIt)->GetPosition();
		m_map->RemovePlayer(position);
		delete* playerIt;
		m_players.erase(playerIt);
		Logger::Log("Removing player '", username, "' from the game.");
		return true;
	}
	Logger::Log("Player '", username, "' not found in the game.");
	return false;
}

const std::vector<Player*>& Game::GetPlayers() const
{
	return m_players;
}

const Map& Game::GetMap() const
{
	return *m_map;
}

Difficulty Game::GetDifficulty() const
{
	return m_difficulty;
}

const std::unordered_map<Direction, Position>& Game::GetMovement() const
{
	return m_movement;
}

const std::vector<std::unique_ptr<Bullet>>& Game::GetActiveBullets() const
{
	return m_bullets->GetActiveBullets();
}

void Game::SetCustomSettings(int lives, double radius, double damage)
{
	m_customLives = lives;
	m_customExplosionRadius = radius;
	m_customBombDamage = damage;

	Logger::Log("Game: Custom settings applied - Lives: ", lives, ", Radius: ", radius, ", Damage: ", damage);
}

void Game::UpdatePlayers()
{
	const std::vector<std::vector<Cell>>& m_grid = m_map->GetGrid();

	std::unordered_map<char, std::function<void(Player*)>> actionMap = {
		{'w', [&](Player* player) { player->SetDirection(Direction::UP); }},
		{'a', [&](Player* player) { player->SetDirection(Direction::LEFT); }},
		{'s', [&](Player* player) { player->SetDirection(Direction::DOWN); }},
		{'d', [&](Player* player) { player->SetDirection(Direction::RIGHT); }},
		{'f', [&](Player* player) {
			Bullet newBullet{ BulletManager::GetNewBulletId(), m_movement[player->GetDirection()] + player->GetPosition(), player->GetPosition() };
			m_bullets->AddBullet(std::move(newBullet));
			m_bullets->HandleBulletCollision(m_bullets->GetActiveBullets().size() - 1);
			RemoveEliminatedPlayers();
		}}
	};

	for (Player* player : m_players)
		if (player != nullptr)
		{
			char key;
			Logger::Log("Updating player '", player->GetName(), "'...");
			std::cin >> key;

			auto actionIt = actionMap.find(key);
			if (actionIt != actionMap.end()) {
				actionIt->second(player); 
				if (key != 'f') { 
					Position newPosition = player->GetPosition();
					if (m_movement.find(player->GetDirection()) != m_movement.end()) {
						newPosition += m_movement[player->GetDirection()];

						if (m_map->IsWithinBounds(newPosition.GetX(), newPosition.GetY()) && m_grid[newPosition.GetX()][newPosition.GetY()].IsWalkable()) {
							m_map->UpdatePlayerPosition(player->GetPosition(), newPosition);
							player->SetPosition(newPosition);
						}
					}
				}
			}
			else {
				Logger::Log("Invalid key pressed by player '", player->GetName(), "'.");
			}
		}
}


void Game::RemoveEliminatedPlayers()
{
	auto it = std::remove_if(m_players.begin(), m_players.end(), [&](Player* player) {
		if (player != nullptr && !player->IsAlive()) {
			m_map->RemovePlayer(player->GetPosition());
			Logger::Log("Player '", player->GetName(), "' eliminated.");
			delete player;
			return true; 
		}
		return false;
		});
	m_players.erase(it, m_players.end());
	m_playersNumber = static_cast<uint8_t>(m_players.size());
	Logger::Log("Updated player count: ", m_playersNumber);
}


void Game::CreateNewMap(uint8_t numPlayers, uint8_t numBombs)
{
	m_map = std::make_unique<Map>(numPlayers, numBombs);
	m_bullets = std::make_unique<BulletManager>(m_players, *m_map, m_difficulty); 
	Logger::Log("Created a new map with ", (int)numPlayers, " players and ",(int) numBombs, " bombs.");
}

bool Game::AddBullet(Bullet&& newBullet)
{
	m_bullets->AddBullet(std::move(newBullet)); 
	int bulletsNum = m_bullets->GetActiveBullets().size();
	m_bullets->HandleBulletCollision(bulletsNum - 1);

	RemoveEliminatedPlayers();

	return bulletsNum == m_bullets->GetActiveBullets().size();
}

void Game::EndGame()
{
	m_isFinished = true;

	for (Player* player : m_players)
		if (player != nullptr) {
			player->AddScore(WIN_POINTS);
			Logger::Log("Player '", player->GetName(), "' received WIN_POINTS.");
			break;
		}
}

