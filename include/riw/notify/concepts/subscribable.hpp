#pragma once

#include <concepts>

namespace riw {
template <class S>
concept subscribable = std::copy_constructible<typename S::value_type> &&
                       requires(S s, typename S::value_type v) { s.receive(v); };
} // namespace riw
