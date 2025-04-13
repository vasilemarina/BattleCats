#pragma once
#include "Player.h"

class Bot : public Player
{
public:
    Bot(const std::string& name, int health, int lives, int score, const Position& position);

    void PerformAction(); // Comportamentul botului la fiecare pas de joc

private:
    void MoveRandomly();
    void ChasePlayer(const Position& playerPosition);
};

