#pragma once

#include <algorithm>
#include <cmath>
#include <complex>
#include <concepts>
#include <cstddef>
#include <limits>
#include <span>
#include <utility>
#include <vector>

#include <riw/dsp/window.hpp>
#include <riw/math/fft.hpp>

namespace riw {
// Planar, synchronous channels. prepare() allocates all streaming resources.
// See docs/phase-vocoder.md for the streaming and phase-coherence contract.
template <std::floating_point FloatType = float>
class phase_vocoder_pitch_shifter {
public:
  static constexpr double minimum_pitch_ratio = 0.5;
  static constexpr double maximum_pitch_ratio = 2.0;

  struct configuration {
    double sample_rate = 48000;
    size_t channels = 1;
    size_t fft_size = 2048;
    size_t hop_size = 256;
    double smoothing_seconds = 0.02;
  };

  // Invalid configuration leaves the old prepared state intact. Allocation
  // failure may throw here, never in process(), reset(), or set_pitch_ratio().
  bool prepare(configuration config) {
    if (!std::isfinite(config.sample_rate) || config.sample_rate <= 0 ||
        config.channels == 0 || config.channels > 8 || config.fft_size < 16 ||
        config.fft_size > 65536 || !detail::is_power_of_two(config.fft_size) ||
        config.hop_size == 0 || config.hop_size > config.fft_size / 4 ||
        config.fft_size % config.hop_size != 0 ||
        !std::isfinite(config.smoothing_seconds) || config.smoothing_seconds < 0)
      return false;
    phase_vocoder_pitch_shifter next;
    next.config_ = config;
    const auto n = config.fft_size;
    next.window_.resize(n);
    hann_inplace(next.window_);
    next.normalization_.assign(config.hop_size, 0);
    for (size_t i = 0; i < n; ++i)
      next.normalization_[i % config.hop_size] += next.window_[i] * next.window_[i];
    next.input_.assign(config.channels * n, 0);
    next.output_.assign(config.channels * n, 0);
    next.analysis_.resize(config.channels * n);
    next.synthesis_.resize(n);
    next.previous_phase_.assign(config.channels * (n / 2 + 1), 0);
    next.previous_energy_.assign(config.channels * (n / 2 + 1), 0);
    next.correction_.assign(n / 2 + 1, 0);
    next.rotation_.resize(n / 2 + 1);
    next.destination_.resize(n / 2 + 1);
    next.smoothing_ = config.smoothing_seconds == 0 ? 0 :
        std::exp(-static_cast<double>(config.hop_size) /
                 config.sample_rate / config.smoothing_seconds);
    next.prepared_ = true;
    *this = std::move(next);
    return true;
  }

  size_t latency_samples() const noexcept { return prepared_ ? config_.fft_size : 0; }
  bool is_prepared() const noexcept { return prepared_; }
  configuration config() const noexcept { return config_; }

  // Call on the processing thread, or while processing is stopped.
  bool set_pitch_ratio(double ratio) noexcept {
    if (!std::isfinite(ratio) || ratio < minimum_pitch_ratio || ratio > maximum_pitch_ratio)
      return false;
    target_ratio_ = ratio;
    return true;
  }

  // Clears the stream and snaps smoothing to the current target ratio.
  void reset() noexcept {
    std::fill(input_.begin(), input_.end(), 0);
    std::fill(output_.begin(), output_.end(), 0);
    std::fill(previous_phase_.begin(), previous_phase_.end(), 0);
    std::fill(previous_energy_.begin(), previous_energy_.end(), 0);
    std::fill(correction_.begin(), correction_.end(), 0);
    position_ = hop_count_ = 0;
    first_frame_ = true;
    ratio_ = target_ratio_;
  }

  // Exactly config.channels pointers; each addresses frames samples. Supports
  // same-channel in-place processing. No other input/output overlap is allowed.
  // Bad shape/unprepared calls return false without touching state or output.
  bool process(std::span<const FloatType *const> input,
               std::span<FloatType *const> output, size_t frames) noexcept {
    if (!prepared_ || input.size() != config_.channels || output.size() != config_.channels)
      return false;
    for (size_t c = 0; c < config_.channels; ++c)
      if (frames != 0 && (!input[c] || !output[c]))
        return false;
    const auto n = config_.fft_size;
    for (size_t s = 0; s < frames; ++s) {
      // Read every channel before writing, also permitting exact planar aliasing.
      for (size_t c = 0; c < config_.channels; ++c)
        input_[c * n + position_] = clean(static_cast<double>(input[c][s]));
      for (size_t c = 0; c < config_.channels; ++c) {
        const auto index = c * n + position_;
        const auto limit = static_cast<double>(std::numeric_limits<FloatType>::max());
        output[c][s] = static_cast<FloatType>(std::clamp(clean(output_[index]), -limit, limit));
        output_[index] = 0;
      }
      position_ = (position_ + 1) % n;
      if (++hop_count_ == config_.hop_size) {
        hop_count_ = 0;
        process_frame();
      }
    }
    return true;
  }

private:
  using complex = std::complex<double>;
  static double clean(double value) noexcept {
    return std::isfinite(value) && std::abs(value) >= 1e-30 ? value : 0;
  }

