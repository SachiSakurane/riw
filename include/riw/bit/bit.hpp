#pragma once

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
  // static_assert(std::numeric_limits<IntegralType>::digits == 32);
  assert(nb > 0 && std::numeric_limits<IntegralType>::digits > nb);

  x = (x << 16) | (x >> 16);
  x = ((x & 0x00FF00FF) << 8) | ((x & 0xFF00FF00) >> 8);
  x = ((x & 0x0F0F0F0F) << 4) | ((x & 0xF0F0F0F0) >> 4);
  x = ((x & 0x33333333) << 2) | ((x & 0xCCCCCCCC) >> 2);
  x = ((x & 0x55555555) << 1) | ((x & 0xAAAAAAAA) >> 1);

  return ((x >> (32 - nb)) & (0xFFFFFFFF >> (32 - nb)));
}
} // namespace riw
