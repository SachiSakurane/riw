#pragma once

#include <tuple>
#include <utility>

namespace riw {
namespace detail {
  template <class... Ts, class... Us, std::size_t... Is>
  constexpr inline auto tuple_pair_impl(const std::tuple<Ts...> &t1, const std::tuple<Us...> &t2,
                                        std::index_sequence<Is...>) {
    return std::make_tuple(std::make_pair(std::get<Is>(t1), std::get<Is>(t2))...);
  }
} // namespace detail

template <class... Ts, class... Us>
constexpr inline auto tuple_pair(const std::tuple<Ts...> &t1, const std::tuple<Us...> &t2) {
  static_assert(sizeof...(Ts) == sizeof...(Us), "Tuples must have the same number of elements");
  return detail::tuple_pair_impl(t1, t2, std::index_sequence_for<Ts...>{});
}
} // namespace riw
