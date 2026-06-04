#pragma once

#include <array>
#include <cmath>
#include <concepts>

#include <riw/math/constants.hpp>

namespace riw {
template <std::floating_point FloatType>
constexpr auto make_highpass_coefficients(FloatType sample_rate, FloatType cutoff_freq,
                                          FloatType resonance) {
  const auto omega = riw::two_pi<FloatType> * cutoff_freq / sample_rate;
  const auto alpha = std::sin(omega) / (static_cast<FloatType>(2) * resonance);
  const auto cos_omega = std::cos(omega);

  const auto b0 = (static_cast<FloatType>(1) + cos_omega) / static_cast<FloatType>(2);
  const auto b1 = -(static_cast<FloatType>(1) + cos_omega);
  const auto b2 = (static_cast<FloatType>(1) + cos_omega) / static_cast<FloatType>(2);
  const auto a0 = static_cast<FloatType>(1) + alpha;
  const auto inv_a0 = static_cast<FloatType>(1) / a0;
  const auto a1 = -static_cast<FloatType>(2) * cos_omega;
  const auto a2 = static_cast<FloatType>(1) - alpha;
  return std::to_array<FloatType>(
      {b0 * inv_a0, b1 * inv_a0, b2 * inv_a0, static_cast<FloatType>(1), a1 * inv_a0,
       a2 * inv_a0});
}

template <std::floating_point FloatType>
constexpr auto make_lowpass_coefficients(FloatType sample_rate, FloatType cutoff_freq,
                                         FloatType resonance) {
  const auto omega = riw::two_pi<FloatType> * cutoff_freq / sample_rate;
  const auto alpha = std::sin(omega) / (static_cast<FloatType>(2) * resonance);
  const auto cos_omega = std::cos(omega);
  const auto b0 = (static_cast<FloatType>(1) - cos_omega) / static_cast<FloatType>(2);
  const auto b1 = static_cast<FloatType>(1) - cos_omega;
  const auto b2 = (static_cast<FloatType>(1) - cos_omega) / static_cast<FloatType>(2);
  const auto a0 = static_cast<FloatType>(1) + alpha;
  const auto inv_a0 = static_cast<FloatType>(1) / a0;
  const auto a1 = -static_cast<FloatType>(2) * cos_omega;
  const auto a2 = static_cast<FloatType>(1) - alpha;
  return std::to_array<FloatType>(
      {b0 * inv_a0, b1 * inv_a0, b2 * inv_a0, static_cast<FloatType>(1), a1 * inv_a0,
       a2 * inv_a0});
}
} // namespace riw
