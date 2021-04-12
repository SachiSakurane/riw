#pragma once

#include <type_traits>

namespace riw
{
  template <typename Function, typename T, typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
  inline constexpr T ease_in(T normalized_time)
  {
    return Function{}(normalized_time);
  }

  template <typename Function, typename T, typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
  inline constexpr T ease_out(T normalized_time)
  {
    return 1.0 - Function{}(1.0 - normalized_time);
  }

  template <typename Function, typename T, typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
  inline constexpr T ease_in_out(T normalized_time)
  {
    return normalized_time < 0.5
               ? riw::ease_in<Function>(normalized_time * 2.0) * 0.5
               : riw::ease_out<Function>((normalized_time - 0.5) * 2.0) * 0.5 + 0.5;
  }

  template <typename EaseFunction, typename T, typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
  inline constexpr T easing(T time, T start_value, T change_in_value, T duration)
  {
    return start_value + change_in_value * EaseFunction{}(time / duration);
  }
}
