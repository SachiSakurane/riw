#pragma once

#include <type_traits>

namespace riw {
template <class Type>
concept enumeration = std::is_enum_v<Type>;
}
