#pragma once

#include <riw/concepts/floationg_point.hpp>

namespace riw {
struct quart {
  constexpr decltype(auto) operator()(riw::floating_point auto v) { return v * v * v * v; }
};
} // namespace riw
