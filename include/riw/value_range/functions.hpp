#pragma once

#include <concepts>

#include <riw/algorithm/lerp.hpp>
#include <riw/value_range/value_range.hpp>
#include <riw/concepts/arithmetic.hpp>

namespace riw {
template <std::totally_ordered Type>
inline constexpr Type length(const value_range<Type> &r) {
  return r.max - r.min;
}

template <std::totally_ordered Type>
inline constexpr Type clamp(Type value, const value_range<Type> &r) {
  return std::clamp(value, r.min, r.max);
}

template <std::floating_point Type>
inline constexpr Type proportion(Type value, const value_range<Type> &r) {
  return (value - r.min) / length(r);
}

template <std::floating_point Type>
inline constexpr bool contain(Type value, const value_range<Type> &r) {
  return r.min <= value && value <= r.max;
}

template <std::floating_point FloatType>
inline constexpr FloatType lerp(const value_range<FloatType> &range, FloatType x) {
  return range.min * (static_cast<FloatType>(1) - x) + range.max * x;
}

template <std::floating_point Type>
inline constexpr Type normalize(Type v, const riw::value_range<Type> &from,
                                const riw::value_range<Type> &to = {static_cast<Type>(0),
                                                                    static_cast<Type>(1)}) {
  return riw::lerp(to, riw::proportion(v, from));
}

template <std::floating_point Type>
inline constexpr decltype(auto) inverse(Type v,
                                        riw::value_range<Type> range = riw::normal_range<Type>) {
  return range.max + range.min - v;
}

template <riw::arithmetic To>
inline constexpr decltype(auto) convert(const auto &range) {
  return riw::value_range<To>{static_cast<To>(range.min), static_cast<To>(range.max)};
}

template <std::floating_point Type>
inline constexpr decltype(auto) log10(const value_range<Type> &r) {
  assert(r.min >= 0);
  return riw::value_range{std::log10(r.min), std::log10(r.max)};
}
}