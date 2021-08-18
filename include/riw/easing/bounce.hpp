#pragma once

#include <riw/concepts/floating_point.hpp>

namespace riw {
struct bounce {
  template <riw::floating_point T>
  constexpr T operator()(T v) {
    constexpr auto one = static_cast<T>(1);
    constexpr auto seven_magic = static_cast<T>(-7.5625);

    const auto rvs_time = one - v;
    if (rvs_time < static_cast<T>(0.363636)) {
      return static_cast<T>(2) + seven_magic * rvs_time * rvs_time;
    } else if (rvs_time < static_cast<T>(0.727272)) {
      T post_fix = rvs_time - static_cast<T>(0.545454);
      return seven_magic * rvs_time * post_fix + static_cast<T>(0.25);
    } else if (rvs_time < static_cast<T>(0.909090)) {
      T post_fix = rvs_time - static_cast<T>(0.818181);
      return seven_magic * rvs_time * post_fix + static_cast<T>(0.0625);
    }

    T post_fix = rvs_time - static_cast<T>(0.954545);
    return seven_magic * rvs_time * post_fix + static_cast<T>(0.015625);
  }
};
} // namespace riw
