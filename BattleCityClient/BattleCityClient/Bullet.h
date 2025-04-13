#pragma once
#include <QPoint>
#include <QPainter>

class Bullet
{
public:
	Bullet(int id, const QPoint& position);

	const QPoint& GetPosition() const;
	void SetPosition(const QPoint& newPosition);
	void Draw(QPainter& painter, int cellSize) const;
	int GetId() const;

private:
	QPoint m_position;
	int m_id;
};
