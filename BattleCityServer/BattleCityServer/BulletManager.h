#pragma once
#include <vector>
#include "Bullet.h"
#include "Map.h"
#include "Bomb.h"
#include "Difficulty.h"
#include "Logger.h"


class BulletManager
{
public:
	BulletManager(std::vector<Player*>& players, Map& map, Difficulty difficulty);

	BulletManager(const BulletManager&) = delete;
	BulletManager& operator=(const BulletManager&) = delete;

	BulletManager(BulletManager&&) noexcept = default;
	BulletManager& operator=(BulletManager&&) noexcept = default;
	void NotifyUIOfMapUpdate(const std::vector<Position>& destroyedCells);

	void AddBullet(Bullet&& bullet); 
	void UpdateBullets(); 
	void DestroyBullet(int index);
	bool HandleBulletCollision(int bulletIndex);

	const std::vector<std::unique_ptr<Bullet>>& GetActiveBullets() const;
	const int GetBullets();

	static int GetNewBulletId();

private:
	bool HandleBulletBulletCollision(int bulletIndex);	
	bool HandleBulletWallCollision(int bulletIndex);	
	bool HandleBulletPlayerCollision(int bulletIndex);	

	std::vector<std::unique_ptr<Bullet>> m_activeBullets;
	std::vector<Player*>& m_players;
	Map& m_map;
	Difficulty m_difficulty;

	static int m_newBulletId;
};

