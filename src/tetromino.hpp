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
    // returns false if it can't move down
    bool move_down(const GameGrid &grid);

    Points &get_blocks() { return m_points; }

    void draw(sf::RenderWindow &window, sf::Shape &shape);

  private:
    Points m_points;
};
} // namespace tetris
