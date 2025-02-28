#include <iostream>
#include <string>

#include <gtest/gtest.h>
#include <riw/notify.hpp>

TEST(Notify_ToObserverTest, AssignLeftValueObserver) {
  riw::behavior<int> b{42};
  
  auto o = b | [](auto i){ return i * 92; };
  std::unique_ptr<riw::observer<int>> oo{o | riw::to_observer_unique_ptr};
  ASSERT_EQ(oo->observe(), 3864);
}

TEST(Notify_ToObserverTest, AssignRightValueObserver) {
  riw::behavior<int> b{42};
  
  std::unique_ptr<riw::observer<int>> o{b | [](auto i){ return i * 92; } | riw::to_observer_unique_ptr};
  ASSERT_EQ(o->observe(), 3864);
}
