#pragma once

#include <riw/concepts/arithmetic.hpp>
#include <riw/concepts/floating_point.hpp>

namespace riw {
template <riw::arithmetic Type>
struct value_range {
  const Type min, max;
  constexpr value_range(Type _min, Type _max) : min{_min}, max{_max} { assert(min <= max); }
};

template <riw::arithmetic Type>
inline constexpr Type length(const value_range<Type> &r) {
  return r.max - r.min;
}

template <riw::floating_point Type>
inline constexpr Type proportion(Type value, const value_range<Type> &r) {
  return (value - r.min) / length(r);
}

template <riw::floating_point Type>
inline constexpr bool contain(Type value, const value_range<Type> &r) {
  return r.min <= value && value <= r.max;
}

template <riw::arithmetic Type>
static constexpr value_range<Type> normal_range = {static_cast<Type>(0), static_cast<Type>(1)};
} // namespace riw
