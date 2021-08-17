#pragma once

#include <riw/concepts/floationg_point.hpp>

namespace riw {
struct quadratic {
  constexpr decltype(auto) operator()(riw::floating_point auto v) { return v * v; }
};
} // namespace riw
