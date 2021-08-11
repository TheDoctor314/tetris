#pragma once

#include <SFML/Graphics.hpp>
#include <array>

#include "grid.hpp"

namespace tetris {
enum class TetriType : uint8_t { I, J, L, O, S, T, Z };

std::array<sf::Vector2i, 4> get_points(TetriType type, sf::Vector2i top_left);

class Tetromino {
  public:
    using Points = std::array<sf::Vector2i, 4>;

    Tetromino(TetriType type, sf::Vector2i top_left)
        : m_points(get_points(type, top_left)) {}

    void update(GameGrid &grid);
    void translate(int x, int y);
    // returns false if it can't move down
    bool move_down(const GameGrid &grid);

    void move_left(const GameGrid &grid);
    void move_right(const GameGrid &grid);

    Points &get_blocks() { return m_points; }

    void draw(sf::RenderWindow &window, sf::Shape &shape) const;

  private:
    Points m_points;
};
} // namespace tetris
