#pragma once

#include <algorithm>
#include <vector>

#include <riw/notify/concepts/observable.hpp>
#include <riw/notify/subscription.hpp>

namespace riw {
template <std::copy_constructible Type>
class observer {
public:
  using value_type = Type;
  using subscription_type = riw::subscription<Type>;

  virtual ~observer() = default;

  void add_subscribe(std::weak_ptr<subscription_type> s) {
    auto &ss = subscriptions.emplace_back(s);
    if (auto l = ss.lock(); l) {
      l->receive(observe());
    }
  }

  virtual value_type observe() const = 0;

  virtual void notify(value_type v) {
    fetch();

    for (auto s : subscriptions) {
      if (auto l = s.lock(); l) {
        l->receive(v);
      }
    }
  }

  std::size_t subscriptions_count() const {
    return std::count_if(std::begin(subscriptions), std::end(subscriptions),
                         [](auto s) { return !s.expired(); });
  };

  void fetch() {
    subscriptions.erase(std::remove_if(std::begin(subscriptions), std::end(subscriptions),
                                       [](auto s) { return s.expired(); }),
                        std::end(subscriptions));
  }

protected:
  std::vector<std::weak_ptr<subscription_type>> subscriptions;

private:
};
} // namespace riw
