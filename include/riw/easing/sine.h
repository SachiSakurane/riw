#pragma once

#include <cmath>
#include <type_traits>

#include <riw/numbers/constants.h>

namespace riw
{
  struct sine
  {
    template <typename T, typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
    constexpr T operator()(T normalized_time)
    {
      return 1.0 - std::cos(riw::half_pi<T> * normalized_time);
    }
  };
}
