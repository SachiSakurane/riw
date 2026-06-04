#include <gtest/gtest.h>
#include <riw/utility/brand.hpp>

#include <concepts>

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

TEST(UtilityTest_Bland, BrandedArithmeticIsTotallyOrdered) {
  using branded_int = riw::brand<int, "int_test">;

  static_assert(std::totally_ordered<branded_int>);

  branded_int low{1};
  branded_int high{2};

  ASSERT_LT(low, high);
  ASSERT_GT(high, low);
  ASSERT_EQ(low <=> high, 1 <=> 2);
}
