#include <riw/concepts/floating_point.hpp>

namespace {
class A;
static_assert(!riw::floating_point<int>);
static_assert(riw::floating_point<float>);
static_assert(!riw::floating_point<A>);
} // namespace
