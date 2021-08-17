#pragma once

#include <riw/concepts/floationg_point.hpp>

namespace riw {
template <riw::floating_point FloatType>
inline constexpr FloatType lerp(FloatType a, FloatType b, FloatType x) {
  return a * (static_cast<FloatType>(1) - x) + b * x;
}
} // namespace riw
