#pragma once
import Position; 
#include <utility>

using battlecity::Position;

class Bullet
{
public:
	Bullet(int id, const Position& initialPos, const Position& playerPos, float speed = DEFAULT_SPEED);

	Bullet(Bullet&& other) noexcept;
	Bullet& operator=(Bullet&& other) noexcept;

	Bullet(const Bullet&) = delete;
	Bullet& operator=(const Bullet&) = delete;

	void Move(const Position& direction);  
	bool HasCollided(const Bullet& otherBullet) const;

	void SetSpeed(float);
	float GetSpeed() const;
	const Position& GetPosition() const;

	Position m_previousPosition;
	Position GetDirection() const;

	void SetCooldown(float);
	float GetCooldown() const;
	int GetId() const;

private:
	static const float DEFAULT_COOLDOWN; 
	static const float DEFAULT_SPEED;
	float m_speed;
	Position m_position;
	Position m_direction;
	float m_cooldown;
	int m_id;
};
