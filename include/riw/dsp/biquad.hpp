#pragma once

#include <array>
#include <concepts>

namespace riw {
template <std::floating_point FloatType>
constexpr auto make_highpass_coefficients(FloatType sample_rate, FloatType cutoff_freq,
                                          FloatType resonance) {
  FloatType omega = riw::two_pi<FloatType> * cutoff_freq / sample_rate;
  FloatType alpha = std::sin(omega) / (2.0f * resonance);
  FloatType cos_omega = std::cos(omega);

  FloatType b0 = (1.0f + cos_omega) / 2.0f;
  FloatType b1 = -(1.0f + cos_omega);
  FloatType b2 = (1.0f + cos_omega) / 2.0f;
  FloatType a0 = 1.0f + alpha;
  FloatType a1 = -2.0f * cos_omega;
  FloatType a2 = 1.0f - alpha;
  return std::to_array<FloatType>(
      {b0 / a0, b1 / a0, b2 / a0, static_cast<FloatType>(1), a1 / a0, a2 / a0});
}

template <std::floating_point FloatType>
constexpr auto make_lowpass_coefficients(FloatType sample_rate, FloatType cutoff_freq,
                                         FloatType resonance) {
  FloatType omega = riw::two_pi<FloatType> * cutoff_freq / sample_rate;
  FloatType alpha = std::sin(omega) / (2.0f * resonance);
  FloatType cos_omega = std::cos(omega);
  FloatType b0 = (1.0f - cos_omega) / 2.0f;
  FloatType b1 = 1.0f - cos_omega;
  FloatType b2 = (1.0f - cos_omega) / 2.0f;
  FloatType a0 = 1.0f + alpha;
  FloatType a1 = -2.0f * cos_omega;
  FloatType a2 = 1.0f - alpha;
  return std::to_array<FloatType>(
      {b0 / a0, b1 / a0, b2 / a0, static_cast<FloatType>(1), a1 / a0, a2 / a0});
}
} // namespace riw
