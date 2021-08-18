#pragma once

#include <riw/concepts/floating_point.hpp>

namespace riw {
struct quadratic {
  constexpr decltype(auto) operator()(riw::floating_point auto v) { return v * v; }
};
} // namespace riw
