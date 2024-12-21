#include <gtest/gtest.h>
#include <riw/value_range/ranged.hpp>

static constexpr auto RANGE = riw::value_range{0, 100};

TEST(Utility_ValueRange_RangedTest, Basic) {
  {
    riw::ranged<int, RANGE> r{50};
    ASSERT_EQ(static_cast<int>(r), 50);
  }
}

TEST(Utility_ValueRange_RangedTest, DownerUnranged) {
  {
    riw::ranged<int, RANGE> r{-100};
    ASSERT_EQ(static_cast<int>(r), 0);
  }
}

TEST(Utility_ValueRange_RangedTest, UpperUnranged) {
  {
    riw::ranged<int, RANGE> r{100000};
    ASSERT_EQ(static_cast<int>(r), 100);
  }
}

TEST(Utility_ValueRange_RangedTest, EqRanged) {
  {
    riw::ranged<int, RANGE> r{42}, s{42}, t{4242};
    ASSERT_TRUE(r == s);
    ASSERT_FALSE(r == t);
  }
}
