#pragma once

#include <concepts>

namespace riw {
template <class S>
concept subscribable = std::copy_constructible<typename S::value_type> &&
                       requires(S s) { s.receive(std::declval<typename S::value_type>); };
} // namespace riw
