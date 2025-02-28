#pragma once

#include <concepts>
#include <memory>

#include <riw/notify/subscription.hpp>

namespace riw {
template <class ObservableType>
concept observable =
    std::copy_constructible<typename ObservableType::value_type> &&
    requires(ObservableType o, const ObservableType co,
             std::weak_ptr<riw::subscription<typename ObservableType::value_type>> s,
             typename ObservableType::value_type v) {
      { co.observe() } -> std::convertible_to<typename ObservableType::value_type>;
      { co.subscriptions_count() } -> std::convertible_to<std::size_t>;
      o.subscribe(s);
      o.notify(v);
      o.fetch();
    };
} // namespace riw
