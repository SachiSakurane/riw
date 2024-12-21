#pragma once

#include <algorithm>
#include <concepts>

namespace riw {
template <std::totally_ordered Type>
struct value_range {
  const Type min, max;
  constexpr value_range(Type _min, Type _max)
      : min{std::min(_min, _max)}, max{std::max(_min, _max)} {}
};

template <std::totally_ordered Type>
value_range(Type, Type) -> value_range<Type>;

} // namespace riw
