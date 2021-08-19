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
};
