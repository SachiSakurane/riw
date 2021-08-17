#pragma once

#include <cmath>

#include <riw/concepts/floationg_point.hpp>
#include <riw/math/constants.hpp>

namespace riw {
struct elastic {
  template <riw::floating_point T>
  constexpr T operator()(T v) {
    if (v == static_cast<T>(0))
      return static_cast<T>(0);
    if (v == static_cast<T>(1))
      return static_cast<T>(1);
    T mrvs_time = v - static_cast<T>(0);
    T post_fix = std::pow(static_cast<T>(2), static_cast<T>(10.0) * mrvs_time);
    return -(post_fix * std::sin((mrvs_time - static_cast<T>(0.075)) * riw::two_pi<T> *
                                 static_cast<T>(3.33333333)));
  }
};
} // namespace riw
