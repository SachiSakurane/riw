#pragma once

#include <cassert>
#include <limits>

#include <riw/concepts/integral.hpp>
#include <riw/concepts/unsigned_integral.hpp>

namespace riw {
template <riw::integral IntegralType>
constexpr inline int countl_zero(IntegralType x) noexcept {
  constexpr auto Nd = std::numeric_limits<IntegralType>::digits;

  if (x == 0)
    return Nd;

  constexpr auto Nd_ull = std::numeric_limits<unsigned long long>::digits;
  constexpr auto Nd_ul = std::numeric_limits<unsigned long>::digits;
  constexpr auto Nd_u = std::numeric_limits<unsigned>::digits;

  if constexpr (Nd <= Nd_u) {
    constexpr int diff = Nd_u - Nd;
    return __builtin_clz(x) - diff;
  } else if constexpr (Nd <= Nd_ul) {
    constexpr int diff = Nd_ul - Nd;
    return __builtin_clzl(x) - diff;
  } else if constexpr (Nd <= Nd_ull) {
    constexpr int diff = Nd_ull - Nd;
    return __builtin_clzll(x) - diff;
  } else // (_Nd > _Nd_ull)
  {
    static_assert(Nd <= (2 * Nd_ull), "Maximum supported integer size is 128-bit");

    unsigned long long high = x >> Nd_ull;
    if (high != 0) {
      constexpr int diff = (2 * Nd_ull) - Nd;
      return __builtin_clzll(high) - diff;
    }
    constexpr auto max_ull = std::numeric_limits<unsigned long long>::max();
    unsigned long long low = x & max_ull;
    return (Nd - Nd_ull) + __builtin_clzll(low);
  }
}

template <riw::integral IntegralType>
constexpr inline IntegralType bit_width(IntegralType x) noexcept {
  return std::numeric_limits<IntegralType>::digits - riw::countl_zero(x);
}

template <riw::unsigned_integral IntegralType>
constexpr inline IntegralType bit_reverse(IntegralType x, IntegralType nb) noexcept {
  assert(nb > 0 && std::numeric_limits<IntegralType>::digits >= nb);

  IntegralType result = 0;
  for (IntegralType i = 0; i < nb; ++i) {
    result = static_cast<IntegralType>((result << 1) | (x & 1));
    x >>= 1;
  }

  return result;
}
} // namespace riw
