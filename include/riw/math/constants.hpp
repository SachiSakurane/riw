#pragma once

#include <riw/concepts/floationg_point.hpp>

namespace riw {
// e
template <riw::floating_point FloatType>
constexpr static const FloatType e = static_cast<FloatType>(2.71828182845904523536028747135266);

// pi
template <riw::floating_point FloatType>
constexpr static const FloatType pi = static_cast<FloatType>(3.14159265358979323846264338327950288);

template <riw::floating_point FloatType>
constexpr static const FloatType two_pi = pi<FloatType> *static_cast<FloatType>(2.0);

template <riw::floating_point FloatType>
constexpr static const FloatType half_pi = pi<FloatType> *static_cast<FloatType>(0.5);

template <riw::floating_point FloatType>
constexpr static const FloatType quarter_pi = pi<FloatType> *static_cast<FloatType>(0.25);

// invert pi
template <riw::floating_point FloatType>
constexpr static const FloatType inv_pi = static_cast<FloatType>(1.0) / pi<FloatType>;

template <riw::floating_point FloatType>
constexpr static const FloatType inv_two_pi = static_cast<FloatType>(1.0) / two_pi<FloatType>;

template <riw::floating_point FloatType>
constexpr static const FloatType inv_half_pi = static_cast<FloatType>(1.0) / half_pi<FloatType>;

template <riw::floating_point FloatType>
constexpr static const FloatType inv_quarter_pi = static_cast<FloatType>(1.0) / quarter_pi<FloatType>;

// root
template <riw::floating_point FloatType>
constexpr static const FloatType root_two = static_cast<FloatType>(1.41421356237309504880168872420969808);

template <riw::floating_point FloatType>
constexpr static const FloatType inv_root_two = static_cast<FloatType>(1.0) / root_two<FloatType>;

// ln
template <riw::floating_point FloatType>
constexpr static const FloatType ln_two = static_cast<FloatType>(0.693147180559945309429);

template <riw::floating_point FloatType>
constexpr static const FloatType ln_ten = static_cast<FloatType>(2.30258509299404568404);
} // namespace riw