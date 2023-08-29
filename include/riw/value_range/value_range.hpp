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
} // namespace riw
