#include "Bomb.h"
Bomb::Bomb(Position pos, double rad, Difficulty difficulty)
    : m_position(pos), m_explosionRadius(rad), m_isExploded(false), m_difficulty(difficulty)
{
}

bool Bomb::GetIsExploded() const
{
    return m_isExploded;
}

void Bomb::Reset(Position newPos)
{
    m_position = newPos;
    m_isExploded = false;
}

bool Bomb::IsInRange(const Position& posTarget) const
{
    return m_position.Distance(posTarget) <= m_explosionRadius;
}

void Bomb::Explode(std::vector<Position>& affectedPositions, Map& map, std::vector<Player*>& players)
{
    for (const Position& pos : affectedPositions)
        if (m_position.Distance(pos) <= m_explosionRadius)
        {
            const Cell& affectedCell = map.GetGrid()[pos.GetX()][pos.GetY()];
            if (affectedCell.GetType() == CellType::BREAKABLE_WALL)
                map.DestroyWall(pos);
            else if (affectedCell.GetType() == CellType::BOMB_TRIGGER_WALL)
            {
                std::vector<Position> affectedPositions;
                for (int row = 0; row < map.GetHeight(); ++row)
                    for (int col = 0; col < map.GetWidth(); ++col)
                    {
                        Position cellPos(row, col);
                        if (pos.Distance(cellPos) <= 10.0)
                            affectedPositions.push_back(cellPos);
                    }

                Bomb bomb(pos, 10.0, m_difficulty);
                map.DestroyWall(pos);
                bomb.Explode(affectedPositions, map, players);
            }
        }

    for (Player* player : players)
        if (player != nullptr && IsInRange(player->GetPosition())) {
            double m_damage = 0.0;
            switch (m_difficulty) {
            case Difficulty::EASY:
                m_damage = 10.0;
                break;
            case Difficulty::MEDIUM:
                m_damage = 20.0;
                break;
            case Difficulty::HARD:
                m_damage = 30.0;
                break;
            case Difficulty::CUSTOM:
                m_damage = 25.0; 
                break;
            }
            player->TakeDamage(m_damage);
        }

    m_isExploded = true;
}

