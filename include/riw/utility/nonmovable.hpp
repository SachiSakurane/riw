#pragma once

namespace riw {
template <typename Type>
class nonmovable {
protected:
  nonmovable() = default;
  virtual ~nonmovable() = default;

private:
  nonmovable(nonmovable &&) = delete;
  nonmovable &operator=(nonmovable &&) = delete;
};

} // namespace riw
