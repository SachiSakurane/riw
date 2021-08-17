#pragma once

#include <riw/algorithm/value_range.hpp>
#include <riw/concepts/floationg_point.hpp>

namespace riw {
template <riw::floating_point Type>
inline constexpr Type normalize(Type v, riw::range<Type> range) {
  assert(range.min <= v);
  assert(v <= range.max);
  return range.min + (v - range.min) / range.length();
}
} // namespace riw
