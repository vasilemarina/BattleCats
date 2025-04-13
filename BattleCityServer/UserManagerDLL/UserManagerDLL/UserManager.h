#pragma once
#include <string>
#include <sqlite_orm/sqlite_orm.h>

#ifdef USERMANAGER_EXPORTS
#define USERMANAGER_API __declspec(dllexport)
#else
#define USERMANAGER_API __declspec(dllimport)
#endif



struct User {
    std::string username;
    std::string password;
    int score = 0;
};

struct DatabaseWeapon {
    std::string username;
    double bullet_speed = 0.25;
    double reload_time = 4.0;
    int upgrade_count = 0;
    bool speed_bonus_applied = false;
};

class USERMANAGER_API UserManager {
public:
    UserManager(const std::string& dbPath);
    ~UserManager();

    bool RegisterUser(const std::string& username, const std::string& password, std::string& errorMessage);
    bool LoginUser(const std::string& username, const std::string& password, std::string& errorMessage);
    bool UpgradeWeapon(const std::string& username, std::string& errorMessage);
    bool GetStats(const std::string& username, int& score, double& bulletSpeed, double& reloadTime, std::string& errorMessage);
    bool UpdateScore(const std::string& username, int newScore, std::string& errorMessage);
    bool UpdateWeapon(const std::string& username, double newBulletSpeed, double newReloadTime, std::string& errorMessage);


private:
    using StorageType = decltype(sqlite_orm::make_storage(
        "",
        sqlite_orm::make_table(
            "users",
            sqlite_orm::make_column("username", &User::username, sqlite_orm::primary_key()),
            sqlite_orm::make_column("password", &User::password),
            sqlite_orm::make_column("score", &User::score)
        ),
        sqlite_orm::make_table(
            "weapons",
            sqlite_orm::make_column("username", &DatabaseWeapon::username, sqlite_orm::primary_key()),
            sqlite_orm::make_column("bullet_speed", &DatabaseWeapon::bullet_speed),
            sqlite_orm::make_column("reload_time", &DatabaseWeapon::reload_time),
            sqlite_orm::make_column("upgrade_count", &DatabaseWeapon::upgrade_count),
            sqlite_orm::make_column("speed_bonus_applied", &DatabaseWeapon::speed_bonus_applied),
            sqlite_orm::foreign_key(&DatabaseWeapon::username).references(&User::username)
        )
    ));

    StorageType m_storage;

};
