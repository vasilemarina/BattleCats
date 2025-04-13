#include "BulletManager.h"
#include "Logger.h"


int BulletManager::m_newBulletId = 0;

BulletManager::BulletManager(std::vector<Player*>& players, Map& map, Difficulty difficulty)
    : m_players(players), m_map(map), m_difficulty(difficulty)
{
}
void BulletManager::AddBullet(Bullet&& bullet) 
{
    m_activeBullets.emplace_back(std::make_unique<Bullet>(std::move(bullet)));
}

void BulletManager::UpdateBullets()
{
    for (int index = 0; index < m_activeBullets.size();) { 
        m_activeBullets[index]->Move(m_activeBullets[index]->GetDirection());
        bool collisionHandled = HandleBulletCollision(index);

        if (collisionHandled) {
            Logger::Log("Bullet with ID: ", index, " was destroyed due to collision.");
        }
        else {
            ++index;
        }
    }
}

void BulletManager::DestroyBullet(int index)
{
    if (index >= 0 && index < m_activeBullets.size()) {
        m_activeBullets.erase(m_activeBullets.begin() + index);
    }
}

bool BulletManager::HandleBulletCollision(int bulletIndex)
{
    if (bulletIndex < 0 || bulletIndex >= static_cast<int>(m_activeBullets.size())) {
        Logger::Log("HandleBulletCollision: Invalid bullet index: ", bulletIndex);
        return false;
    }

    if (HandleBulletBulletCollision(bulletIndex)) {
        DestroyBullet(bulletIndex);
        return true; 
    }
    if (HandleBulletWallCollision(bulletIndex)) {
        DestroyBullet(bulletIndex);
        return true; 
    }
    if (HandleBulletPlayerCollision(bulletIndex)) {
        DestroyBullet(bulletIndex);
        return true; 
    }

    return false; 
}


bool BulletManager::HandleBulletBulletCollision(int bulletIndex) 
{
    bool foundCollision = false;
    for (int j = 0; j < m_activeBullets.size(); j++) {
        if (bulletIndex != j && m_activeBullets[bulletIndex]->HasCollided(*m_activeBullets[j])) {
            foundCollision = true;
            Logger::Log("Bullet with ID: ", m_activeBullets[bulletIndex]->GetId(), " collided with bullet ", m_activeBullets[j]->GetId());
            DestroyBullet(bulletIndex);
            DestroyBullet(j); 
            break;  
        }
    }
    return foundCollision;
}

bool BulletManager::HandleBulletWallCollision(int bulletIndex) {
    if (bulletIndex < 0 || bulletIndex >= static_cast<int>(m_activeBullets.size())) {
        Logger::Log("HandleBulletWallCollision: Invalid bullet index: ", bulletIndex);
        return false;
    }

    Bullet& bullet = *m_activeBullets[bulletIndex];
    const Position& bulletPos = bullet.GetPosition();

    if (!m_map.IsWithinBounds(bulletPos.GetX(), bulletPos.GetY())) {
        Logger::Log("Bullet is out of bounds and will be destroyed.");
        return true; 
    }

    CellType cellType = m_map.GetType(bulletPos.GetX(), bulletPos.GetY());
    switch (cellType) {
    case CellType::EMPTY:
        return false; 
    case CellType::BREAKABLE_WALL:
        m_map.DestroyWall(bulletPos);
        Logger::Log("Bullet destroyed a breakable wall.");
        return true; 
    case CellType::INDESTRUCTIBLE_WALL:
        Logger::Log("Bullet hit an indestructible wall.");
        return true; 
    case CellType::BOMB_TRIGGER_WALL:
        Logger::Log("Bullet triggered a bomb!");
        m_map.DestroyWall(bulletPos);
        return true; 
    default:
        return false;
    }
}


bool BulletManager::HandleBulletPlayerCollision(int bulletIndex)
{
    bool foundCollision = false;
    for (Player* player : m_players) {
        if (player != nullptr && m_activeBullets[bulletIndex]->GetPosition().Distance(player->GetPosition()) == 0) {
            Logger::Log("Bullet ID: ", m_activeBullets[bulletIndex]->GetId(), " hit player at position (",
                player->GetPosition().GetX(), ", ", player->GetPosition().GetY(), ").");
            foundCollision = true;
            DestroyBullet(bulletIndex);  

            double damage = 20.0;
            player->TakeDamage(damage);

            if (player->IsAlive()) {
                m_map.UpdatePlayerPosition(player->GetPosition(), player->GetInitialPosition());
                player->SetPosition(player->GetInitialPosition());
            }
            break;  
        }
    }
    return foundCollision;
}

const std::vector<std::unique_ptr<Bullet>>& BulletManager::GetActiveBullets() const
{
    return m_activeBullets;
}

const int BulletManager::GetBullets()
{
    return m_activeBullets.size();
}

int BulletManager::GetNewBulletId()
{
    return m_newBulletId++;
}
