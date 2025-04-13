#pragma once
#include <cmath>

class Position
{
public:
	Position(int x = 0, int y = 0);
	int GetX() const;
	int GetY() const;
	void SetX(int x);
	void SetY(int y);

	Position operator+(const Position&)const;
	Position& operator+=(const Position&);
	bool operator==(const Position& other) const;
	double Distance(const Position& position2) const;

private:
	int m_x, m_y;

};

