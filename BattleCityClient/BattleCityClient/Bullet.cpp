#include "Bullet.h"

Bullet::Bullet(int id, const QPoint& position)
	:
	m_id(id),
	m_position{ position }
{
}
const QPoint& Bullet::GetPosition() const
{
	return m_position;
}

void Bullet::SetPosition(const QPoint& newPosition)
{
	m_position = newPosition;
}

void Bullet::Draw(QPainter& painter, int cellSize) const
{
	painter.setBrush(Qt::black);
	painter.drawEllipse(QPoint(m_position.y() * cellSize + cellSize / 2, m_position.x() * cellSize + cellSize / 2), 5, 5);
}

int Bullet::GetId() const
{
	return m_id;
}