#pragma once
#include <iostream>
#include <vector>

enum class Cell {
	EMPTY,
	BREAKABLE_WALL,
	BOMB_TRIGGER_WALL,
	INDESTRUCTIBLE_WALL,
	PLAYER 

};

class Map
{
public:
	Map(uint8_t height, uint8_t width, const std::vector<std::vector<Cell>>& grid);

	uint8_t GetHeight() const;
	uint8_t GetWidth() const;
private:
	uint8_t m_height;
	uint8_t m_width;
	std::vector<std::vector<Cell>> m_grid;
};

