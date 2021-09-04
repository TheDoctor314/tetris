#include "game.hpp"

namespace tetris {
void clear_line(GameGrid &grid, std::size_t line) {
    assert(line < grid.rows());

    for (std::size_t x = 0; x < grid.columns(); ++x) {
        grid(x, line) = {};
    }
}

void shift_lines(GameGrid &grid, const std::size_t line) {
    for (auto y = line; y > 0; --y) {
        for (std::size_t x = 0; x < grid.columns(); ++x) {
            std::swap(grid(x, y), grid(x, y - 1));
        }
    }
}

template<std::size_t SIZE>
void clear_lines(GameGrid &grid, const std::array<bool, SIZE> lines_to_clear) {
    static_assert(ROWS == SIZE);

    for (int y = grid.rows() - 1; y >= 0; --y) {
        if (lines_to_clear[y]) {
            clear_line(grid, y);
            shift_lines(grid, y);
        }
    }
}

void Game::update() {
    if (game_over) {
        return;
    }

    ++fall_ticker;
    ++move_ticker;
    soft_drop_ticker = (soft_drop_ticker + 1) % SOFT_DROP_TICKS;

    if (clear_effect_timer != 0) {
        --clear_effect_timer;

        if (clear_effect_timer == 0) {
            clear_lines(grid, lines_to_clear);
            lines_to_clear = {};
        }
        return;
    }

    if (input.rotate) {
        auto clockwise = input.rotate.value() == Rotate::CW;
        current_mino.rotate(clockwise, grid);
        input.rotate = {};
    }

    if (soft_drop_ticker == 0) {
        if (input.soft_drop_pressed) {
            if (current_mino.move_down(grid)) {
                fall_ticker = 0;
            }
        }
    }

    if (input.hard_drop_pressed) {
        current_mino.update_ghost(grid);
        current_mino.hard_drop();
    }

    if (fall_ticker == FALL_TICKS) {
        if (!current_mino.move_down(grid)) {

            for (const auto &block : current_mino.get_blocks()) {
                if (block.y == 0) {
                    game_over = true;
                    return;
                }
            }

            current_mino.update(grid);

            // FIXME: Improve on this
            // What if there is already a tetromino at the spawn
            // location?
            current_mino = Tetromino{next_shape, starting_point};
            next_shape = get_next_shape();

            for (int y = grid.rows() - 1; y >= 0; --y) {
                bool clear_line = true;
                for (std::size_t x = 0; x < grid.columns(); ++x) {
                    if (!grid(x, y)) {
                        clear_line = false;
                        break;
                    }
                }

                if (clear_line) {
                    lines_to_clear[y] = true;
                    clear_effect_timer = CLEAR_EFFECT_DURATION;
                }
            }
        }
        fall_ticker = 0;
    }

    if (move_ticker == MOVE_TICKS) {
        if (input.move) {
            switch (input.move.value()) {
            case Move::Left:
                current_mino.move_left(grid);
                break;
            case Move::Right:
                current_mino.move_right(grid);
                break;
            default:
                break;
            }
        }

        move_ticker = 0;
    }

    current_mino.update_ghost(grid);
}

void draw_game_over(sf::RenderWindow &window, sf::Font &font) {
    sf::Text text;
    text.setFont(font);

    text.setString("Game Over!");
    text.setCharacterSize(25);

    text.setPosition(0, 50);

    window.draw(text);
}

void Game::draw(sf::RenderWindow &window, sf::Shape &shape,
                sf::Font &font) const {
    if (game_over) {
        draw_game_over(window, font);
        return;
    }

    current_mino.draw(window, shape);
    draw_grid(window, shape);
}

void Game::draw_grid(sf::RenderWindow &window, sf::Shape &shape) const {
    for (std::size_t y = 0; y < grid.rows(); y++) {
        for (std::size_t x = 0; x < grid.columns(); x++) {
            if (auto block = grid(x, y); block) {
                auto x_coord = CELL_WIDTH * x;
                auto y_coord = CELL_HEIGHT * y;

                shape.setPosition(x_coord, y_coord);
                shape.setFillColor(get_colour(block.value()));
                window.draw(shape);
            }
        }
    }
}

void Game::handle_key_up(sf::Keyboard::Key key) {
    using sf::Keyboard;

    switch (key) {
    case Keyboard::Left:
    case Keyboard::Right:
        move_ticker = 0;
        input.move = {};
        break;
    case Keyboard::C:
    case Keyboard::Z:
        input.rotate = {};
        break;
    case Keyboard::Down:
        input.soft_drop_pressed = false;
        break;
    case Keyboard::Space:
        input.hard_drop_pressed = false;
        break;
    default:
        break;
    }
}
void Game::handle_key_down(sf::Keyboard::Key key) {
    using sf::Keyboard;

    switch (key) {
    case Keyboard::Left:
        input.move = Move::Left;
        break;
    case Keyboard::Right:
        input.move = Move::Right;
        break;
    case Keyboard::C:
        input.rotate = Rotate::CW;
        break;
    case Keyboard::Z:
        input.rotate = Rotate::CounterCW;
        break;
    case Keyboard::Down:
        input.soft_drop_pressed = true;
        break;
    case Keyboard::Space:
        input.hard_drop_pressed = true;
        break;
    default:
        break;
    }
}
} // namespace tetris
