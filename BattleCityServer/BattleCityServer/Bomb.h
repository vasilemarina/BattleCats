#pragma once
import Position; 
#include "Map.h"
#include <vector>
#include "Player.h"
#include "Difficulty.h"

using battlecity::Position;

class Bomb
{
public:
    Bomb(Position pos, double rad, Difficulty difficulty);

    bool GetIsExploded() const;
    void Reset(Position newPos);
    bool IsInRange(const Position& posTarget) const;
    void Explode(std::vector<Position>& affectedPositions, Map& map, std::vector<Player*>& players);

private:
    Position m_position;
    bool m_isExploded;
    double m_explosionRadius;
    Difficulty m_difficulty;
};
