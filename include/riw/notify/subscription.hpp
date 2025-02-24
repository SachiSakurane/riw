#pragma once

namespace riw {
template <class Type>
class subscription {
public:
  using value_type = Type;

  virtual ~subscription() {}

  virtual void receive(value_type v) = 0;
};
} // namespace riw