  void process_frame() noexcept {
    const auto n = config_.fft_size;
    const auto bins = n / 2 + 1;
    const auto h = static_cast<double>(config_.hop_size);
    ratio_ = target_ratio_ + smoothing_ * (ratio_ - target_ratio_);
    for (size_t c = 0; c < config_.channels; ++c) {
      auto spectrum = std::span<complex>(analysis_).subspan(c * n, n);
      for (size_t i = 0; i < n; ++i)
        spectrum[i] = input_[c * n + (position_ + i) % n] * window_[i];
      fft_inplace(spectrum);
    }
    for (size_t k = 0; k < bins; ++k) {
      const auto omega = two_pi<double> * static_cast<double>(k) / static_cast<double>(n);
      double strongest = 0;
      double frequency = omega;
      double previous_energy = 0;
      for (size_t c = 0; c < config_.channels; ++c) {
        const auto value = analysis_[c * n + k];
        const auto phase = std::arg(value);
        const auto magnitude = std::norm(value);
        const auto index = c * bins + k;
        previous_energy += previous_energy_[index];
        if (magnitude > strongest) {
          strongest = magnitude;
          // fft() has a positive sign: positive-bin phase advances by -omega*H.
          if (!first_frame_ && magnitude > 1e-60 && previous_energy_[index] > 1e-60)
            frequency = omega - std::remainder(phase - previous_phase_[index] + omega * h,
                                               two_pi<double>) / h;
        }
        previous_phase_[index] = phase;
        previous_energy_[index] = magnitude;
      }
      // Silence has no meaningful phase. Restart newly active bins from their
      // analysis phase instead of carrying an arbitrary silent phase ramp.
      correction_[k] = k == 0 || strongest <= 1e-60 || previous_energy <= 1e-60 ? 0 :
          std::remainder(correction_[k] - (ratio_ - 1) * frequency * h, two_pi<double>);
      // DC stays DC; bins beyond Nyquist are discarded rather than aliased.
      destination_[k] = static_cast<size_t>(std::floor(static_cast<double>(k) * ratio_ + 0.5));
      // Remap around the window center, avoiding a ratio-dependent translation
      // of each frame's time origin. With positive-sign FFT this has a + sign.
      const auto centering = two_pi<double> *
          (static_cast<double>(destination_[k]) - static_cast<double>(k)) *
          (static_cast<double>(n) - 1) / (2 * static_cast<double>(n));
      rotation_[k] = std::polar(1.0, correction_[k] + centering);
    }
    first_frame_ = false;
    for (size_t c = 0; c < config_.channels; ++c) {
      std::fill(synthesis_.begin(), synthesis_.end(), complex{});
      for (size_t k = 0; k < bins; ++k) {
        const auto dest = destination_[k];
        if (dest >= bins)
          continue;
        auto value = analysis_[c * n + k] * rotation_[k];
        // A real FFT's edge bins have half the energy of a conjugate pair.
        if (k == 0 || k == n / 2) {
          if (dest != 0 && dest != n / 2)
            value *= 0.5;
        } else if (dest == 0 || dest == n / 2) {
          value *= 2;
        }
        synthesis_[dest] += value;
      }
      synthesis_[0] = {synthesis_[0].real(), 0};
      synthesis_[n / 2] = {synthesis_[n / 2].real(), 0};
      for (size_t k = 1; k < n / 2; ++k)
        synthesis_[n - k] = std::conj(synthesis_[k]);
      fft_inplace(std::span<complex>(synthesis_), false);
      // Frame starts at t-N+1, and is scheduled at t+1: exactly N delay.
      // All frames contributing to output sample s+N finish by s+N-1.
      for (size_t i = 0; i < n; ++i) {
        const auto index = c * n + (position_ + i) % n;
        output_[index] = clean(output_[index] + synthesis_[i].real() * window_[i] /
                              normalization_[i % config_.hop_size]);
      }
    }
  }

  configuration config_{};
  bool prepared_ = false;
  bool first_frame_ = true;
  size_t position_ = 0;
  size_t hop_count_ = 0;
  double target_ratio_ = 1;
  double ratio_ = 1;
  double smoothing_ = 0;
  std::vector<double> window_, normalization_, input_, output_, previous_phase_, previous_energy_,
      correction_;
  std::vector<complex> analysis_, synthesis_, rotation_;
  std::vector<size_t> destination_;
};
} // namespace riw
