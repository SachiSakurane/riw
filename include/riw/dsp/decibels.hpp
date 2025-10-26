#pragma once

#include <algorithm>
#include <concepts>

namespace riw {
template <std::floating_point T>
static constexpr auto default_minus_infinity_db = static_cast<T>(-100);

template <std::floating_point T>
constexpr auto gain_to_decibels(T gain, T min_db = default_minus_infinity_db<T>) -> T {
  return gain > static_cast<T>(0) ? std::max(min_db, static_cast<T>(20) * std::log10(gain))
                                  : min_db;
}
} // namespace riw
