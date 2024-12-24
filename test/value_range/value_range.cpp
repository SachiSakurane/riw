#include <gtest/gtest.h>
#include <riw/utility/brand.hpp>
#include <riw/value_range.hpp>

TEST(Utility_ValueRange_ValueRangeTest, Basic) {
  {
    riw::value_range actual{0, 100};
    ASSERT_EQ(actual.min, 0);
    ASSERT_EQ(actual.max, 100);
  }
  {
    riw::value_range actual{42, 0};
    ASSERT_EQ(actual.min, 0);
    ASSERT_EQ(actual.max, 42);
  }
}

TEST(Utility_ValueRange_ValueRangeTest, BlandedType) {
  using branded = riw::brand<int, "brand">;
  {
    riw::value_range actual{static_cast<branded>(0), static_cast<branded>(100)};
    ASSERT_EQ(actual.min, 0);
    ASSERT_EQ(actual.max, 100);
  }
  {
    riw::value_range actual{static_cast<branded>(42), static_cast<branded>(0)};
    ASSERT_EQ(actual.min, 0);
    ASSERT_EQ(actual.max, 42);
  }
}

TEST(Utility_ValueRange_ValueRangeTest, Convert) {
  using branded = riw::brand<int, "brand">;
  {
    riw::value_range b{static_cast<branded>(0), static_cast<branded>(100)};
    riw::value_range<int> actual = b;
    ASSERT_EQ(actual.min, 0);
    ASSERT_EQ(actual.max, 100);
  }
}
