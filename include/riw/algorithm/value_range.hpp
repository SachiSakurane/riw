#pragma once

#include <riw/concepts/arithmetic.hpp>

namespace riw {
template <riw::arithmetic Type>
struct range {
  const Type min, max;
  constexpr range(Type _min, Type _max) : min{_min}, max{_max} { assert(min < max); }
  Type length() const { return max - min; }
};

template <riw::arithmetic Type>
static constexpr range<Type> normal_range = {static_cast<Type>(0), static_cast<Type>(1)};
} // namespace riw
