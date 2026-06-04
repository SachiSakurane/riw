#include <riw/dsp/biquad.hpp>
#include <riw/dsp/window.hpp>
#include <riw/value_range/functions.hpp>

#include <array>

namespace {
static_assert(riw::length(riw::value_range{1.0, 3.0}) == 2.0);
static_assert(riw::inverse(0.25) == 0.75);

[[maybe_unused]] void use_dsp_headers() {
  [[maybe_unused]] auto coefficients = riw::make_lowpass_coefficients(48000.0, 1000.0, 0.707);

  std::array<double, 4> window{};
  riw::hann_inplace(window);
}
} // namespace
