#include "Player.h"
#include "Bullet.h"
#include <iostream>

Player::Player(const std::string& name, int health, int lives, int score, Position initialPos)
    :
    m_name(name), 
    m_health(health),
    m_maxHealth(health),
    m_lives(lives),
    m_score(score),
    m_initialPosition(initialPos),
    m_position(initialPos),
    m_weaponUpgrades(0),
	m_weapon(std::format("{}'s weapon", name), 1, 100), 
    m_doubleBulletSpeed(false),
    m_scoreDoubledSpeedUsed(false),
    m_direction(Direction::RIGHT)
{
    m_points = 0;
}

Player::Player(Player&& other) noexcept
    : m_name(std::move(other.m_name)),
    m_position(std::move(other.m_position)),
    m_initialPosition(std::move(other.m_initialPosition)),
    m_weapon(std::move(other.m_weapon)),
    m_health(other.m_health),
    m_lives(other.m_lives),
    m_score(other.m_score),
    m_weaponUpgrades(other.m_weaponUpgrades),
    m_doubleBulletSpeed(other.m_doubleBulletSpeed),
    m_scoreDoubledSpeedUsed(other.m_scoreDoubledSpeedUsed) {}


Player& Player::operator=(Player&& other) noexcept {
    if (this != &other) {
        m_name = std::move(other.m_name);
        m_position = std::move(other.m_position);
        m_initialPosition = std::move(other.m_initialPosition);
        m_weapon = std::move(other.m_weapon);
        m_health = other.m_health;
        m_lives = other.m_lives;
        m_score = other.m_score;
        m_weaponUpgrades = other.m_weaponUpgrades;
        m_doubleBulletSpeed = other.m_doubleBulletSpeed;
        m_scoreDoubledSpeedUsed = other.m_scoreDoubledSpeedUsed;
    }
    return *this;
}


void Player::TakeDamage(int m_damage) {
    m_health -= m_damage;
    if (m_health <= 0 && m_lives > 0) {
        m_health = m_maxHealth;
        m_lives--;
    }
}

bool Player::IsAlive() const {
    return m_lives > 0;
}

std::string Player::GetName() const {
    return m_name;
}

int Player::GetHealth() const {
    return m_health;
}

void Player::SetHealth(int newHealth) {
    m_health = newHealth;
    if (m_health < 0) {
        m_health = 0;
    }
}

int Player::GetScore() const {
    return m_score;
}

void Player::AddScore(int m_points) {
    m_score += m_points;
    if (m_score < 0) {
        m_score = 0;
    }
}

Direction Player::GetDirection()
{
    return m_direction;
}

void Player::SetDirection(Direction direction)
{
    m_direction = direction;
}

void Player::SetReloadTime(double reloadTime) {
    m_weapon.SetFireDelay(static_cast<int>(reloadTime * 1000)); 
}

const Position& Player::GetPosition() const
{
    return m_position;
}

const Position& Player::GetInitialPosition() const
{
    return m_initialPosition;
}

void Player::SetPosition(Position newPosition)
{
    m_position = newPosition;
}

void Player::TryUpgradeCooldown() {
    if (m_points >= 500 && m_weaponUpgrades < 4) {
        m_weapon.SetFireDelay(m_weapon.GetFireDelay() / 2);
        m_points -= 500;
        m_weaponUpgrades++;
    }
    else if (m_weaponUpgrades >= 4) {
        std::cout << "Maximum weapon upgrades reached!\n";
    }
    else {
        std::cout << "Not enough points to upgrade weapon cooldown!\n";
    }
}

void Player::TryDoubleBulletSpeed() {
    if (!m_scoreDoubledSpeedUsed && m_score >= 10) {
        m_doubleBulletSpeed = true;
        m_scoreDoubledSpeedUsed = true;
    }
    else if (m_scoreDoubledSpeedUsed) {
        std::cout << "Bullet speed already doubled!\n";
    }
    else {
        std::cout << "Not enough score to double bullet speed!\n";
    }
}

bool Player::HasScoreDoubledSpeedUsed() const {
    return m_scoreDoubledSpeedUsed;
}