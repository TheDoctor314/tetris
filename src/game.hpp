#pragma once

#include <SFML/Graphics.hpp>
#include <random>

#include "input.hpp"
#include "tetromino.hpp"

namespace tetris {
struct Game {
    Game()
        : current_mino(get_next_shape(), starting_point)
        , next_shape(get_next_shape()) {}

  public:
    void update();
    void handle_key_up(sf::Keyboard::Key);
    void handle_key_down(sf::Keyboard::Key);
    void draw(sf::RenderWindow &window, sf::Shape &shape, sf::Font &font);

  private:
    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<> dist{0, 6};
    sf::Vector2i starting_point{COLS / 2 - 2, 0};

  public:
    Input input;
    Tetromino current_mino;
    TetriType next_shape;

    bool game_over{false};
    unsigned fall_ticker = 0;
    unsigned move_ticker = 0;
    unsigned clear_effect_timer = 0;

    GameGrid grid;
    std::array<bool, ROWS> lines_to_clear{};

  private:
    auto get_next_shape() -> TetriType {
        return static_cast<TetriType>(dist(rng));
    }
};
} // namespace tetris
