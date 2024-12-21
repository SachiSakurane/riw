#include <gtest/gtest.h>
#include <riw/utility/brand.hpp>
#include <riw/value_range.hpp>

TEST(Utility_ValueRange_ValueRangeTest, Basic) {
  {
    riw::value_range<int> actual{0, 100};
    ASSERT_EQ(actual.min, 0);
    ASSERT_EQ(actual.max, 100);
  }
    {
    riw::value_range<int> actual{42, 0};
    ASSERT_EQ(actual.min, 0);
    ASSERT_EQ(actual.max, 42);
  }
}

TEST(Utility_ValueRange_ValueRangeTest, BlandedType) {
  {
    riw::value_range<int> actual{0, 100};
    ASSERT_EQ(actual.min, 0);
    ASSERT_EQ(actual.max, 100);
  }
    {
    riw::value_range<int> actual{42, 0};
    ASSERT_EQ(actual.min, 0);
    ASSERT_EQ(actual.max, 42);
  }
}
