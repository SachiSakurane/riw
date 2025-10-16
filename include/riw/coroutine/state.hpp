#pragma once

#include <cppcoro/async_generator.hpp>
#include <cppcoro/single_consumer_event.hpp>
#include <cppcoro/task.hpp>

#include <algorithm>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace riw {

template <typename T>
class state {
public:
  explicit state(T initial) : value_(std::move(initial)) {}

  // 非同期更新（互換のため残す）
  cppcoro::task<void> set(T new_value) {
    set_sync(std::move(new_value));
    co_return;
  }

  // 同期更新：ロック解放後に通知してデッドロック回避
  void set_sync(T new_value) {
    std::vector<std::shared_ptr<cppcoro::single_consumer_event>> to_notify;

    {
      std::lock_guard lock(mutex_);
      value_ = std::move(new_value);

      // 生きているイベントを強参照にコピー（後でロック外で通知）
      to_notify.reserve(events_.size());
      auto it = events_.begin();
      while (it != events_.end()) {
        if (auto sp = it->lock()) {
          to_notify.emplace_back(std::move(sp));
          ++it;
        } else {
          it = events_.erase(it); // ここでついでに掃除
        }
      }
    } // ← ここで mutex_ を解放

    // 通知はロック外で実施（再入で watch() が mutex_ を取りに来ても安全）
    for (auto &e : to_notify) {
      e->set();
    }
  }

  // 現在値の取得
  T get() const {
    std::lock_guard lock(mutex_);
    return value_;
  }

  // 非同期ストリーム購読
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
    co_yield last; // 初回は最新値

    while (true) {
      co_await *event; // まず待つ
      {
        std::lock_guard lock(mutex_);
        co_yield value_; // 値を読むのはロック内
      }
      event->reset(); // 消費後にリセット（取りこぼし防止）
    }
  }

  // ラムダ購読（バックグラウンド常駐）
  template <typename Callback>
  void subscribe(Callback &&cb) {
    std::thread([this, cb = std::forward<Callback>(cb)]() mutable {
      cppcoro::sync_wait([&]() -> cppcoro::task<void> {
        auto gen = watch();
        for (auto it = co_await gen.begin(); it != gen.end(); co_await ++it) {
          cb(*it);
        }
      }());
    }).detach();
  }

private:
  mutable std::mutex mutex_;
  std::vector<std::weak_ptr<cppcoro::single_consumer_event>> events_;
  T value_;
};

} // namespace riw
