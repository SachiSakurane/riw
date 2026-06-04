#pragma once

#include <cmath>
#include <concepts>

#include <riw/math/constants.hpp>

namespace riw {
template <class ContainerType>
constexpr void hann_inplace(ContainerType &srcdst) {
  using FloatType = typename ContainerType::value_type;
  const auto size = srcdst.size();
  if (size == 0)
    return;
  if (size == 1) {
    srcdst[0] = static_cast<FloatType>(1);
    return;
  }

  const auto step = riw::two_pi<FloatType> / static_cast<FloatType>(size - 1);
  const auto cos_step = std::cos(step);
  const auto sin_step = std::sin(step);
  auto cos_angle = static_cast<FloatType>(1);
  auto sin_angle = static_cast<FloatType>(0);

  for (auto n = size - size; n + 1 < size; ++n) {
    srcdst[n] = static_cast<FloatType>(0.5) * (static_cast<FloatType>(1) - cos_angle);

    const auto next_cos = cos_angle * cos_step - sin_angle * sin_step;
    const auto next_sin = sin_angle * cos_step + cos_angle * sin_step;
    cos_angle = next_cos;
    sin_angle = next_sin;
  }
  srcdst[size - 1] = static_cast<FloatType>(0);
}
} // namespace riw
