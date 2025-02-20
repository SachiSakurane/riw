#pragma once

#include <concepts>

namespace riw {
template <class Type>
class subscription {
public:
  using value_type = Type;

  virtual void receive(value_type v) = 0;
};
} // namespace riw
