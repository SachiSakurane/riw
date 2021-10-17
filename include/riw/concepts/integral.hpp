#pragma once

#include <type_traits>

namespace riw {
template <class Type>
concept integral = std::is_integral_v<Type>;
}
