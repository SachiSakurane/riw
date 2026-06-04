#include <array>
#include <cmath>
#include <complex>
#include <vector>

#include <gtest/gtest.h>
#include <riw/math/fft.hpp>

namespace {
template <class ComplexType>
void expect_complex_near(const ComplexType &actual, const ComplexType &expected,
                         typename ComplexType::value_type tolerance) {
  EXPECT_NEAR(actual.real(), expected.real(), tolerance);
  EXPECT_NEAR(actual.imag(), expected.imag(), tolerance);
}
} // namespace

TEST(Math_FftTest, MatchesKnownFourPointResult) {
  using complex = std::complex<double>;
  const std::array<complex, 4> src{complex{1.0, 0.0}, complex{2.0, 0.0}, complex{3.0, 0.0},
                                  complex{4.0, 0.0}};
  std::array<complex, 4> dst{};

  riw::fft(src, dst);

  expect_complex_near(dst[0], complex{5.0, 0.0}, 1e-12);
  expect_complex_near(dst[1], complex{-1.0, -1.0}, 1e-12);
  expect_complex_near(dst[2], complex{-1.0, 0.0}, 1e-12);
  expect_complex_near(dst[3], complex{-1.0, 1.0}, 1e-12);
}

TEST(Math_FftTest, RoundTripPowerOfTwoSize) {
  using complex = std::complex<double>;
  const std::vector<complex> src{complex{0.0, 1.0}, complex{1.0, 2.0}, complex{2.0, 3.0},
                                 complex{3.0, 4.0}, complex{4.0, 5.0}, complex{5.0, 6.0},
                                 complex{6.0, 7.0}, complex{7.0, 8.0}};
  std::vector<complex> spectrum(src.size());
  std::vector<complex> actual(src.size());

  riw::fft(src, spectrum);
  riw::fft<std::vector<complex>, std::vector<complex>, false>(spectrum, actual);

  for (size_t i = 0; i < src.size(); ++i)
    expect_complex_near(actual[i], src[i], 1e-12);
}

TEST(Math_FftTest, RoundTripMixedRadixSize) {
  using complex = std::complex<double>;
  const std::vector<complex> src{complex{1.0, 0.0},  complex{0.5, 0.25}, complex{-1.0, 0.5},
                                 complex{2.0, -0.5}, complex{0.0, 1.0},  complex{3.0, 2.0}};
  std::vector<complex> spectrum(src.size());
  std::vector<complex> actual(src.size());

  riw::fft(src, spectrum);
  riw::fft<std::vector<complex>, std::vector<complex>, false>(spectrum, actual);

  for (size_t i = 0; i < src.size(); ++i)
    expect_complex_near(actual[i], src[i], 1e-12);
}

TEST(Math_FftTest, RoundTripLargeMixedRadixSize) {
  using complex = std::complex<double>;
  std::vector<complex> src(18);
  for (size_t i = 0; i < src.size(); ++i)
    src[i] = complex{std::sin(static_cast<double>(i)), std::cos(static_cast<double>(2 * i))};

  std::vector<complex> spectrum(src.size());
  std::vector<complex> actual(src.size());

  riw::fft(src, spectrum);
  riw::fft<std::vector<complex>, std::vector<complex>, false>(spectrum, actual);

  for (size_t i = 0; i < src.size(); ++i)
    expect_complex_near(actual[i], src[i], 1e-12);
}

TEST(Math_FftTest, SupportsFloatComplexContainers) {
  using complex = std::complex<float>;
  const std::vector<complex> src{complex{1.0F, 0.0F}, complex{2.0F, -1.0F}, complex{0.5F, 0.25F},
                                 complex{3.0F, 2.0F}, complex{-1.0F, 0.0F}};
  std::vector<complex> spectrum(src.size());
  std::vector<complex> actual(src.size());

  riw::fft(src, spectrum);
  riw::fft<std::vector<complex>, std::vector<complex>, false>(spectrum, actual);

  for (size_t i = 0; i < src.size(); ++i)
    expect_complex_near(actual[i], src[i], 1e-5F);
}
