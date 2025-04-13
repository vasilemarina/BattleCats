#pragma once
#include "Player.h"

enum class CellType {
	EMPTY,
	BREAKABLE_WALL,
	BOMB_TRIGGER_WALL,
	INDESTRUCTIBLE_WALL,
	PLAYER 

};

class Cell
{
public:
	Cell(CellType m_type);
	void Destroy(); 
	bool IsWalkable() const; 
	void SetType(const CellType& newType);
	CellType GetType() const; 
	bool GetIsDestroyed() const;
	bool TriggersExplosion() const;

private:
	CellType m_type;
	bool m_isDestroyed;

};

