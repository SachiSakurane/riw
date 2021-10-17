#pragma once

#include <cassert>
#include <cmath>
#include <complex>

#include <riw/bit/bit.hpp>
#include <riw/concepts/convertible_to.hpp>
#include <riw/math/constants.hpp>

namespace riw {
template <class SrcContainer, class DstContainer, bool IsForward = true>
requires riw::convertible_to<typename SrcContainer::value_type,
                             std::complex<typename SrcContainer::value_type::value_type>> &&
    riw::convertible_to<typename DstContainer::value_type,
                        std::complex<typename DstContainer::value_type::value_type>>
constexpr inline void fft(const SrcContainer &src, DstContainer &dst) {
  using complex_type = typename DstContainer::value_type;
  using value_type = typename complex_type::value_type;

  assert((src.size() & (src.size() - 1)) == 0 && "invalid input size");
  size_t cnt = src.size();
  size_t msb = riw::bit_width(cnt) - 1;
  value_type nrm = static_cast<value_type>(1) / std::sqrt(static_cast<value_type>(cnt));

  // pre-process the input data
  for (size_t j = 0; j < cnt; ++j)
    dst[j] = nrm * src[riw::bit_reverse(j, msb)];

  // fft passes
  for (size_t i = 0; i < msb; ++i) {
    size_t bm = 1 << i; // butterfly mask
    size_t bw = 2 << i; // butterfly width
    value_type ang =
        static_cast<value_type>(IsForward ? riw::pi<value_type> : -riw::pi<value_type>) /
        static_cast<value_type>(bm);

    // fft butterflies
    for (size_t j = 0; j < (cnt / 2); ++j) {
      size_t i1 = ((j >> i) << (i + 1)) + j % bm; // left wing
      size_t i2 = i1 ^ bm;                        // right wing
      complex_type z1 = std::polar(static_cast<value_type>(1),
                                   ang * static_cast<value_type>(i1 ^ bw)); // left wing rotation
      complex_type z2 = std::polar(static_cast<value_type>(1),
                                   ang * static_cast<value_type>(i2 ^ bw)); // right wing rotation
      complex_type tmp = dst[i1];

      dst[i1] += z1 * dst[i2];
      dst[i2] = tmp + z2 * dst[i2];
    }
  }
}

} // namespace riw
