#pragma once

#include <algorithm>

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

template <riw::arithmetic Type>
inline constexpr Type clamp(Type value, const value_range<Type> &r) {
  return std::clamp(value, r.min, r.max);
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

template <riw::arithmetic Type, riw::value_range<Type> Range>
struct ranged {
  static constexpr decltype(auto) range = Range;
  constexpr ranged(Type v) : value{riw::clamp(v, Range)} {}

  ranged(const ranged &) = default;
  ranged(ranged &&) = default;
  ranged &operator=(const ranged &) = default;
  ranged &operator=(ranged &&r) = default;

  Type get() const { return value; }

private:
  Type value;
};

template <riw::arithmetic Type, riw::value_range<Type> Range>
bool operator==(const ranged<Type, Range> &a, const ranged<Type, Range> &b) {
  return a.get() == b.get();
}

} // namespace riw
