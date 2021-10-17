#include <riw/bit/bit.hpp>

namespace {
// countl_zero test
static_assert(riw::countl_zero(0b1000'0000'0000'0000'0000'0000'0000'0000u) == 0);
static_assert(riw::countl_zero(0b0100'0000'0000'0000'0000'0000'0000'0000u) == 1);
static_assert(riw::countl_zero(0u) == 32);

// bit_width test
static_assert(riw::bit_width(0u) == 0);
static_assert(riw::bit_width(1024u) == 11);
static_assert(riw::bit_width(0) == 0);
static_assert(riw::bit_width(1) == 1);
static_assert(riw::bit_width(2) == 2);
static_assert(riw::bit_width(3) == 2);
static_assert(riw::bit_width(4) == 3);
} // namespace
