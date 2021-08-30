#include <SFML/Graphics.hpp>
#include <chrono>
#include <random>

#include "grid.hpp"
#include "input.hpp"
#include "tetromino.hpp"

using namespace tetris;

void draw_grid(const GameGrid &grid, sf::RenderWindow &window,
               sf::Shape &shape);

void draw_game_over(sf::RenderWindow &window, sf::Font &font);

template<std::size_t SIZE>
void clear_lines(GameGrid &grid, const std::array<bool, SIZE> lines_to_clear);

int main() {
    sf::RenderWindow window(
        sf::VideoMode(COLS * CELL_WIDTH * 2, ROWS * CELL_HEIGHT * 2), "Tetris");
    window.setVerticalSyncEnabled(true);

    auto grid_view =
        sf::View(sf::FloatRect(0, 0, COLS * CELL_WIDTH, ROWS * CELL_HEIGHT));

    window.setView(grid_view);

    sf::RectangleShape shape{sf::Vector2f(CELL_WIDTH, CELL_HEIGHT)};
    shape.setOutlineColor(sf::Color::Black);
    shape.setOutlineThickness(-1);

    sf::Font font;
    font.loadFromFile("Roboto-Medium.ttf");

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<> dist(0, 6);

    GameGrid grid{};

    auto get_next_shape = [&dist, &rng] {
        return static_cast<TetriType>(dist(rng));
    };

    auto starting_point = sf::Vector2i(COLS / 2 - 2, 0);

    auto mino = Tetromino{get_next_shape(), starting_point};
    auto next_shape = get_next_shape();

    auto previous_time = std::chrono::steady_clock::now();
    unsigned lag = 0;

    bool game_over = false;
    unsigned fall_ticker = 0;
    unsigned move_ticker = 0;

    unsigned clear_effect_timer = 0;
    std::array<bool, grid.rows()> lines_to_clear{};

    auto input = Input{};

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                using sf::Event;
                using sf::Keyboard;

            case Event::Closed:
                window.close();
                break;
            case Event::KeyPressed:
                switch (event.key.code) {
                case Keyboard::Q:
                    window.close();
                    break;
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
                default:
                    break;
                }
                break;
            case Event::KeyReleased:
                switch (event.key.code) {
                case Keyboard::Left:
                case Keyboard::Right:
                    move_ticker = 0;
                    input.move = {};
                    break;
                case Keyboard::C:
                case Keyboard::Z:
                    input.rotate = {};
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }

        auto delta_time = std::chrono::duration_cast<std::chrono::microseconds>(
                              std::chrono::steady_clock::now() - previous_time)
                              .count();

        lag += delta_time;
        previous_time += std::chrono::microseconds(delta_time);

        while (lag > FRAME_DURATION) {
            lag -= FRAME_DURATION;
            fall_ticker += 1;
            move_ticker += 1;

            if (game_over) {
                break;
            }

            if (clear_effect_timer != 0) {
                --clear_effect_timer;

                if (clear_effect_timer == 0) {
                    clear_lines(grid, lines_to_clear);
                    lines_to_clear = {};
                }
                continue;
            }

            if (input.rotate) {
                auto clockwise = input.rotate.value() == Rotate::CW;
                mino.rotate(clockwise, grid);
                input.rotate = {};
            }

            if (fall_ticker == FALL_TICKS) {
                if (!mino.move_down(grid)) {

                    for (const auto &block : mino.get_blocks()) {
                        if (block.y == 0) {
                            game_over = true;
                        }
                    }

                    mino.update(grid);

                    // FIXME: Improve on this
                    // What if there is already a tetromino at the spawn
                    // location?
                    mino = Tetromino{next_shape, starting_point};
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
                        mino.move_left(grid);
                        break;
                    case Move::Right:
                        mino.move_right(grid);
                        break;
                    default:
                        break;
                    }
                }

                move_ticker = 0;
            }
        }

        window.clear();

        mino.draw(window, shape);
        draw_grid(grid, window, shape);

        if (game_over) {
            draw_game_over(window, font);
        }

        window.display();
    }

    return 0;
}

void draw_grid(const GameGrid &grid, sf::RenderWindow &window,
               sf::Shape &shape) {
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
    for (int y = grid.rows() - 1; y >= 0; --y) {
        if (lines_to_clear[y]) {
            clear_line(grid, y);

            shift_lines(grid, y);
        }
    }
}

void draw_game_over(sf::RenderWindow &window, sf::Font &font) {
    sf::Text text;
    text.setFont(font);

    text.setString("Game Over!");
    text.setCharacterSize(25);

    text.setPosition(0, 50);

    window.draw(text);
}
