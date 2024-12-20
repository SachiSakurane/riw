#include <gtest/gtest.h>
#include <riw/algorithm/lerp.hpp>

TEST(AlgorithmTestLeap, Basic) {
  {
    const auto actual = riw::lerp(0.0, 1.0, 0.5);
    ASSERT_EQ(actual, 0.5);
  }
  {
    const auto actual = riw::lerp(-1.0, 1.0, 0.5);
    ASSERT_EQ(actual, 0.0);
  }
}

TEST(AlgorithmTestLeap, LowerLimit) {
  {
    const auto actual = riw::lerp(0.0, 1.0, 0.0);
    ASSERT_EQ(actual, 0.0);
  }
  {
    const auto actual = riw::lerp(0.0, -1.0, -1.0);
    ASSERT_EQ(actual, 1.0);
  }
}

TEST(AlgorithmTestLeap, LowerOver) {
  {
    const auto actual = riw::lerp(0.0, 1.0, -1.0);
    ASSERT_EQ(actual, -1.0);
  }
  {
    const auto actual = riw::lerp(0.0, -1.0, -1.0);
    ASSERT_EQ(actual, 1.0);
  }
}

TEST(AlgorithmTestLeap, UpperLimit) {
  {
    const auto actual = riw::lerp(0.0, 1.0, 1.0);
    ASSERT_EQ(actual, 1.0);
  }
  {
    const auto actual = riw::lerp(0.0, -1.0, 1.0);
    ASSERT_EQ(actual, -1.0);
  }
}

TEST(AlgorithmTestLeap, UpperOver) {
  {
    const auto actual = riw::lerp(0.0, 1.0, 2.0);
    ASSERT_EQ(actual, 2.0);
  }
  {
    const auto actual = riw::lerp(0.0, -1.0, 2.0);
    ASSERT_EQ(actual, -2.0);
  }
}
