#pragma once

#include <riw/algorithm/lerp.hpp>
#include <riw/algorithm/value_range.hpp>
#include <riw/concepts/floating_point.hpp>

namespace riw {
template <riw::floating_point Type>
inline constexpr Type normalize(Type v, const riw::value_range<Type> &from,
                                const riw::value_range<Type> &to) {
  return riw::lerp(to, riw::proportion(v, from));
}
} // namespace riw
