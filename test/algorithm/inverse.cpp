#include <gtest/gtest.h>
#include <riw/algorithm/inverse.hpp>

TEST(AlgorithmTest, Inverse) {
  ASSERT_EQ(riw::inverse(0.5), 0.5);
  ASSERT_EQ(riw::inverse(0.2), 0.8);
  ASSERT_EQ(riw::inverse(-0.5), 1.5);
  ASSERT_EQ(riw::inverse(1.5), -0.5);
  ASSERT_EQ(riw::inverse(2.0, {2.0, 4.0}), 4.0);
}
