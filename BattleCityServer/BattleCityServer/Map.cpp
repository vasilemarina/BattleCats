#include "Map.h"
#include "Logger.h"


const uint8_t Map::MINIMUM_RANGE = 5;
const uint8_t Map::MAXIMUM_RANGE = 30;

Map::Map(uint8_t numPlayers, uint8_t numberBombs)
	: m_mapHeight(MINIMUM_RANGE),
	m_mapWidth(MINIMUM_RANGE),
	m_numPlayers(numPlayers),
	m_numberOfBombs(numberBombs),
	m_map_storage(initMapStorage("map_settings.sqlite")) 
{
	m_map_storage.sync_schema();
	InitializeBombSettings();

	GenerateRandomMapDimensions();
	InitializeMap();

	PlaceBombs(m_numberOfBombs);

	m_corners.resize(4);
	m_corners[0] = { 0, 0 };
	m_corners[1] = { 0, m_mapWidth - 1 };
	m_corners[2] = { m_mapHeight - 1, 0 };
	m_corners[3] = { m_mapHeight - 1, m_mapWidth - 1 };
}

Map::Map(uint8_t numPlayers, Difficulty difficulty)
	: m_mapHeight(MINIMUM_RANGE),
	m_mapWidth(MINIMUM_RANGE),
	m_numPlayers(numPlayers),
	m_numberOfBombs(0), 
	m_map_storage(initMapStorage("map_settings.sqlite")) 
{
	m_map_storage.sync_schema();
	InitializeBombSettings();

	GenerateRandomMapDimensions();
	InitializeMap();

	std::string difficultyStr;
	switch (difficulty) {
	case Difficulty::EASY:
		difficultyStr = "EASY";
		break;
	case Difficulty::MEDIUM:
		difficultyStr = "MEDIUM";
		break;
	case Difficulty::HARD:
		difficultyStr = "HARD";
		break;
	case Difficulty::CUSTOM:
		difficultyStr = "CUSTOM";
		break;
	default:
		difficultyStr = "MEDIUM";
		break;
	}

	if (difficulty != Difficulty::CUSTOM) {
		m_numberOfBombs = GetBombCountForDifficulty(difficultyStr);
	}
	else {
		m_numberOfBombs = 0;
	}

	if (difficulty != Difficulty::CUSTOM) {
		PlaceBombs(m_numberOfBombs);
	}

	m_corners.resize(4);
	m_corners[0] = { 0, 0 };
	m_corners[1] = { 0, m_mapWidth - 1 };
	m_corners[2] = { m_mapHeight - 1, 0 };
	m_corners[3] = { m_mapHeight - 1, m_mapWidth - 1 };
}

Map::Map()
	: m_mapHeight(MINIMUM_RANGE),
	m_mapWidth(MINIMUM_RANGE),
	m_numPlayers(0),
	m_numberOfBombs(0),
	m_map_storage(initMapStorage("map_settings.sqlite")) 
{
	m_map_storage.sync_schema();
	InitializeBombSettings();

	InitializeMap();
}

Map::~Map()
{
}

void Map::InitializeBombSettings()
{
	auto bombSettings = m_map_storage.get_all<BombSetting>();
	if (bombSettings.empty()) {
		m_map_storage.replace(BombSetting{ "EASY", 3 });
		m_map_storage.replace(BombSetting{ "MEDIUM", 6 });
		m_map_storage.replace(BombSetting{ "HARD", 10 });
		std::cout << "Initialized default bomb settings.\n";
	}
}

int Map::GetBombCountForDifficulty(const std::string& difficulty)
{
	auto storage = initMapStorage("map_settings.sqlite");
	storage.sync_schema();

	auto settings = storage.get_all<BombSetting>(
		sqlite_orm::where(sqlite_orm::c(&BombSetting::difficulty) == difficulty)
	);

	if (!settings.empty()) {
		return settings[0].bomb_count;
	}

	std::cerr << "Bomb count for difficulty '" << difficulty << "' not found. Using default of 6.\n";
	return 6; // Default value
}



void Map::GenerateRandomMapDimensions()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> distrib(MINIMUM_RANGE, MAXIMUM_RANGE);
	m_mapHeight = distrib(gen);
	m_mapWidth = distrib(gen);
}

void Map::InitializeMap()
{
	ResizeMap();
	PlaceRandomWalls();
	PlacePlayers();
}

void Map::SetHeight(const uint8_t& height)
{
	m_mapHeight = height;
	InitializeMap();
}

void Map::SetWidth(const uint8_t& width)
{
	m_mapWidth = width;
	InitializeMap();
}

void Map::PlacePlayers()
{
	if (m_numPlayers >= 1) m_grid[0][0] = Cell(CellType::PLAYER);
	if (m_numPlayers >= 2) m_grid[0][m_mapWidth - 1] = Cell(CellType::PLAYER);
	if (m_numPlayers >= 3) m_grid[m_mapHeight - 1][0] = Cell(CellType::PLAYER);
	if (m_numPlayers == 4) m_grid[m_mapHeight - 1][m_mapWidth - 1] = Cell(CellType::PLAYER);
}

void Map::ResizeMap()
{
	m_grid.resize(m_mapHeight, std::vector<Cell>(m_mapWidth, Cell(CellType::EMPTY)));
}

