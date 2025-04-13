#pragma once

#include< string>
#include <QPoint>
#include "Direction.h"

class Player
{
public:
    Player(const std::string& name, int health, int lives, int score, int points, const QPoint& position, Direction direction);

    const std::string& GetName() const;
    int GetHealth() const;
    int GetScore() const;
    const QPoint& GetPosition() const;
    Direction GetDirection() const;

private:
    std::string m_name;
    int m_health, m_lives;
    int m_score, m_points;
    QPoint m_position;
    Direction m_direction;
};

