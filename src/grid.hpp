#pragma once

#include <array>
#include <cassert>

#include "constants.hpp"

namespace tetris {
template<typename T, std::size_t cols_, std::size_t rows_>
class Grid {
  public:
    using value_type = T;
    using ref = value_type &;
    using const_ref = const value_type &;

    // performs bounds checking
    constexpr ref at(std::size_t x, std::size_t y) {
        assert(x < cols_);
        assert(y < rows_);
        return operator()(x, y);
    }
    constexpr const_ref at(std::size_t x, std::size_t y) const {
        assert(x < cols_);
        assert(y < rows_);
        return operator()(x, y);
    }

    // no bound checks
    constexpr ref operator()(std::size_t x, std::size_t y) {
        return m_data[index(x, y)];
    }
    constexpr const_ref operator()(std::size_t x, std::size_t y) const {
        return m_data[index(x, y)];
    }

    constexpr std::size_t columns() const { return cols_; }
    constexpr std::size_t rows() const { return rows_; }

  private:
    constexpr std::size_t index(std::size_t x, std::size_t y) const {
        return y * cols_ + x;
    }

  private:
    std::array<value_type, cols_ * rows_> m_data{};
};

using GameGrid = Grid<bool, cols, rows>;
} // namespace tetris
