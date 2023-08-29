#pragma once

#include <algorithm>

#include <riw/value_range/value_range.hpp>
#include <riw/concepts/arithmetic.hpp>

namespace riw {
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
}
