#pragma once

#include <SFML/Graphics.hpp>
#include <array>

#include "grid.hpp"

namespace tetris {
enum class TetriType : uint8_t { I, J, L, O, S, T, Z };

std::array<sf::Vector2i, 4> get_points(TetriType type, int x, int y);

class Tetromino {
  public:
    using Points = std::array<sf::Vector2i, 4>;

    Tetromino(TetriType type, int x, int y)
        : m_points(get_points(type, x, y)) {}

    void update(GameGrid &grid);
    void translate(int x, int y);

    Points &get_blocks() { return m_points; }

  private:
    Points m_points;
};
} // namespace tetris
