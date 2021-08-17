#pragma once

#include <type_traits>

namespace riw {
template <class Type>
concept arithmetic = std::is_arithmetic_v<Type>;
}
