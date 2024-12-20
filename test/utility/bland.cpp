#include <gtest/gtest.h>
#include <riw/utility/brand.hpp>

TEST(UtilityTest_Bland, ArithmetricBland) {
  int i = 1;
  riw::brand<int, "int_test"> branded_int_copy = i;
  riw::brand<int, "int_test"> branded_int_construct{42};

  ASSERT_EQ(static_cast<int>(branded_int_copy), 1);
  ASSERT_EQ(branded_int_construct.value, 42);

  auto plus = branded_int_copy + branded_int_construct;
  ASSERT_EQ(static_cast<int>(plus), 43);

  decltype(branded_int_copy) pluseq = 42;
  pluseq += branded_int_construct;
  ASSERT_EQ(static_cast<int>(pluseq), 84);
}
