#pragma once

#include <cmath>

#include <riw/concepts/floating_point.hpp>
#include <riw/math/constants.hpp>

namespace riw {
struct sine {
  template <riw::floating_point T>
  constexpr decltype(auto) operator()(T v) {
    return static_cast<T>(1) - std::cos(riw::half_pi<T> * v);
  }
};
} // namespace riw
