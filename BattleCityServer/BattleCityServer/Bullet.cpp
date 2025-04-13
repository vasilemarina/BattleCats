#include "Bullet.h"

const float Bullet::DEFAULT_SPEED = 0.25;
const float Bullet::DEFAULT_COOLDOWN = 1.0; 

Bullet::Bullet(int id, const Position& initialPos, const Position& playerPos, float speed)
	: m_id(id),
	m_position(initialPos),
	m_previousPosition(playerPos),
	m_speed(speed),
	m_cooldown(DEFAULT_COOLDOWN),
	m_direction(initialPos.GetX() - playerPos.GetX(), initialPos.GetY() - playerPos.GetY()) {}


Bullet::Bullet(Bullet&& other) noexcept
	: m_id(other.m_id),
	m_position(std::move(other.m_position)),
	m_previousPosition(std::move(other.m_previousPosition)),
	m_speed(other.m_speed),
	m_cooldown(other.m_cooldown),
	m_direction(std::move(other.m_direction)) {}


Bullet& Bullet::operator=(Bullet&& other) noexcept {
	if (this != &other) {
		m_position = std::move(other.m_position);
		m_previousPosition = std::move(other.m_previousPosition);
		m_speed = other.m_speed;
		m_cooldown = other.m_cooldown;
		m_direction = std::move(other.m_direction);
	}
	return *this;
}

void Bullet::Move(const Position& direction) {
	m_previousPosition = m_position;
	m_position += direction;  
}

bool Bullet::HasCollided(const Bullet& otherBullet) const
{
	return m_position.Distance(otherBullet.GetPosition()) == 0;
}

void Bullet::SetSpeed(float newSpeed)
{
	m_speed = newSpeed;
}

float Bullet::GetSpeed() const
{
	return m_speed;
}

void Bullet::SetCooldown(float newCooldown)
{
	m_cooldown = newCooldown;
}

float Bullet::GetCooldown() const
{
	return m_cooldown;
}

int Bullet::GetId() const
{
	return m_id;
}

const Position& Bullet::GetPosition() const
{
	return m_position;
}

Position Bullet::GetDirection() const {
	int dx = m_position.GetX() - m_previousPosition.GetX();
	int dy = m_position.GetY() - m_previousPosition.GetY();
	return Position(dx, dy);  
}

