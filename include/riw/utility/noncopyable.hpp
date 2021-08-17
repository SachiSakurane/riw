#pragma once

namespace riw {
template <typename Type>
class noncopyable {
protected:
  noncopyable() = default;
  virtual ~noncopyable() = default;

private:
  noncopyable(const noncopyable &) = delete;
  noncopyable &operator=(const noncopyable &) = delete;
};

} // namespace riw
