#include <gtest/gtest.h>
#include <riw/algorithm/normalize.hpp>

TEST(AlgorithmTest, ValueRange) {
  riw::value_range r{0.0, 1.0};

  ASSERT_EQ(r.min, 0.0);
  ASSERT_EQ(r.max, 1.0);
  ASSERT_EQ(riw::length(r), 1.0);
  ASSERT_EQ(riw::proportion(0.2, r), 0.2);
  ASSERT_EQ(riw::proportion(0.2, {0.0, 2.0}), 0.1);
}
