#pragma once

#include <algorithm>

#include <riw/algorithm/value_range.hpp>
#include <riw/concepts/floationg_point.hpp>

namespace riw {
template <riw::floating_point Type>
inline constexpr decltype(auto) inverse(Type v, riw::range<Type> range = riw::normal_range<Type>) {
  return range.max + range.min - v;
}
} // namespace riw
