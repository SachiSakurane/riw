#pragma once

#include <algorithm>
#include <concepts>
#include <type_traits>

#include <riw/notify/observer.hpp>

namespace riw {
template <std::copy_constructible Type>
class behavior : public riw::observer<Type> {
public:
  using value_type = riw::observer<Type>::value_type;

  constexpr behavior(value_type initial_value) : current_value{initial_value} {}

  template <class... Args>
  constexpr behavior(Args &&...args) : current_value{std::forward<Args>(args)...} {}

  behavior(const behavior &) = default;
  behavior(behavior &&) = default;

  behavior &operator=(const value_type &value) {
    this->current_value = value;
    this->notify(observe());
    return *this;
  }
  behavior &operator=(value_type &&value) {
    this->current_value = std::move(value);
    this->notify(observe());
    return *this;
  }

  operator value_type() const { return current_value; }
  value_type observe() const override { return current_value; }

private:
  value_type current_value;
};

} // namespace riw
