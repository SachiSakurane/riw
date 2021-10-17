#pragma once

#include <type_traits>

#include <riw/concepts/integral.hpp>

namespace riw {
template <class Type>
concept signed_integral = riw::integral<Type> && std::is_signed_v<Type>;
}
