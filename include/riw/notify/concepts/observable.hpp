#pragma once

#include <concepts>

namespace riw {
template <class ObservableType>
concept observable =
    std::copy_constructible<typename ObservableType::value_type> &&
    requires(ObservableType o, typename ObservableType::subscription_type &subscription) {
      { o.observe() } -> std::same_as<typename ObservableType::value_type>;
      o.add_subscription(subscription);
      o.remove_subscription(subscription);
    };
} // namespace riw
