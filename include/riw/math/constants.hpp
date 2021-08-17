#pragma once

#include <riw/concepts/floationg_point.hpp>

namespace riw {
// e
template <riw::floating_point _T>
constexpr static const _T e = static_cast<_T>(2.71828182845904523536028747135266);

// pi
template <riw::floating_point _T>
constexpr static const _T pi = static_cast<_T>(3.14159265358979323846264338327950288);

template <riw::floating_point _T>
constexpr static const _T two_pi = pi<_T> *static_cast<_T>(2.0);

template <riw::floating_point _T>
constexpr static const _T half_pi = pi<_T> *static_cast<_T>(0.5);

template <riw::floating_point _T>
constexpr static const _T quarter_pi = pi<_T> *static_cast<_T>(0.25);

// invert pi
template <riw::floating_point _T>
constexpr static const _T inv_pi = static_cast<_T>(1.0) / pi<_T>;

template <riw::floating_point _T>
constexpr static const _T inv_two_pi = static_cast<_T>(1.0) / two_pi<_T>;

template <riw::floating_point _T>
constexpr static const _T inv_half_pi = static_cast<_T>(1.0) / half_pi<_T>;

template <riw::floating_point _T>
constexpr static const _T inv_quarter_pi = static_cast<_T>(1.0) / quarter_pi<_T>;

// root
template <riw::floating_point _T>
constexpr static const _T root_two = static_cast<_T>(1.41421356237309504880168872420969808);

template <riw::floating_point _T>
constexpr static const _T inv_root_two = static_cast<_T>(1.0) / root_two<_T>;

// ln
template <riw::floating_point _T>
constexpr static const _T ln_two = static_cast<_T>(0.693147180559945309429);

template <riw::floating_point _T>
constexpr static const _T ln_ten = static_cast<_T>(2.30258509299404568404);
} // namespace riw