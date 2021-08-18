#pragma once

#include <cmath>

#include <riw/concepts/floating_point.hpp>

namespace riw {
struct exponential {
  template <riw::floating_point T>
  constexpr T operator()(T v) {
    return std::pow(static_cast<T>(2), static_cast<T>(10) * (v - static_cast<T>(1)));
  }
};
} // namespace riw
