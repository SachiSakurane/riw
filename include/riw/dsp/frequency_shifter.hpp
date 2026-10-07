#pragma once

#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <limits>
#include <span>
#include <utility>
#include <vector>

#include <riw/math/constants.hpp>

namespace riw {
// Windowed Hilbert FIR followed by single-sideband modulation. All channels
// share an oscillator. See docs/frequency-shifter.md for bandwidth limitations.
template <std::floating_point FloatType = float>
class frequency_shifter {
public:
  struct configuration {
    double sample_rate = 48000;
    size_t channels = 1;
    size_t filter_size = 257;
    double smoothing_seconds = 0.02;
  };

  // Invalid configuration or allocation failure preserves the old state.
  // Allocation can throw here; processing, reset and controls never allocate.
  bool prepare(configuration config) {
    if (!std::isfinite(config.sample_rate) || config.sample_rate <= 0 ||
        config.channels == 0 || config.channels > 8 ||
        config.filter_size < 7 || config.filter_size > 4095 ||
        config.filter_size % 2 == 0 ||
        !std::isfinite(config.smoothing_seconds) || config.smoothing_seconds < 0)
      return false;
    frequency_shifter next;
    next.config_ = config;
    next.history_.assign(config.channels * config.filter_size, 0);
    const auto delay = (config.filter_size - 1) / 2;
    for (size_t m = 1; m <= delay; m += 2) {
      // h[D+m] = 2/(pi*m) times a symmetric Blackman window;
      // h[D-m] = -h[D+m]. Even offsets and the center are zero.
      const auto angle = pi<double> * static_cast<double>(m) / static_cast<double>(delay);
      const auto window = 0.42 + 0.5 * std::cos(angle) + 0.08 * std::cos(2 * angle);
      next.coefficients_.push_back(2 * window / (pi<double> * static_cast<double>(m)));
    }
    next.smoothing_step_ = config.smoothing_seconds == 0 ? 1 :
        -std::expm1(-(1 / config.sample_rate) / config.smoothing_seconds);
    next.prepared_ = true;
    *this = std::move(next);
    return true;
  }

  bool is_prepared() const noexcept { return prepared_; }
  configuration config() const noexcept { return config_; }
  size_t latency_samples() const noexcept {
    return prepared_ ? (config_.filter_size - 1) / 2 : 0;
  }

  // Positive Hz moves positive-frequency components upward; negative Hz moves
  // them downward. A prepared stream accepts [-sample_rate/2, sample_rate/2].
  // Call controls on the processing thread, or while processing is stopped.
  bool set_shift_hz(double hz) noexcept {
    if (!prepared_ || !std::isfinite(hz) || std::abs(hz) > config_.sample_rate / 2)
      return false;
    target_shift_ = hz;
    return true;
  }

  // Clear filter history and oscillator phase, and snap to the target shift.
  void reset() noexcept {
    std::fill(history_.begin(), history_.end(), 0);
    position_ = 0;
    phase_ = 0;
    shift_ = target_shift_;
  }

  // Exactly config.channels planar pointers, each addressing frames samples.
  // Same-channel in-place processing is supported; other overlap is prohibited.
  // Invalid shape/unprepared calls leave output and streaming state untouched.
  bool process(std::span<const FloatType *const> input,
               std::span<FloatType *const> output, size_t frames) noexcept {
    if (!prepared_ || input.size() != config_.channels || output.size() != config_.channels)
      return false;
    for (size_t c = 0; c < config_.channels; ++c)
      if (frames != 0 && (!input[c] || !output[c]))
        return false;
    const auto n = config_.filter_size;
    const auto delay = latency_samples();
    const auto limit = static_cast<double>(std::numeric_limits<FloatType>::max());
    for (size_t s = 0; s < frames; ++s) {
      shift_ += smoothing_step_ * (target_shift_ - shift_);
      const auto cosine = std::cos(phase_);
      const auto sine = std::sin(phase_);
      for (size_t c = 0; c < config_.channels; ++c)
        history_[c * n + position_] = clean(static_cast<double>(input[c][s]));
      for (size_t c = 0; c < config_.channels; ++c) {
        const auto *history = history_.data() + c * n;
        double quadrature = 0;
        for (size_t k = 0; k < coefficients_.size(); ++k) {
          const auto m = 2 * k + 1;
          quadrature += coefficients_[k] *
              (history[(position_ + n - delay - m) % n] -
               history[(position_ + n - delay + m) % n]);
        }
        const auto real = history[(position_ + n - delay) % n];
        const auto value = clean(real * cosine - quadrature * sine);
        output[c][s] = static_cast<FloatType>(std::clamp(value, -limit, limit));
      }
      position_ = (position_ + 1) % n;
      // Divide first so finite shifts remain safe even at extreme sample rates.
      phase_ = std::remainder(phase_ + two_pi<double> * (shift_ / config_.sample_rate),
                              two_pi<double>);
    }
    return true;
  }

private:
  static double clean(double value) noexcept {
    return std::isfinite(value) && std::abs(value) >= 1e-30 ? value : 0;
  }

  configuration config_{};
  bool prepared_ = false;
  size_t position_ = 0;
  double target_shift_ = 0;
  double shift_ = 0;
  double phase_ = 0;
  double smoothing_step_ = 1;
  std::vector<double> history_, coefficients_;
};
} // namespace riw
