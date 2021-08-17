#include <gtest/gtest.h>
#include <riw/algorithm/normalize.hpp>

TEST(AlgorithmTest, ValueRange) {
  riw::range r{0.0, 1.0};

  ASSERT_EQ(r.min, 0.0);
  ASSERT_EQ(r.max, 1.0);
  ASSERT_EQ(r.length(), 1.0);
}
