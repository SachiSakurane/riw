#pragma once

#include <vector>

#include <riw/notify/concepts/observable.hpp>
#include <riw/notify/subsctiption.hpp>

namespace riw {
template <std::copy_constructible Type>
class observer {
public:
  using value_type = Type;
  using subscription_type = riw::subscription<Type>;

  void add_subscription(subscription_type &s) {
    subscriptions.emplace_back(s);
    s.receive(observe());
  }

  void remove_subscription(subscription_type &s) {
    if (auto it = std::remove_if(std::begin(subscriptions), std::end(subscriptions),
                                 [&s](const auto &ss) { return &(ss.get()) == &s; });
        it != std::end(subscriptions)) {
      subscriptions.erase(it);
    }
  }

  virtual value_type observe() const = 0;

  virtual void notify(value_type v) {
    for (auto &sub : subscriptions) {
      sub.get().receive(v);
    }
  }

  std::size_t subscriptions_count() const { return subscriptions.size(); };

private:
  std::vector<std::reference_wrapper<subscription_type>> subscriptions;
};
} // namespace riw
