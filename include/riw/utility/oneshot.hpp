#pragma once

#include <chrono>
#include <cmath>

#include <riw/easing.hpp>

namespace riw {
template <std::floating_point ValueType = float>
class oneshot {
public:
  explicit oneshot(const std::chrono::milliseconds dur) : duration{dur} {}

  decltype(auto) get_duration() const { return duration; }
  void set_duration(std::chrono::milliseconds dur) { duration = dur; }

  bool is_end() const { return duration < (std::chrono::steady_clock::now() - update_time); }

  template <class Easing = riw::quint>
  ValueType get_out() const {
    const auto d = std::chrono::steady_clock::now() - update_time;
    const auto v = std::clamp(
        static_cast<ValueType>(std::chrono::duration_cast<std::chrono::milliseconds>(d).count()) /
            static_cast<float>(duration.count()),
        static_cast<ValueType>(0), static_cast<ValueType>(1));
    return riw::ease_out<Easing>(v);
  }

  void reset() { update_time = std::chrono::steady_clock::now(); }

private:
  std::chrono::milliseconds duration;
  std::chrono::steady_clock::time_point update_time{std::chrono::steady_clock::now()};
};
} // namespace riw
