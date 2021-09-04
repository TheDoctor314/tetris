#pragma once

#include <optional>

enum class Move {
    Left,
    Right,
};

enum class Rotate {
    CW,
    CounterCW,
};

struct Input {
    std::optional<Move> move;
    std::optional<Rotate> rotate;
    bool soft_drop_pressed = false;
    bool hard_drop_pressed = false;
};
