#pragma once

#include <cmath>
#include <type_traits>

namespace riw
{
  struct circular
  {
    template <typename T, typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
    constexpr T operator()(T normalized_time)
    {
      return static_cast<T>(1) - std::sqrt(static_cast<T>(1) - normalized_time * normalized_time);
    }
  };

}
