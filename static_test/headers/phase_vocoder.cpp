#include <riw/dsp/phase_vocoder.hpp>

#include <type_traits>

static_assert(std::is_default_constructible_v<riw::phase_vocoder_pitch_shifter<float>>);
static_assert(std::is_default_constructible_v<riw::phase_vocoder_pitch_shifter<double>>);
static_assert(riw::phase_vocoder_pitch_shifter<>::minimum_pitch_ratio == 0.5);
