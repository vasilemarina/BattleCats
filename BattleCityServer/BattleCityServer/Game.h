#pragma once
#include "Map.h"
#include "BulletManager.h"
#include "Difficulty.h"
#include <format>
#include <unordered_map>
#include <iostream>

class Game
{
public:
	Game(uint8_t playersNumber, Difficulty gameDifficulty);
	~Game();

	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;

	void Run();
	void AddPlayer(Player* );
	void UpdatePlayerPosition(Position oldPosition, Position newPosition);
	bool RemovePlayer(const std::string& username);
	void RemoveEliminatedPlayers();
	void CreateNewMap(uint8_t numPlayers, uint8_t numBombs);
	bool AddBullet(Bullet&& bullet);

	const std::vector<Player*>& GetPlayers() const;
	const Map& GetMap() const;
	Difficulty GetDifficulty() const;
	const std::unordered_map<Direction, Position>& GetMovement() const;
	const std::vector<std::unique_ptr<Bullet>>& GetActiveBullets() const;
	void SetCustomSettings(int lives, double radius, double damage);
	BulletManager& GetBulletManager() { return *m_bullets; }

public:
	void UpdatePlayers();
	void EndGame();

	static const uint8_t MAX_PLAYERS;
	static const uint8_t KILL_POINTS, WIN_POINTS;
	static const uint8_t HALF_BULLETS_DELAY_PRICE;
	static const uint8_t HALF_BULLETS_DELAY_LIMIT;
	static const uint8_t DOUBLE_BULLETS_SPEED_PRICE;
	static const uint8_t DOUBLE_BULLETS_SPEED_LIMIT;

	uint8_t m_playersNumber;
	uint8_t m_initialLivesNum, m_initialHealth;
	std::unique_ptr<Map> m_map;
	std::vector<Player*> m_players;
	std::unique_ptr<BulletManager> m_bullets;
	bool m_isFinished;
	Difficulty m_difficulty;
	std::unordered_map<Direction, Position> m_movement;

	int m_customLives;                
	double m_customExplosionRadius;   
	double m_customBombDamage;
};

