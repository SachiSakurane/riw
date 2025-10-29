#pragma once

#include <algorithm>
#include <concepts>

#include <riw/value_range/value_range.hpp>

namespace riw {
template <class Type, riw::value_range<Type> Range>
struct ranged {
  using value_type = Type;

  static constexpr decltype(auto) range_value = Range;
  constexpr ranged(Type v) : value{std::clamp(v, range_value.min, range_value.max)} {}

  ranged(const ranged &) = default;
  ranged(ranged &&) = default;
  ranged &operator=(const ranged &) = default;
  ranged &operator=(ranged &&r) = default;

  operator value_type() const { return value; }

  Type value;
};

template <std::equality_comparable Type, riw::value_range<Type> Range>
constexpr bool operator==(const ranged<Type, Range> &a, const ranged<Type, Range> &b) {
  return a.value == b.value;
}
} // namespace riw
