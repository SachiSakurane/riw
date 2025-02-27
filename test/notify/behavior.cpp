#include <iostream>
#include <string>

#include <gtest/gtest.h>
#include <riw/notify/adapter.hpp>
#include <riw/notify/behavior.hpp>

TEST(Notify_BehaviorTest, Construction) {
  class ClassA {};

  riw::behavior<int> i{10};
  riw::behavior<std::string> s{"string"};
  riw::behavior<ClassA> c{};
}

TEST(Notify_BehaviorTest, Cast) {
  riw::behavior<int> i{10};
  ASSERT_EQ(i, 10);

  riw::behavior<std::string> s{"string"};
  ASSERT_STREQ(static_cast<std::string>(s).c_str(), "string");
}

TEST(Notify_BehaviorTest, SubstitutionOperator) {
  {
    riw::behavior<int> i{10};
    i = 42;
    ASSERT_EQ(i, 42);
  }
  {
    riw::behavior<std::string> s{"string"};
    s = std::string{"namasute"};

    ASSERT_STREQ(static_cast<std::string>(s).c_str(), "namasute");
  }
  {
    struct ClassA {
      int x;
    };
    riw::behavior<ClassA> c{ClassA{.x = 42}};
    c = ClassA{.x = 4242};
    ASSERT_EQ(static_cast<ClassA>(c).x, 4242);
  }
}
