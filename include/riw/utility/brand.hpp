#pragma once

#include <concepts>
#include <type_traits>

#include <riw/utility/template_string_literal.hpp>

namespace riw {
template <class T, template_string_literal B, class = void>
struct brand;

template <class T, template_string_literal B>
struct brand<T, B, std::enable_if_t<std::is_class_v<T> && !std::is_final_v<T>>> : T {
  static constexpr template_string_literal __brand = B;
};

template <class T, template_string_literal B>
struct brand<T, B, std::enable_if_t<std::is_arithmetic_v<T>>> {
  using value_type = T;

  using reference = brand<T, B> &;
  using const_reference = const brand<T, B> &;

  constexpr brand() noexcept : value() {}
  constexpr brand(value_type v) noexcept : value(v) {}

  brand(const brand &) = default;
  brand &operator=(const brand &) = default;
  brand(brand &&) = default;
  brand &operator=(brand &&) = default;

  operator value_type() const { return value; }

  reference operator++() {
    ++value;
    return (*this);
  }
  reference operator--() {
    --value;
    return (*this);
  }

  brand operator++(int) {
    brand temp(*this);
    ++*this;
    return temp;
  }
  brand operator--(int) {
    brand temp(*this);
    --*this;
    return temp;
  }

  brand operator~() const { return ~value; }
  brand operator-() const { return -value; }
  brand operator+() const { return +value; }

  brand &operator*=(const brand &v) {
    value *= v.value;
    return (*this);
  }
  brand &operator/=(const brand &v) {
    value /= v.value;
    return (*this);
  }
  brand &operator%=(const brand &v) {
    value %= v.value;
    return (*this);
  }
  brand &operator+=(const brand &v) {
    value += v.value;
    return (*this);
  }
  brand &operator-=(const brand &v) {
    value -= v.value;
    return (*this);
  }
  brand &operator<<=(const brand &v) {
    value <<= v.value;
    return (*this);
  }
  brand &operator>>=(const brand &v) {
    value >>= v.value;
    return (*this);
  }
  brand &operator&=(const brand &v) {
    value &= v.value;
    return (*this);
  }
  brand &operator|=(const brand &v) {
    value |= v.value;
    return (*this);
  }
  brand &operator^=(const brand &v) {
    value ^= v.value;
    return (*this);
  }

  explicit operator bool() const noexcept { return value; }
  bool operator!() const noexcept { return !static_cast<bool>(*this); }

  T value;
  static constexpr template_string_literal __brand = B;
};

template <class T, template_string_literal B>
brand<T, B> operator*(const brand<T, B> &t1, const brand<T, B> &t2) {
  return brand<T, B>(t1) *= t2;
}

template <class T, template_string_literal B>
brand<T, B> operator/(const brand<T, B> &t1, const brand<T, B> &t2) {
  return brand<T, B>(t1) /= t2;
}

template <class T, template_string_literal B>
brand<T, B> operator%(const brand<T, B> &t1, const brand<T, B> &t2) {
  return brand<T, B>(t1) %= t2;
}

template <class T, template_string_literal B>
brand<T, B> operator+(const brand<T, B> &t1, const brand<T, B> &t2) {
  return brand<T, B>(t1) += t2;
}

template <class T, template_string_literal B>
brand<T, B> operator-(const brand<T, B> &t1, const brand<T, B> &t2) {
  return brand<T, B>(t1) -= t2;
}

template <class T, template_string_literal B>
brand<T, B> operator<<(const brand<T, B> &t, std::size_t n) {
  return brand<T, B>(t) <<= n;
}

template <class T, template_string_literal B>
brand<T, B> operator>>(const brand<T, B> &t, std::size_t n) {
  return brand<T, B>(t) >>= n;
}

template <class T, template_string_literal B>
bool operator==(const brand<T, B> &t1, const brand<T, B> &t2) {
  return t1.value == t2.value;
}

template <class T, template_string_literal B>
bool operator<(const brand<T, B> &t1, const brand<T, B> &t2) {
  return t1.value < t2.value;
}

template <class T, template_string_literal B>
bool operator<=>(const brand<T, B> &t1, const brand<T, B> &t2) {
  return t1.value <=> t2.value;
}

} // namespace riw
