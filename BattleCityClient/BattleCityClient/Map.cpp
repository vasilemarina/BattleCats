#include "Map.h"

Map::Map(uint8_t height, uint8_t width, const std::vector<std::vector<Cell>>& grid)
	:
	m_grid(grid),
	m_height(height),
	m_width(width)
{
}
