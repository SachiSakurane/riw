#include <gtest/gtest.h>
#include <riw/algorithm/normalize.hpp>

TEST(AlgorithmTest, Normalize) {
  ASSERT_EQ(riw::normalize(0.2, {0.0, 1.0}, {0.0, 2.0}), 0.4);
  ASSERT_EQ(riw::normalize(0.2, {0.0, 2.0}, {0.0, 1.0}), 0.1);
}
