#include <riw/bit/bit.hpp>

#include <cstdint>

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

// bit_reverse test
static_assert(riw::bit_reverse(static_cast<std::uint8_t>(0b0000'0101), static_cast<std::uint8_t>(4)) ==
              static_cast<std::uint8_t>(0b0000'1010));
static_assert(riw::bit_reverse(0b100u, 3u) == 0b001u);
static_assert(riw::bit_reverse(1u, 32u) == 0x8000'0000u);
static_assert(riw::bit_reverse(1ull, 64ull) == 0x8000'0000'0000'0000ull);
} // namespace
