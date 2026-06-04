#include <array>
#include <vector>

#include <gtest/gtest.h>
#include <riw/dsp/window.hpp>

TEST(Dsp_WindowTest, LeavesEmptyWindowEmpty) {
  std::vector<double> window;

  riw::hann_inplace(window);

  EXPECT_TRUE(window.empty());
}

TEST(Dsp_WindowTest, HandlesSingleSampleWindow) {
  std::array<double, 1> window{};

  riw::hann_inplace(window);

  EXPECT_DOUBLE_EQ(window[0], 1.0);
}

TEST(Dsp_WindowTest, MatchesKnownFiveSampleWindow) {
  std::array<double, 5> window{};

  riw::hann_inplace(window);

  EXPECT_NEAR(window[0], 0.0, 1e-12);
  EXPECT_NEAR(window[1], 0.5, 1e-12);
  EXPECT_NEAR(window[2], 1.0, 1e-12);
  EXPECT_NEAR(window[3], 0.5, 1e-12);
  EXPECT_NEAR(window[4], 0.0, 1e-12);
}

TEST(Dsp_WindowTest, SupportsFloatWindows) {
  std::array<float, 4> window{};

  riw::hann_inplace(window);

  EXPECT_NEAR(window[0], 0.0F, 1e-6F);
  EXPECT_NEAR(window[1], 0.75F, 1e-6F);
  EXPECT_NEAR(window[2], 0.75F, 1e-6F);
  EXPECT_NEAR(window[3], 0.0F, 1e-6F);
}
