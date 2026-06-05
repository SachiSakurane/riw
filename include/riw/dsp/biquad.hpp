#pragma once

#include <array>
#include <cmath>
#include <concepts>

#include <riw/math/constants.hpp>

namespace riw {
template <std::floating_point FloatType>
class biquad_filter {
public:
  using coefficients_type = std::array<FloatType, 6>;

  constexpr void set_coefficients(const coefficients_type &coefficients) {
    const auto inv_a0 = static_cast<FloatType>(1) / coefficients[3];
    b0 = coefficients[0] * inv_a0;
    b1 = coefficients[1] * inv_a0;
    b2 = coefficients[2] * inv_a0;
    a1 = coefficients[4] * inv_a0;
    a2 = coefficients[5] * inv_a0;
  }

  constexpr void reset() {
    s1 = static_cast<FloatType>(0);
    s2 = static_cast<FloatType>(0);
  }

  constexpr FloatType filter(FloatType in) {
    const auto out = s1 + b0 * in;
    s1 = s2 + b1 * in - a1 * out;
    s2 = b2 * in - a2 * out;
    return out;
  }

private:
  FloatType b0 = static_cast<FloatType>(1);
  FloatType b1 = static_cast<FloatType>(0);
  FloatType b2 = static_cast<FloatType>(0);
  FloatType a1 = static_cast<FloatType>(0);
  FloatType a2 = static_cast<FloatType>(0);
  FloatType s1 = static_cast<FloatType>(0);
  FloatType s2 = static_cast<FloatType>(0);
};

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
