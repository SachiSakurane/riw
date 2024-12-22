#pragma once

#include <riw/value_range/value_range.hpp>

namespace riw {
template <class Type>
static constexpr value_range normal_range = {static_cast<Type>(0), static_cast<Type>(1)};
}