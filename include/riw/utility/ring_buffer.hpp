#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

#include <riw/utility/noncopyable.hpp>

namespace riw {
template <class ValueType>
class ring_buffer : private noncopyable<ring_buffer<ValueType>> {
public:
  void resize(std::size_t size) {
    buffer.resize(size);
    index = 0;
  }

  void push_block(const std::vector<ValueType> &block, unsigned size) {
    if (buffer.empty()) {
      return;
    }

    for (unsigned i = 0; i < block.size() && i < size; ++i) {
      buffer[(index + i) % buffer.size()] = block[i];
    }

    index = (index + std::min(static_cast<unsigned>(block.size()), size)) % buffer.size();
  }

  void copy_to(std::vector<ValueType> &dst) const {
    for (unsigned s = 0; s < dst.size() && s < buffer.size(); ++s) {
      dst[s] = buffer[(index + s) % buffer.size()];
    }
  }

private:
  unsigned index{0};
  std::vector<ValueType> buffer;
};
} // namespace riw
