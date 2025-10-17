#pragma once

#include <mutex>
#include <riw/coroutine/subscription.hpp>
#include <vector>

namespace riw {

class subscription_manager {
public:
  subscription_manager() = default;
  ~subscription_manager() { unsubscribe_all(); }

  // 購読を追加
  void add(subscription &&s) {
    std::lock_guard lock(mutex_);
    subs_.emplace_back(std::move(s));
  }

  // すべて解除
  void unsubscribe_all() {
    std::lock_guard lock(mutex_);
    for (auto &s : subs_) {
      s.unsubscribe();
    }
    subs_.clear();
  }

private:
  std::vector<subscription> subs_;
  std::mutex mutex_;
};

} // namespace riw
