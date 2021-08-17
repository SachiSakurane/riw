#include <riw/concepts/floationg_point.hpp>

namespace {
class A;
static_assert(!riw::floating_point<int>);
static_assert(riw::floating_point<float>);
static_assert(!riw::floating_point<A>);
} // namespace
