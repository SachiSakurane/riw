#pragma once

#include <atomic>
#include <memory>

namespace riw {
class subscription {
public:
  explicit subscription(std::shared_ptr<std::atomic_bool> active) : active_(std::move(active)) {}

  ~subscription() { unsubscribe(); }

  void unsubscribe() const noexcept {
    if (active_)
      active_->store(false);
  }

  bool is_active() const noexcept { return active_ && active_->load(); }

  subscription(subscription &&) noexcept = default;
  subscription &operator=(subscription &&) noexcept = default;

  subscription(const subscription &) = delete;
  subscription &operator=(const subscription &) = delete;

private:
  std::shared_ptr<std::atomic_bool> active_;
};
} // namespace riw
