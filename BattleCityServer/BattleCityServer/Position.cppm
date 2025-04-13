export module Position;

import std;

export namespace battlecity {
    export class Position {
    public:
        Position(int x = 0, int y = 0) : m_x(x), m_y(y) {}

        int GetX() const { return m_x; }
        int GetY() const { return m_y; }
        void SetX(int x) { m_x = x; }
        void SetY(int y) { m_y = y; }
        
        
        double Distance(const Position& other) const {
            return std::sqrt(std::pow(m_x - other.m_x, 2) + std::pow(m_y - other.m_y, 2));
        }

        Position operator+(const Position& pos) const {
            return Position{ m_x + pos.m_x, m_y + pos.m_y };
        }

        Position& operator+=(const Position& pos) {
            m_x += pos.m_x;
            m_y += pos.m_y;
            return *this;
        }

        bool operator==(const Position& other) const {
            return m_x == other.m_x && m_y == other.m_y;
        }

    private:
        int m_x;
        int m_y;
    };

    export std::ostream& operator<<(std::ostream& os, const Position& position) {
        os << "(" << position.GetX() << ", " << position.GetY() << ")";
        return os;
    }
}
