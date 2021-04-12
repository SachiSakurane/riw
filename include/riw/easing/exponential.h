#pragma once

#include <cmath>
#include <type_traits>

namespace riw
{
  struct exponential
  {
    template <typename T, typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
    constexpr T operator()(T normalized_time)
    {
      return std::pow(static_cast<T>(2), static_cast<T>(10.0 * (normalized_time - 1.0)));
    }
  };
}
