#pragma once

#include <riw/concepts/integral.hpp>
#include <riw/concepts/signed_integral.hpp>

namespace riw {
template <class Type>
concept unsigned_integral = riw::integral<Type> && !riw::signed_integral<Type>;
}
