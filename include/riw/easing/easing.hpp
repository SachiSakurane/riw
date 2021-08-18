#pragma once

#include <riw/concepts/floating_point.hpp>

namespace riw {
template <class Function>
inline constexpr decltype(auto) ease_in(riw::floating_point auto normalized_time) {
  return Function{}(normalized_time);
}

template <class Function, riw::floating_point FloatType>
inline constexpr decltype(auto) ease_out(FloatType v) {
  return static_cast<FloatType>(1.0) - Function{}(static_cast<FloatType>(1.0) - v);
}

template <class Function, riw::floating_point FloatType>
inline constexpr decltype(auto) ease_in_out(FloatType v) {
  constexpr FloatType half = static_cast<FloatType>(0.5);
  constexpr FloatType two = static_cast<FloatType>(2.0);

  return (v < half) ? riw::ease_in<Function>(v * two) * half
                    : riw::ease_out<Function>((v - half) * two) * half + half;
}

template <class EaseFunction>
inline constexpr decltype(auto) easing(riw::floating_point auto v, riw::floating_point auto offset,
                                       riw::floating_point auto factor,
                                       riw::floating_point auto duration) {
  return offset + factor * EaseFunction{}(v / duration);
}
} // namespace riw
