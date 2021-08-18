#pragma once

#include <riw/algorithm/value_range.hpp>
#include <riw/concepts/floating_point.hpp>

namespace riw {
template <riw::floating_point FloatType>
inline constexpr FloatType lerp(FloatType a, FloatType b, FloatType x) {
  return a * (static_cast<FloatType>(1) - x) + b * x;
}

template <riw::floating_point FloatType>
inline constexpr FloatType lerp(const value_range<FloatType>& range, FloatType x) {
  return range.min * (static_cast<FloatType>(1) - x) + range.max * x;
}

} // namespace riw
