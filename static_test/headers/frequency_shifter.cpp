#include <riw/dsp/frequency_shifter.hpp>

#include <type_traits>
#include <utility>

using Shifter = riw::frequency_shifter<double>;
static_assert(std::is_default_constructible_v<riw::frequency_shifter<float>>);
static_assert(std::is_default_constructible_v<Shifter>);
static_assert(noexcept(std::declval<Shifter &>().reset()));
static_assert(noexcept(std::declval<Shifter &>().set_shift_hz(1)));
static_assert(noexcept(std::declval<Shifter &>().process({}, {}, 0)));
