#pragma once

#include <concepts>

#include <riw/math/constants.hpp>

namespace riw {
template <class ContainerType>
constexpr void hann_inplace(ContainerType &srcdst) {
  using FloatType = typename ContainerType::value_type;
  for (auto n = 0u; n < srcdst.size(); ++n) {
    srcdst[n] = static_cast<FloatType>(0.5) *
                (static_cast<FloatType>(1) -
                 std::cos(riw::two_pi<FloatType> * n / static_cast<FloatType>(srcdst.size() - 1)));
  }
}
} // namespace riw
