#pragma once

#include <concepts>
#include <memory>

#include <riw/notify/subscription.hpp>

namespace riw {
template <class ObservableType>
concept observable =
    std::copy_constructible<typename ObservableType::value_type> &&
    requires(ObservableType o,
             std::weak_ptr<riw::subscription<typename ObservableType::value_type>> s) {
      { o.observe() } -> std::convertible_to<typename ObservableType::value_type>;
      o.add_subscribe(s);
    };
} // namespace riw
