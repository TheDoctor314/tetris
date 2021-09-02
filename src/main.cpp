#include <SFML/Graphics.hpp>
#include <chrono>

#include "game.hpp"
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

    auto previous_time = std::chrono::steady_clock::now();
    unsigned lag = 0;

    Game game{};

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
                default:
                    game.handle_key_down(event.key.code);
                    break;
                }
                break;
            case Event::KeyReleased:
                game.handle_key_up(event.key.code);
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
            game.update();
        }

        window.clear();

        game.draw(window, shape, font);

        window.display();
    }

    return 0;
}
