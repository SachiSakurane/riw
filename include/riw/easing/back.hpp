#pragma once

#include <riw/concepts/floating_point.hpp>

namespace riw {
struct back {
  template <riw::floating_point T>
  constexpr T operator()(T v) {
    return v * v * (static_cast<T>(2.70158) * v - static_cast<T>(1.70158));
  }
};
} // namespace riw
