#pragma once

#include <chrono>
#include <riw/concepts/floating_point.hpp>

namespace riw {
  template <riw::floating_point FloatType, typename Resolution = std::chrono::milliseconds>
  inline FloatType to_float_seconds(std::chrono::nanoseconds nano) {
    return static_cast<FloatType>(std::chrono::duration_cast<Resolution>(nano).count()) / static_cast<FloatType>(Resolution::period::den);
  }
}
