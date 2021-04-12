#pragma once

#include <type_traits>

namespace riw
{
  struct linear
  {
    template <typename T, typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
    constexpr T operator()(T normalized_time)
    {
      return normalized_time;
    }
  };
}
