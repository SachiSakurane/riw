#pragma once

#include <any>
#include <vector>

#include <riw/notify/concepts/stoable.hpp>

namespace riw {
class storage {
public:
  storage() {}

  storage &operator+=(riw::stoable auto &&s) {
    this->store.emplace_back(std::move(s));
    return *this;
  }

  std::size_t size() const { return store.size(); }

private:
  std::vector<std::any> store;
};
} // namespace riw
