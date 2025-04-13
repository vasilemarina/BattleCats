#include "Cell.h"

Cell::Cell(CellType m_type) :m_type(m_type), m_isDestroyed(false)
{

}

void Cell::Destroy()
{
	if (m_type == CellType::BREAKABLE_WALL || m_type == CellType::BOMB_TRIGGER_WALL || m_type == CellType::PLAYER)
	{
		m_type = CellType::EMPTY;
		m_isDestroyed = true;
	}

}

bool Cell::IsWalkable() const
{
	return m_type == CellType::EMPTY;
}

void Cell::SetType(const CellType& newType)
{
	m_type = newType;
}

CellType Cell::GetType() const
{
	return m_type;
}

bool Cell::GetIsDestroyed() const
{
	return m_isDestroyed;
}

bool Cell::TriggersExplosion() const
{
	return m_type == CellType::BOMB_TRIGGER_WALL;
}