void Map::PlaceRandomWalls() {
	PlaceWalls(CellType::INDESTRUCTIBLE_WALL, 10);
	PlaceWalls(CellType::BREAKABLE_WALL, 20);
}
void Map::PlaceWalls(CellType wallType, uint8_t percentageLimit) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> distRow(0, m_mapHeight - 1);
	std::uniform_int_distribution<int> distCol(0, m_mapWidth - 1);

	uint8_t maximumWallLimit = static_cast<uint8_t>(m_mapHeight * m_mapWidth * (percentageLimit / 100.0));

	std::vector<Position> wallPositions;
	wallPositions.reserve(maximumWallLimit);

	std::generate_n(std::back_inserter(wallPositions), maximumWallLimit, [&]() {
		uint8_t i, j;
		do {
			i = distRow(gen);
			j = distCol(gen);
		} while (IsCorner(i, j) || m_grid[i][j].GetType() != CellType::EMPTY);
		return Position{ i, j };
		});

	for (const auto& pos : wallPositions) {
		m_grid[pos.GetX()][pos.GetY()] = Cell(wallType);
	}
}


void Map::PlaceBombs(int noOfBombs)
{
	uint8_t totalBreakableWalls = std::ranges::count_if(m_grid | std::views::join, [](const Cell& cell) {
		return cell.GetType() == CellType::BREAKABLE_WALL;
		});
	int maxAllowedBombs = static_cast<int>(totalBreakableWalls * 0.7);

	if (noOfBombs > maxAllowedBombs) {
		Logger::Log("The number of bombs requested (", noOfBombs,
			") exceeds the limit (", maxAllowedBombs, "). Adjusting to maximum.");
		noOfBombs = maxAllowedBombs;
	}
	Logger::Log("Number of bombs to place: ", noOfBombs);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> distRow(0, m_mapHeight - 1);
	std::uniform_int_distribution<int> distCol(0, m_mapWidth - 1);

	m_numberOfBombs = noOfBombs;
	int bombCounter = 0;

	while (bombCounter < noOfBombs) {
		uint8_t i = distRow(gen);
		uint8_t j = distCol(gen);

		if (!IsCorner(i, j) && m_grid[i][j].GetType() == CellType::BREAKABLE_WALL) {
			m_grid[i][j] = Cell(CellType::BOMB_TRIGGER_WALL);
			bombCounter++;
		}
	}
}


uint8_t Map::GetHeight() const
{
	return m_mapHeight;
}

uint8_t Map::GetWidth() const
{
	return m_mapWidth;
}

CellType Map::GetType(const uint8_t& i, const uint8_t& j) const
{
	return m_grid[i][j].GetType();
}

bool Map::IsWithinBounds(uint8_t x, uint8_t y) const
{
	return (x >= 0 && x < m_mapHeight && y >= 0 && y < m_mapWidth);
}

bool Map::IsCorner(uint8_t x, uint8_t y) const
{
	return ((x == 0 && y == 0) ||
		(x == 0 && y == m_mapWidth - 1) ||
		(x == m_mapHeight - 1 && y == 0) ||
		(x == m_mapHeight - 1 && y == m_mapWidth - 1));
}

const std::vector<std::vector<Cell>>& Map::GetGrid() const
{
	return m_grid;
}

const std::vector<Position>& Map::GetCorners() const
{
	return m_corners;
}

void Map::DisplayMap(const std::vector<Bullet>& activeBullets)
{
	std::cout << static_cast<int>(m_mapHeight) << ' ' << static_cast<int>(m_mapWidth) << '\n';

	for (uint8_t i = 0; i < m_mapHeight; ++i) {
		for (uint8_t j = 0; j < m_mapWidth; ++j) {
			bool isBullet = false;
			for (const Bullet& bullet : activeBullets)
				if (bullet.GetPosition().Distance(Position{ i, j }) == 0 && GetType(i, j) == CellType::EMPTY)
				{
					std::cout << '*';
					isBullet = true;
					break;
				}

			if (!isBullet)
				switch (GetType(i, j)) {
				case CellType::PLAYER:
					std::cout << 'P';
					break;
				case CellType::INDESTRUCTIBLE_WALL:
					std::cout << '=';
					break;
				case CellType::BREAKABLE_WALL:
					std::cout << '-';
					break;
				case CellType::BOMB_TRIGGER_WALL:
					std::cout << 'B';
					break;
				case CellType::EMPTY:
					std::cout << ' ';
					break;
				}
		}
		std::cout << '\n';
	}
}

void Map::UpdatePlayerPosition(Position oldPosition, Position newPosition)
{
	m_grid[oldPosition.GetX()][oldPosition.GetY()] = CellType::EMPTY;
	m_grid[newPosition.GetX()][newPosition.GetY()] = CellType::PLAYER;
}

void Map::DestroyWall(Position wallPosition) {
	Cell& cell = m_grid[wallPosition.GetX()][wallPosition.GetY()];
	if (!cell.GetIsDestroyed()) {
		cell.Destroy();
		Logger::Log("Wall destroyed at position: (", wallPosition.GetX(), ", ", wallPosition.GetY(), ")");
	}
}

void Map::RemovePlayer(const Position& playerPosition)
{
	m_grid[playerPosition.GetX()][playerPosition.GetY()].Destroy();
}


