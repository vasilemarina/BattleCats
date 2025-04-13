#include "Weapon.h"
#include "Logger.h"
#include <iostream>

const uint16_t Weapon::INIT_FIRE_DELAY = 4000;

Weapon::Weapon(const std::string& name, int damage, int ammo)
    : m_name(name), m_damage(damage), m_ammo(ammo), m_fireDelay(INIT_FIRE_DELAY), m_lastShotTime(0) {}

void Weapon::Use() {
    --m_ammo;
    Logger::Log("Used ", m_name, ". Ammo left: ", m_ammo);
    m_lastShotTime = GetCurrentTime();
}

bool Weapon::HasAmmo() const {
    return m_ammo > 0;
}

std::optional<std::string> Weapon::CanShoot() const
{
    std::optional<std::string> error;
    if (m_ammo <= 0)
        error = "Out of ammo!";


    if (error.has_value())
        Logger::Log(error.value());
    return error;
}

std::string Weapon::GetName() const {
    return m_name;
}

int Weapon::GetDamage() const {
    return m_damage;
}

int Weapon::GetAmmo() const {
    return m_ammo;
}

int Weapon::GetFireDelay() const
{
    return m_fireDelay;
}

void Weapon::SetFireDelay(int newDelay)
{
    m_fireDelay = newDelay;
}

long Weapon::GetCurrentTime() const
{
    auto currentTime = std::chrono::steady_clock::now();
    auto millisCurrentTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch());
    return millisCurrentTime.count();
}
