#include <gtest/gtest.h>
#include <riw/algorithm/lerp.hpp>

#include <array>

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

TEST(AlgorithmTestLeap, Container) {
  constexpr std::array values{0.0, 10.0, 20.0};

  ASSERT_EQ(riw::lerp(values, 0.0), 0.0);
  ASSERT_EQ(riw::lerp(values, 0.25), 5.0);
  ASSERT_EQ(riw::lerp(values, 0.5), 10.0);
  ASSERT_EQ(riw::lerp(values, 1.0), 20.0);
}

TEST(AlgorithmTestLeap, SingleElementContainer) {
  constexpr std::array values{42.0};

  ASSERT_EQ(riw::lerp(values, 0.0), 42.0);
  ASSERT_EQ(riw::lerp(values, 0.5), 42.0);
  ASSERT_EQ(riw::lerp(values, 1.0), 42.0);
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
