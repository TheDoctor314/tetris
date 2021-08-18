#include "tetromino.hpp"

namespace tetris {
std::array<sf::Vector2i, 4> get_points(TetriType type, sf::Vector2i top_left) {
    using sf::Vector2i;
    std::array<Vector2i, 4> ret{};

    switch (type) {
    case TetriType::I:
        ret = {
            Vector2i(0, 0),
            Vector2i(1, 0),
            Vector2i(2, 0),
            Vector2i(3, 0),
        };
        break;
    case TetriType::J:
        ret = {
            Vector2i(1, 0),
            Vector2i(1, 1),
            Vector2i(1, 2),
            Vector2i(0, 2),
        };
        break;
    case TetriType::L:
        ret = {
            Vector2i(0, 0),
            Vector2i(0, 1),
            Vector2i(0, 2),
            Vector2i(1, 2),
        };
        break;
    case TetriType::S:
        ret = {
            Vector2i(0, 1),
            Vector2i(1, 1),
            Vector2i(1, 0),
            Vector2i(2, 0),
        };
        break;
    case TetriType::O:
        ret = {
            Vector2i(0, 0),
            Vector2i(0, 1),
            Vector2i(1, 1),
            Vector2i(1, 0),
        };
        break;
    case TetriType::T:
        ret = {
            Vector2i(0, 0),
            Vector2i(1, 0),
            Vector2i(2, 0),
            Vector2i(1, 1),
        };
        break;
    case TetriType::Z:
        ret = {
            Vector2i(0, 0),
            Vector2i(1, 0),
            Vector2i(1, 1),
            Vector2i(2, 1),
        };
        break;
    }

    for (auto &pos : ret) {
        pos += top_left;
    }

    return ret;
}

void Tetromino::update(GameGrid &grid) {
    for (auto [x, y] : m_points) {
        grid.at(x, y) = true;
    }
}

bool Tetromino::move_down(const GameGrid &grid) {
    const int rows = grid.rows();

    for (const auto &point : m_points) {
        if (point.y + 1 == rows) {
            return false;
        }
        if (grid(point.x, point.y + 1)) {
            return false;
        }
    }

    for (auto &point : m_points) {
        point.y += 1;
    }
    return true;
}

void Tetromino::move_left(const GameGrid &grid) {
    for (const auto &point : m_points) {
        if (point.x - 1 < 0) {
            return;
        }

        if (grid(point.x - 1, point.y)) {
            return;
        }
    }

    for (auto &point : m_points) {
        point.x--;
    }
}
void Tetromino::move_right(const GameGrid &grid) {
    const int cols = grid.columns();

    for (const auto &point : m_points) {
        if (point.x + 1 >= cols) {
            return;
        }

        if (grid(point.x + 1, point.y)) {
            return;
        }
    }

    for (auto &point : m_points) {
        point.x++;
    }
}

void Tetromino::draw(sf::RenderWindow &window, sf::Shape &shape) const {
    for (auto &point : m_points) {
        auto x = cell_width * point.x;
        auto y = cell_width * point.y;

        shape.setPosition(x, y);
        window.draw(shape);
    }
}
} // namespace tetris
