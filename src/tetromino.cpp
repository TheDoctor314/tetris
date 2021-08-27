#include "tetromino.hpp"

namespace tetris {
std::array<sf::Vector2i, 4> get_points(TetriType type, sf::Vector2i top_left) {
    using sf::Vector2i;
    std::array<Vector2i, 4> ret{};

    switch (type) {
    case TetriType::I:
        ret = {
            Vector2i(2, 1),
            Vector2i(0, 1),
            Vector2i(1, 1),
            Vector2i(3, 1),
        };
        break;
    case TetriType::J:
        ret = {
            Vector2i(1, 1),
            Vector2i(0, 0),
            Vector2i(0, 1),
            Vector2i(2, 1),
        };
        break;
    case TetriType::L:
        ret = {
            Vector2i(1, 1),
            Vector2i(2, 0),
            Vector2i(0, 1),
            Vector2i(2, 1),
        };
        break;
    case TetriType::S:
        ret = {
            Vector2i(1, 1),
            Vector2i(0, 1),
            Vector2i(1, 0),
            Vector2i(2, 0),
        };
        break;
    case TetriType::O:
        ret = {
            Vector2i(2, 0),
            Vector2i(2, 1),
            Vector2i(1, 1),
            Vector2i(1, 0),
        };
        break;
    case TetriType::T:
        ret = {
            Vector2i(1, 1),
            Vector2i(1, 0),
            Vector2i(0, 1),
            Vector2i(2, 1),
        };
        break;
    case TetriType::Z:
        ret = {
            Vector2i(1, 1),
            Vector2i(0, 0),
            Vector2i(1, 0),
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
        grid.at(x, y) = m_shape;
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

void Tetromino::rotate(const bool clockwise, const GameGrid &grid) {
    if (m_shape == TetriType::O) {
        return;
    }

    auto new_points = m_points;
    // the first point stored in the m_points array is the origin about
    // which we rotate the tetromino.
    auto origin = new_points[0];

    for (std::size_t i = 1; i < new_points.size(); ++i) {
        auto &point = new_points[i];
        auto relative_pos = point - origin;

        if (clockwise) {
            point.x = origin.x + relative_pos.y;
            point.y = origin.y - relative_pos.x;
        } else {
            point.x = origin.x - relative_pos.y;
            point.y = origin.y + relative_pos.x;
        }
    }

    bool can_turn = true;

    for (const auto &[x, y] : new_points) {
        if (x < 0 || x >= static_cast<int>(grid.columns())) {
            can_turn = false;
        }
        if (y < 0 || y >= static_cast<int>(grid.rows())) {
            can_turn = false;
        }
        if (grid(x, y)) {
            can_turn = false;
        }
    }

    if (can_turn) {
        m_points = std::move(new_points);
    }
}

void Tetromino::draw(sf::RenderWindow &window, sf::Shape &shape) const {
    shape.setFillColor(get_colour(m_shape));

    for (auto &point : m_points) {
        auto x = CELL_WIDTH * point.x;
        auto y = CELL_WIDTH * point.y;

        shape.setPosition(x, y);
        window.draw(shape);
    }
}
} // namespace tetris
