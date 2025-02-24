#pragma once

#include <functional>

#include <riw/notify/observer.hpp>

namespace riw {
template <std::copy_constructible Type, observable ObserverType = riw::observer<Type>>
class accessor : public ObserverType {
public:
  using value_type = Type;

  accessor(std::function<value_type()> &&get, std::function<void(value_type)> &&set)
      : getter{std::move(get)}, setter{std::move(set)} {}

  accessor &operator=(const value_type &value) {
    this->setter(value);
    this->notify(observe());
    return *this;
  }
  accessor &operator=(value_type &&value) {
    this->setter(value);
    this->notify(observe());
    return *this;
  }

  operator value_type() const { return getter(); }
  value_type observe() const override { return getter(); }

private:
  const std::function<value_type()> getter;
  const std::function<void(value_type)> setter;
};
} // namespace riw
