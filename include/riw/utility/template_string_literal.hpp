#pragma once

#include <cstddef>

namespace riw {
template <std::size_t Size>
struct template_string_literal {
  static constexpr int length = Size - 1;

  constexpr template_string_literal(const char (&s_literal)[Size]) {
    for (int i = 0; i < Size; i++)
      buf[i] = s_literal[i];
    buf[length] = '\0';
  }

  char buf[Size];
};
} // namespace utility
