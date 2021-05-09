#pragma once

#include <type_traits>

namespace riw
{
  template <class Type, class FloatType, typename = typename std::enable_if<std::is_floating_point<FloatType>::value>::type>
  inline constexpr Type lerp(Type a, Type b, FloatType x)
  {
    return static_cast<Type>(a * (static_cast<FloatType>(1) - x) + b * x);
  }
}
