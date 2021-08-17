#include <riw/concepts/arithmetic.hpp>

namespace {
class A;
static_assert(riw::arithmetic<int>);
static_assert(riw::arithmetic<float>);
static_assert(!riw::arithmetic<A>);
} // namespace
