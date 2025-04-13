#pragma once
#include <iostream>
#include <vector>
#include <random>
#include <ranges>
#include <string> 
#include "Cell.h"
import Position;
#include "Difficulty.h"
#include "Bullet.h" 
#include <sqlite_orm/sqlite_orm.h> 

using battlecity::Position;


struct BombSetting {
    std::string difficulty; 
    int bomb_count;
};

inline auto initMapStorage(const std::string& filename) {
    return sqlite_orm::make_storage(
        filename,
        sqlite_orm::make_table(
            "bomb_settings",
            sqlite_orm::make_column("difficulty", &BombSetting::difficulty, sqlite_orm::primary_key()),
            sqlite_orm::make_column("bomb_count", &BombSetting::bomb_count)
        )
    );
}

class Map {
public:
    static const uint8_t MINIMUM_RANGE;
    static const uint8_t MAXIMUM_RANGE;

    Map(uint8_t numPlayers, uint8_t numberBombs);
    Map(uint8_t numPlayers, Difficulty difficulty);
    Map();
    ~Map();

    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;

    Map(Map&&) noexcept = default;
    Map& operator=(Map&&) noexcept = default;

    void GenerateRandomMapDimensions();
    void InitializeMap();
    void SetHeight(const uint8_t& height);
    void SetWidth(const uint8_t& width);
    void PlacePlayers();
    void ResizeMap();
    void PlaceRandomWalls();
    void PlaceWalls(CellType wallType, uint8_t percentageLimit);
    void PlaceBombs(int noOfBombs);
    uint8_t GetHeight() const;
    uint8_t GetWidth() const;
    CellType GetType(const uint8_t& i, const uint8_t& j) const;
    bool IsWithinBounds(uint8_t x, uint8_t y) const;
    bool IsCorner(uint8_t x, uint8_t y) const;
    const std::vector<std::vector<Cell>>& GetGrid() const;
    const std::vector<Position>& GetCorners() const;
    void DisplayMap(const std::vector<Bullet>& activeBullets);
    void UpdatePlayerPosition(Position oldPosition, Position newPosition);
    void DestroyWall(Position wallPosition);
    void RemovePlayer(const Position& playerPosition);
   static int GetBombCountForDifficulty(const std::string& difficulty); 
private:
    uint8_t m_mapHeight;
    uint8_t m_mapWidth;
    uint8_t m_numPlayers;
    uint8_t m_numberOfBombs;

    std::vector<std::vector<Cell>> m_grid;
    std::vector<Position> m_corners;

    decltype(initMapStorage("")) m_map_storage;

    void InitializeBombSettings(); 
    
};
