#pragma once
#include <string>
#include <format>
import Position;
#include "Weapon.h"
#include "Direction.h"

using battlecity::Position;

class Player {
public:
    Player(const std::string& name, int health, int lives, int score, Position initialPos);
    Player(Player&& other) noexcept;
    Player& operator=(Player&& other) noexcept;
    void TakeDamage(int m_damage);
    bool IsAlive() const;

    std::string GetName() const;
    int GetHealth() const;
    void SetHealth(int);
    int GetScore() const;
    int GetPoints() const;
    void AddPoints(int gainedPoints);
    
    void AddScore(int m_points);
    Direction GetDirection();
    void SetDirection(Direction);
    void SetReloadTime(double reloadTime);


    const Position& GetPosition() const;
    const Position& GetInitialPosition() const;
    void SetPosition(Position);

    void TryUpgradeCooldown();
    void TryDoubleBulletSpeed();
    bool HasScoreDoubledSpeedUsed() const;

    Weapon m_weapon;
private:
    std::string m_name;
    int m_health, m_maxHealth, m_lives;
    int m_score, m_points;
    Position m_position, m_initialPosition;
    uint8_t m_weaponUpgrades;
    bool m_doubleBulletSpeed;
    bool m_scoreDoubledSpeedUsed;
    Direction m_direction;
};
