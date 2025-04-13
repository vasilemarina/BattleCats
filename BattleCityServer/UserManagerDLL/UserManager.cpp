#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include "UserManager.h"
#include <iostream>
#include <sqlite_orm/sqlite_orm.h>

using namespace sqlite_orm;

UserManager::UserManager(const std::string& dbPath)
    : m_storage(sqlite_orm::make_storage(
        dbPath,
        make_table(
            "users",
            make_column("username", &User::username, primary_key()),
            make_column("password", &User::password),
            make_column("score", &User::score)
        ),
        make_table(
            "weapons",
            make_column("username", &DatabaseWeapon::username, primary_key()),
            make_column("bullet_speed", &DatabaseWeapon::bullet_speed),
            make_column("reload_time", &DatabaseWeapon::reload_time),
            make_column("upgrade_count", &DatabaseWeapon::upgrade_count),
            make_column("speed_bonus_applied", &DatabaseWeapon::speed_bonus_applied),
            foreign_key(&DatabaseWeapon::username).references(&User::username)
        )

    )) {
    m_storage.sync_schema(true); 
}

UserManager::~UserManager() {}

bool UserManager::RegisterUser(const std::string& username, const std::string& password, std::string& errorMessage) {
    try {
        if (m_storage.count<User>(where(c(&User::username) == username)) > 0) {
            errorMessage = "User already exists.";
            return false;
        }

        m_storage.replace(User{ username, password, 0 });
        m_storage.replace(DatabaseWeapon{ username });


        return true;
    }
    catch (const std::exception& ex) {
        errorMessage = std::string("Error registering user: ") + ex.what();
        return false;
    }
}

bool UserManager::LoginUser(const std::string& username, const std::string& password, std::string& errorMessage) {
    try {
        auto user = m_storage.get_pointer<User>(username);
        if (!user || user->password != password) {
            errorMessage = "Invalid username or password.";
            return false;
        }
        return true;
    }
    catch (const std::exception& ex) {
        errorMessage = std::string("Error logging in: ") + ex.what();
        return false;
    }
}

bool UserManager::UpgradeWeapon(const std::string& username, std::string& errorMessage) {
    try {
        auto user = m_storage.get_pointer<User>(username);
        if (!user) {
            errorMessage = "User not found.";
            return false;
        }

        auto weapon = m_storage.get_pointer<DatabaseWeapon>(username);
        if (!weapon) {
            errorMessage = "Weapon not found.";
            return false;
        }

        if (user->score < 500 || weapon->upgrade_count >= 4) {
            errorMessage = "Not enough points or max upgrades reached.";
            return false;
        }

        user->score -= 500;
        weapon->reload_time -= 1.0;
        weapon->upgrade_count++;

        m_storage.update(*user);
        m_storage.update(*weapon);

        return true;
    }
    catch (const std::exception& ex) {
        errorMessage = std::string("Error upgrading weapon: ") + ex.what();
        return false;
    }
}

bool UserManager::GetStats(const std::string& username, int& score, double& bulletSpeed, double& reloadTime, std::string& errorMessage) {
    try {
        auto user = m_storage.get_pointer<User>(username);
        auto weapon = m_storage.get_pointer<DatabaseWeapon>(username);

        if (!user || !weapon) {
            errorMessage = "User or weapon not found.";
            return false;
        }

        score = user->score;
        bulletSpeed = weapon->bullet_speed;
        reloadTime = weapon->reload_time;

        return true;
    }
    catch (const std::exception& ex) {
        errorMessage = std::string("Error getting stats: ") + ex.what();
        return false;
    }
}

bool UserManager::UpdateScore(const std::string& username, int newScore, std::string& errorMessage) {
    try {
        auto user = m_storage.get_pointer<User>(username);
        if (!user) {
            errorMessage = "User not found.";
            return false;
        }

        user->score = newScore;
        m_storage.update(*user);
        return true;
    }
    catch (const std::exception& ex) {
        errorMessage = std::string("Error updating score: ") + ex.what();
        return false;
    }
}

bool UserManager::UpdateWeapon(const std::string& username, double newBulletSpeed, double newReloadTime, std::string& errorMessage) {
    try {
        auto weapon = m_storage.get_pointer<DatabaseWeapon>(username);
        if (!weapon) {
            errorMessage = "Weapon not found.";
            return false;
        }

        weapon->bullet_speed = newBulletSpeed;
        weapon->reload_time = newReloadTime;
        m_storage.update(*weapon);
        return true;
    }
    catch (const std::exception& ex) {
        errorMessage = std::string("Error updating weapon: ") + ex.what();
        return false;
    }
}

bool UserManager::UserExists(const std::string& username, std::string& errorMessage) {
    try {
        return m_storage.count<User>(sqlite_orm::where(sqlite_orm::c(&User::username) == username)) > 0;
    }
    catch (const std::exception& ex) {
        errorMessage = std::string("Error checking user existence: ") + ex.what();
        return false;
    }
}

bool UserManager::RegisterWeapon(const std::string& username, double bulletSpeed, double reloadTime, std::string& errorMessage) {
    try {
        m_storage.replace(DatabaseWeapon{ username, bulletSpeed, reloadTime, 0, false });
        return true;
    }
    catch (const std::exception& ex) {
        errorMessage = std::string("Error registering weapon: ") + ex.what();
        return false;
    }
}

