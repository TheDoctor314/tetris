#include <SFML/Graphics.hpp>
#include <chrono>

#include "grid.hpp"
#include "tetromino.hpp"

using namespace tetris;

int main() {
    const int cell_width = 25;
    const int cell_height = 25;

    // const int game_width = 800;
    // const int game_height = 600;

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

    GameGrid grid{};

    auto mino = Tetromino{TetriType::Z, 0, 0};

    auto previous_time = std::chrono::steady_clock::now();
    unsigned lag = 0;
    const unsigned frame_duration = 16667;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                using sf::Event;

            case Event::Closed:
                window.close();
                break;
            case Event::KeyPressed:
                using sf::Keyboard;
                if (event.key.code == Keyboard::Q)
                    window.close();
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

            mino.translate(0, 1);

            window.clear();

            for (auto &point : mino.get_blocks()) {
                auto x = cell_width * point.x;
                auto y = cell_width * point.y;

                shape.setPosition(x, y);
                window.draw(shape);
            }

            for (int y = 0; y < 20; y++) {
                for (int x = 0; x < 10; x++) {
                    if (grid(x, y)) {
                        auto x_coord = cell_width * x;
                        auto y_coord = cell_height * y;

                        shape.setPosition(x_coord, y_coord);
                        window.draw(shape);
                    }
                }
            }

            window.display();
        }
    }

    return 0;
}
