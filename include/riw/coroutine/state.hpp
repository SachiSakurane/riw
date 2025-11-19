#pragma once

#include <riw/coroutine/subscription.hpp>
#include <cppcoro/async_generator.hpp>
#include <cppcoro/single_consumer_event.hpp>
#include <cppcoro/task.hpp>

#include <algorithm>
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace riw {

template <typename T>
class state {
public:
  explicit state(T initial) : value_(std::move(initial)) {}

  void set_sync(T new_value) {
    std::vector<std::shared_ptr<cppcoro::single_consumer_event>> to_notify;

    {
      std::lock_guard lock(mutex_);
      value_ = std::move(new_value);
      for (auto it = events_.begin(); it != events_.end();) {
        if (auto sp = it->lock()) {
          to_notify.emplace_back(sp);
          ++it;
        } else {
          it = events_.erase(it);
        }
      }
    }

    for (auto &e : to_notify) e->set();
  }

  cppcoro::task<void> set(T new_value) {
    set_sync(std::move(new_value));
    co_return;
  }

  T get() const {
    std::lock_guard lock(mutex_);
    return value_;
  }

  cppcoro::async_generator<T> watch() {
    auto event = std::make_shared<cppcoro::single_consumer_event>();
    {
      std::lock_guard lock(mutex_);
      events_.push_back(event);
    }

    T last;
    {
      std::lock_guard lock(mutex_);
      last = value_;
    }

    co_yield last;

    while (true) {
      co_await *event;
      {
        std::lock_guard lock(mutex_);
        co_yield value_;
      }
      event->reset();
    }
  }

  template <typename Callback>
  [[nodiscard]] subscription subscribe(Callback&& cb) {
    auto active = std::make_shared<std::atomic_bool>(true);

    std::thread([this, cb = std::forward<Callback>(cb), active]() mutable {
      cppcoro::sync_wait([&, active]() -> cppcoro::task<void> {
        auto gen = watch();
        auto it = co_await gen.begin();
        while (it != gen.end()) {
          if (!active->load()) co_return;
          cb(*it);
          co_await ++it;
        }
      }());
    }).detach();

    return subscription(active);
  }

private:
  mutable std::mutex mutex_;
  std::vector<std::weak_ptr<cppcoro::single_consumer_event>> events_;
  T value_;
};

} // namespace riw
