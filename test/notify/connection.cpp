#include <iostream>
#include <string>

#include <gtest/gtest.h>
#include <riw/notify/behavior.hpp>
#include <riw/notify/connection.hpp>

using namespace std::literals::string_literals;

TEST(Notify_ConnectionTest, Pipeline) {
  riw::behavior<int> i{10};

  ASSERT_EQ(i, 10);

  auto o = i | [](auto i) { return i * 2; };
  ASSERT_EQ(o, 20);

  auto oo = i | [](auto i) { return i * 2; } | [](auto i) { return i * 2; };
  ASSERT_EQ(oo, 40);

  auto ooo = oo | [](auto i) { return i * 2; };
  ASSERT_EQ(ooo, 80);
}

TEST(Notify_ConnectionTest, Transform) {
  riw::behavior<int> i{10};

  ASSERT_EQ(i, 10);

  auto o = i | [](auto i) { return std::to_string(i); } | [](auto s) { return s + "nyan"s; };

  ASSERT_EQ(static_cast<std::string>(o), "10nyan"s);
}

TEST(Notify_ConnectionTest, Effect) {
  riw::behavior<int> i{10};

  ASSERT_EQ(i, 10);

  auto o = i | [](auto i) {
    std::cout << "value: " << i;
    return i * 2;
  };

  testing::internal::CaptureStdout();
  i = 42;
  std::string log = testing::internal::GetCapturedStdout();

  ASSERT_EQ(i, 42);
  ASSERT_EQ(static_cast<std::string>(log), "value: 42"s);
}

TEST(Notify_ConnectionTest, Notify) {
  riw::behavior<int> i{42};

  ASSERT_EQ(i, 42);
  auto o = i | [](auto i) { return std::to_string(i); };

  testing::internal::CaptureStdout();

  auto s = o | [](auto s) { std::cout << "string: " << s << std::endl; };

  std::string log = testing::internal::GetCapturedStdout();

  ASSERT_EQ(static_cast<std::string>(log), "string: 42\n"s);
}
