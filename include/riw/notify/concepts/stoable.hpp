#pragma once

#include <type_traits>

#include <riw/notify/store.hpp>

namespace riw {
template <class Type>
concept stoable = std::is_base_of_v<riw::store, Type>;
}
