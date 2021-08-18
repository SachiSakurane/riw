#pragma once

#include <riw/concepts/floating_point.hpp>

namespace riw {
struct cubic {
  constexpr decltype(auto) operator()(riw::floating_point auto v) { return v * v * v; }
};
} // namespace riw
