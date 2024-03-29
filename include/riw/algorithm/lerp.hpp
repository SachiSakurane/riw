#pragma once

#include <algorithm>

#include <riw/concepts/floating_point.hpp>

namespace riw {
template <riw::floating_point FloatType>
inline constexpr FloatType lerp(FloatType a, FloatType b, FloatType x) {
  return a * (static_cast<FloatType>(1) - x) + b * x;
}

template <class Container, riw::floating_point FloatType>
inline constexpr FloatType lerp(const Container &c, FloatType v) {
  using cof_value_type = decltype(v);
  v = std::clamp(v, static_cast<cof_value_type>(0.0), static_cast<cof_value_type>(1.0));
  if (v == static_cast<cof_value_type>(1.0))
    return c[c.size() - 1];
  v *= static_cast<cof_value_type>(c.size() - 1);
  size_t v_pos = static_cast<size_t>(v);
  typename Container::value_type l = v - v_pos;
  return riw::lerp(c[v_pos], c[v_pos + 1], l);
}

} // namespace riw
