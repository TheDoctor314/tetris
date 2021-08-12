#include <SFML/Graphics.hpp>
#include <chrono>
#include <random>

#include "grid.hpp"
#include "tetromino.hpp"

using namespace tetris;

void draw_grid(GameGrid &grid, sf::RenderWindow &window, sf::Shape &shape);

enum class Move {
    None,
    Left,
    Right,
};

int main() {
    sf::RenderWindow window(
        sf::VideoMode(cols * cell_width * 2, rows * cell_height * 2), "Tetris");
    window.setVerticalSyncEnabled(true);

    auto grid_view =
        sf::View(sf::FloatRect(0, 0, cols * cell_width, rows * cell_height));

    window.setView(grid_view);

    sf::RectangleShape shape{sf::Vector2f(cell_width, cell_height)};
    shape.setFillColor(sf::Color::Green);
    shape.setOutlineColor(sf::Color::Black);
    shape.setOutlineThickness(-1);

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<> dist(0, 6);

    GameGrid grid{};

    auto get_next_shape = [](auto &dist, auto &rng) {
        return static_cast<TetriType>(dist(rng));
    };

    auto starting_point = sf::Vector2i(cols / 2 - 2, 0);

    auto mino = Tetromino{get_next_shape(dist, rng), starting_point};
    auto next_shape = get_next_shape(dist, rng);

    auto previous_time = std::chrono::steady_clock::now();
    unsigned lag = 0;
    unsigned fall_ticker = 0;

    unsigned move_ticker = 0;
    const unsigned move_ticks = 10;

    auto movement = Move::None;

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
                    movement = Move::Left;
                    break;
                case Keyboard::Right:
                    movement = Move::Right;
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
                    movement = Move::None;
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

        while (lag > frame_duration) {
            lag -= frame_duration;
            fall_ticker += 1;
            move_ticker += 1;

            if (fall_ticker == fall_ticks) {
                if (!mino.move_down(grid)) {
                    mino.update(grid);

                    // FIXME: Improve on this
                    // What if there is already a tetromino at the spawn
                    // location?
                    mino = Tetromino{next_shape, starting_point};
                    next_shape = get_next_shape(dist, rng);
                }
                fall_ticker = 0;
            }

            if (move_ticker == move_ticks) {
                using sf::Keyboard;

                switch (movement) {
                case Move::Left:
                    mino.move_left(grid);
                    break;
                case Move::Right:
                    mino.move_right(grid);
                    break;
                case Move::None:
                default:
                    break;
                }

                move_ticker = 0;
            }
        }

        window.clear();

        mino.draw(window, shape);
        draw_grid(grid, window, shape);

        window.display();
    }

    return 0;
}

void draw_grid(GameGrid &grid, sf::RenderWindow &window, sf::Shape &shape) {
    for (std::size_t y = 0; y < grid.rows(); y++) {
        for (std::size_t x = 0; x < grid.columns(); x++) {
            if (grid(x, y)) {
                auto x_coord = cell_width * x;
                auto y_coord = cell_height * y;

                shape.setPosition(x_coord, y_coord);
                window.draw(shape);
            }
        }
    }
}
