#pragma once

#include <cmath>
#include <type_traits>

#include <riw/numbers/constants.h>

namespace riw
{
  struct elastic
  {
    template <typename T, typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
    constexpr T operator()(T normalized_time)
    {
      if (normalized_time == 0)
        return 0;
      if (normalized_time == 1)
        return 1;
      T mrvs_time = normalized_time - 1;
      T post_fix = std::pow(2.0, 10.0 * mrvs_time);
      return -(post_fix * std::sin((mrvs_time - 0.075) * mk2::two_pi<T> * 3.33333333));
    }
  };
}
