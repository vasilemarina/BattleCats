#pragma once
#include <string>
#include <chrono>
#include <optional>
#include "Bullet.h"

class Weapon {
public:
    Weapon(const std::string& name, int damage, int ammo);

    void Use();
    bool HasAmmo() const;
    std::optional<std::string> CanShoot() const;

    std::string GetName() const;
    int GetDamage() const;
    int GetAmmo() const;
    int GetFireDelay() const;
    void SetFireDelay(int newDelay);
private:
    long GetCurrentTime() const;

    static const uint16_t INIT_FIRE_DELAY;
    std::string m_name;
    int m_damage;
    int m_ammo;
    int m_fireDelay;
    long m_lastShotTime;
};