#pragma once

#include <riw/concepts/floationg_point.hpp>

namespace riw {
struct linear {
  constexpr decltype(auto) operator()(riw::floating_point auto v) { return v; }
};
} // namespace riw
