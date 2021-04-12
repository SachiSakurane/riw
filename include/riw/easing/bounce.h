#pragma once

#include <type_traits>

namespace riw
{
  struct bounce
  {
    template <typename T, typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
    constexpr T operator()(T normalized_time)
    {
      T rvs_time = 1.0 - normalized_time;
      if (rvs_time < (1.0 / 2.75))
      {
        return 1.0 - 7.5625 * rvs_time * rvs_time;
      }
      else if (rvs_time < (2.0 / 2.75))
      {
        T post_fix = rvs_time - (1.5 / 2.75);
        return -7.5625 * rvs_time * post_fix + 0.25;
      }
      else if (rvs_time < (2.5 / 2.75))
      {
        T post_fix = rvs_time - (2.25 / 2.75);
        return -7.5625 * rvs_time * post_fix + 0.0625;
      }

      T post_fix = rvs_time - (2.625 / 2.75);
      return -7.5625 * rvs_time * post_fix + 0.015625;
    }
  };
}
