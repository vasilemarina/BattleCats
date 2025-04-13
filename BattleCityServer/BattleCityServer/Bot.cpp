#include "Bot.h"
#include "Map.h"
#include <cstdlib> 

Bot::Bot(const std::string& name, int health, int lives, int score, const Position& position)
    : Player(name, health, lives, score, position)
{
}

void Bot::PerformAction()
{
    if (rand() % 2 == 0) {
        MoveRandomly();
    }
    else {
        // ChasePlayer(playerPosition);
    }
}

void Bot::MoveRandomly()
{
    int dx = (rand() % 3) - 1; 
    int dy = (rand() % 3) - 1; 

    Position newPosition = GetPosition();
    newPosition.SetX(newPosition.GetX() + dx);
    newPosition.SetY(newPosition.GetY() + dy);

    SetPosition(newPosition);
}

void Bot::ChasePlayer(const Position& playerPosition)
{
    Position currentPosition = GetPosition();
    if (playerPosition.GetX() > currentPosition.GetX()) {
        currentPosition.SetX(currentPosition.GetX() + 1);
    }
    else if (playerPosition.GetX() < currentPosition.GetX()) {
        currentPosition.SetX(currentPosition.GetX() - 1);
    }

    if (playerPosition.GetY() > currentPosition.GetY()) {
        currentPosition.SetY(currentPosition.GetY() + 1);
    }
    else if (playerPosition.GetY() < currentPosition.GetY()) {
        currentPosition.SetY(currentPosition.GetY() - 1);
    }

    SetPosition(currentPosition);
}
