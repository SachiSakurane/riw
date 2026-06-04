#pragma once

#include <cassert>
#include <cmath>
#include <complex>
#include <cstddef>
#include <utility>
#include <vector>

#include <riw/concepts/convertible_to.hpp>
#include <riw/math/constants.hpp>

namespace riw {
namespace detail {
inline bool is_power_of_two(size_t n) { return n != 0 && (n & (n - 1)) == 0; }

inline size_t reverse_bits(size_t value, size_t bit_count) {
  size_t result = 0;
  for (size_t i = 0; i < bit_count; ++i) {
    result = (result << 1) | (value & 1);
    value >>= 1;
  }
  return result;
}

template <class ComplexType>
inline ComplexType unit_root(size_t numerator, size_t denominator, bool is_forward) {
  using value_type = typename ComplexType::value_type;
  const auto sign = static_cast<value_type>(is_forward ? 1 : -1);
  const auto angle = sign * riw::two_pi<value_type> * static_cast<value_type>(numerator) /
                     static_cast<value_type>(denominator);
  return std::polar(static_cast<value_type>(1), angle);
}

template <class ComplexType>
inline void fft_power_of_two(std::vector<ComplexType> &values, bool is_forward) {
  const auto n = values.size();
  size_t bit_count = 0;
  for (auto width = n; width > 1; width >>= 1)
    ++bit_count;

  for (size_t i = 0; i < n; ++i) {
    const auto j = reverse_bits(i, bit_count);
    if (i < j)
      std::swap(values[i], values[j]);
  }

  for (size_t width = 2; width <= n; width <<= 1) {
    const auto step = unit_root<ComplexType>(1, width, is_forward);
    const auto half_width = width >> 1;

    for (size_t offset = 0; offset < n; offset += width) {
      auto twiddle = ComplexType{1};
      for (size_t i = 0; i < half_width; ++i) {
        const auto even = values[offset + i];
        const auto odd = values[offset + i + half_width] * twiddle;
        values[offset + i] = even + odd;
        values[offset + i + half_width] = even - odd;
        twiddle *= step;
      }
    }
  }
}

inline size_t smallest_factor(size_t n) {
  if ((n % 2) == 0)
    return 2;

  for (size_t i = 3; i <= n / i; i += 2) {
    if ((n % i) == 0)
      return i;
  }

  return n;
}

template <class ComplexType>
inline void dft(const std::vector<ComplexType> &src, std::vector<ComplexType> &dst, bool is_forward) {
  const auto n = src.size();
  dst.assign(n, {});

  for (size_t k = 0; k < n; ++k) {
    const auto step = unit_root<ComplexType>(k, n, is_forward);
    auto twiddle = ComplexType{1};
    auto sum = ComplexType{};
    for (size_t j = 0; j < n; ++j) {
      sum += src[j] * twiddle;
      twiddle *= step;
    }
    dst[k] = sum;
  }
}

template <class ComplexType>
inline void fft_impl(const std::vector<ComplexType> &src, std::vector<ComplexType> &dst,
                     bool is_forward) {
  const auto n = src.size();
  if (is_power_of_two(n)) {
    dst = src;
    fft_power_of_two(dst, is_forward);
    return;
  }

  if (n <= 16) {
    dft(src, dst, is_forward);
    return;
  }

  const auto radix = smallest_factor(n);
  if (radix == n) {
    dft(src, dst, is_forward);
    return;
  }

  const auto rows = n / radix;
  std::vector<ComplexType> row_src(radix);
  std::vector<ComplexType> row_dst(radix);
  std::vector<ComplexType> work(n);

  for (size_t row = 0; row < rows; ++row) {
    for (size_t col = 0; col < radix; ++col)
      row_src[col] = src[row + rows * col];

    dft(row_src, row_dst, is_forward);

    for (size_t col = 0; col < radix; ++col)
      work[row * radix + col] = row_dst[col];
  }

  std::vector<ComplexType> col_src(rows);
  std::vector<ComplexType> col_dst(rows);
  dst.assign(n, {});

  for (size_t col = 0; col < radix; ++col) {
    for (size_t row = 0; row < rows; ++row)
      col_src[row] = work[row * radix + col] * unit_root<ComplexType>(row * col, n, is_forward);

    fft_impl(col_src, col_dst, is_forward);

    for (size_t row = 0; row < rows; ++row)
      dst[col + radix * row] = col_dst[row];
  }
}
} // namespace detail

template <class SrcContainer, class DstContainer, bool IsForward = true>
requires riw::convertible_to<typename SrcContainer::value_type,
                             std::complex<typename SrcContainer::value_type::value_type>> &&
    riw::convertible_to<typename DstContainer::value_type,
                        std::complex<typename DstContainer::value_type::value_type>>
inline void fft(const SrcContainer &src, DstContainer &dst) {
  using complex_type = typename DstContainer::value_type;
  using value_type = typename complex_type::value_type;

  assert(src.size() == dst.size() && "invalid output size");
  const auto size = src.size();
  if (size == 0)
    return;

  std::vector<complex_type> work_src(size);
  std::vector<complex_type> work_dst;
  for (size_t i = 0; i < size; ++i)
    work_src[i] = static_cast<complex_type>(src[i]);

  detail::fft_impl(work_src, work_dst, IsForward);

  const auto norm = static_cast<value_type>(1) / std::sqrt(static_cast<value_type>(size));
  for (size_t i = 0; i < size; ++i)
    dst[i] = norm * work_dst[i];
}

} // namespace riw
