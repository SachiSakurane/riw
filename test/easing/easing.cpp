#include <gtest/gtest.h>
#include <riw/easing/easing.hpp>
#include <riw/easing/linear.hpp>

TEST(EasingTest, Easing) {
  ASSERT_EQ(riw::ease_in<riw::linear>(0.5), 0.5);
  ASSERT_EQ(riw::ease_in_out<riw::linear>(0.5), 0.5);
  ASSERT_EQ(riw::ease_out<riw::linear>(0.5), 0.5);
  ASSERT_EQ(riw::easing<riw::linear>(0.5, 1.0, 2.0, 1.0), 2.0);
}
