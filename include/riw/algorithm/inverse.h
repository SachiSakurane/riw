#pragma once

#include <type_traits>

namespace riw
{
  template <typename Type, typename = typename std::enable_if<std::is_floating_point<Type>::value>::type>
  inline constexpr decltype(auto) inverse(Type v)
  {
    return static_cast<Type>(1) - v;
  }
}
