#include "Position.h"

Position::Position(int x, int y) : m_x(x), m_y(y)
{
}

int Position::GetX() const
{
	return m_x;
}

int Position::GetY() const
{
	return m_y;
}

void Position::SetX(int x)
{
	m_x = x;
}

void Position::SetY(int y)
{
	m_y = y;
}

Position Position::operator+(const Position& pos)const
{
	return Position{ m_x + pos.m_x, m_y + pos.m_y };
}

Position& Position::operator+=(const Position& pos)
{
	*this = *this + pos;
	return *this;
}

bool Position::operator==(const Position& other) const
{
	return m_x == other.m_x && m_y == other.m_y;
}

double Position::Distance(const Position& position2) const
{
	return std::sqrt(std::pow(m_x - position2.m_x, 2) + std::pow(m_y - position2.m_y, 2));
}


