#include "Player.h"

Player::Player(const std::string& name, int health, int lives, int score, int points, const QPoint& position, Direction direction)
	:
	m_name(name),
	m_health(health),
	m_lives(lives),
	m_score(score),
	m_points(points),
	m_position(position),
	m_direction(direction)
{
}

const std::string& Player::GetName() const
{
	return m_name;
}

int Player::GetHealth() const
{
	return m_health;
}

int Player::GetScore() const
{
	return m_score;
}

const QPoint& Player::GetPosition() const
{
	return m_position;
}

Direction Player::GetDirection() const
{
	return m_direction;
}