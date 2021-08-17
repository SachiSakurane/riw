#include <gtest/gtest.h>
#include <riw/algorithm/lerp.hpp>

TEST(AlgorithmTest, Lerp) {
  ASSERT_EQ(riw::lerp(0.0, 1.0, 0.5), 0.5);
  ASSERT_EQ(riw::lerp(0.0, 1.0, 0.2), 0.2);
  ASSERT_EQ(riw::lerp(-1.0, 1.0, 0.5), 0.0);
}
