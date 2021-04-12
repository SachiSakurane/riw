#pragma once

#include <type_traits>

namespace riw
{
  struct back
  {
    template <typename T, typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
    constexpr T operator()(T normalized_time)
    {
      return normalized_time * normalized_time * (static_cast<T>(2.70158) * normalized_time - static_cast<T>(1.70158));
    }
  };

}
