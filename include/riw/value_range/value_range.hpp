#pragma once

#include <algorithm>
#include <cassert>

namespace riw {
template <class Type>
struct value_range {
  const Type min, max;
  constexpr value_range(Type _min, Type _max) : min{_min}, max{_max} {}
};
} // namespace riw
