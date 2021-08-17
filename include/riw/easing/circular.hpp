#pragma once

#include <cmath>

#include <riw/concepts/floationg_point.hpp>

namespace riw {
struct circular {
  template <riw::floating_point T>
  constexpr T operator()(T v) {
    return static_cast<T>(1) - std::sqrt(static_cast<T>(1) - v * v);
  }
};
} // namespace riw
