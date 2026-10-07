#include <riw/dsp/frequency_shifter.hpp>

#include <array>
#include <complex>
#include <limits>
#include <random>

#include <gtest/gtest.h>

namespace {
using Shifter = riw::frequency_shifter<double>;

std::vector<double> run(Shifter &shifter, const std::vector<double> &input, size_t block = 137) {
  std::vector<double> output(input.size());
  for (size_t i = 0; i < input.size(); i += block) {
    std::array<const double *, 1> in{input.data() + i};
    std::array<double *, 1> out{output.data() + i};
    EXPECT_TRUE(shifter.process(in, out, std::min(block, input.size() - i)));
  }
  return output;
}

std::vector<double> tones(size_t count, double rate) {
  std::vector<double> result(count);
  for (size_t i = 0; i < count; ++i)
    result[i] = 0.3 * std::cos(riw::two_pi<double> * 2000 * static_cast<double>(i) / rate) +
                0.2 * std::sin(riw::two_pi<double> * 5000 * static_cast<double>(i) / rate);
  return result;
}

// One second of coherent integer-Hz projection after the FIR startup.
double amplitude(const std::vector<double> &signal, double rate, double frequency) {
  std::complex<double> sum{};
  const size_t count = static_cast<size_t>(rate);
  const size_t begin = signal.size() - count;
  for (size_t i = 0; i < count; ++i)
    sum += signal[begin + i] * std::polar(1., riw::two_pi<double> * frequency *
                                           static_cast<double>(i) / rate);
  return 2 * std::abs(sum) / static_cast<double>(count);
}
} // namespace

TEST(Dsp_FrequencyShifter, ZeroShiftIsExactDelayForImpulseNoiseAndDc) {
  std::mt19937 random(42);
  std::uniform_real_distribution<double> distribution(-0.5, 0.5);
  for (size_t n : {7, 31, 257, 511}) {
    for (int kind = 0; kind < 3; ++kind) {
      Shifter shifter;
      ASSERT_TRUE(shifter.prepare({48000, 1, n}));
      const auto delay = shifter.latency_samples();
      EXPECT_EQ(delay, (n - 1) / 2);
      std::vector<double> input(2000 + delay);
      for (size_t i = 0; i < 2000; ++i)
        input[i] = kind == 0 ? (i == 0 ? 1. : 0.) : kind == 1 ? distribution(random) : 0.25;
      const auto output = run(shifter, input);
      for (size_t i = 0; i < delay; ++i) EXPECT_EQ(output[i], 0);
      for (size_t i = delay; i < output.size(); ++i)
        EXPECT_EQ(output[i], input[i - delay]);
    }
  }
}

TEST(Dsp_FrequencyShifter, MovesEveryToneByHzAndRejectsOppositeSideband) {
  for (double rate : {44100., 48000., 96000.}) {
    for (double shift : {-300., 300.}) {
      Shifter shifter;
      ASSERT_TRUE(shifter.prepare({rate, 1, 257, 0}));
      ASSERT_TRUE(shifter.set_shift_hz(shift));
      shifter.reset();
      const auto output = run(shifter, tones(static_cast<size_t>(rate) + 1024, rate));
      EXPECT_NEAR(amplitude(output, rate, 2000 + shift), 0.3, 0.0003);
      EXPECT_NEAR(amplitude(output, rate, 5000 + shift), 0.2, 0.0003);
      EXPECT_LT(amplitude(output, rate, 2000 - shift), 0.0003);
      EXPECT_LT(amplitude(output, rate, 5000 - shift), 0.0003);
      EXPECT_LT(amplitude(output, rate, 2000), 0.0003);
      EXPECT_LT(amplitude(output, rate, 5000), 0.0003);
    }
  }
}

TEST(Dsp_FrequencyShifter, BlocksInPlaceAndSmoothedAutomationAgree) {
  Shifter a, b, c;
  ASSERT_TRUE(a.prepare({48000, 1, 257, 0.01}));
  ASSERT_TRUE(b.prepare(a.config()));
  ASSERT_TRUE(c.prepare(a.config()));
  auto input = tones(6000, 48000);
  for (double shift : {350., -550., 0.}) {
    ASSERT_TRUE(a.set_shift_hz(shift));
    ASSERT_TRUE(b.set_shift_hz(shift));
    ASSERT_TRUE(c.set_shift_hz(shift));
    const auto whole = run(a, input, input.size());
    EXPECT_EQ(whole, run(b, input, 1));
    auto inplace = input;
    std::array<const double *, 1> in{inplace.data()};
    std::array<double *, 1> out{inplace.data()};
    ASSERT_TRUE(c.process(in, out, inplace.size()));
    EXPECT_EQ(whole, inplace);
  }
}

TEST(Dsp_FrequencyShifter, FloatStereoSharesPhaseAndKeepsSilentChannelSilent) {
  riw::frequency_shifter<float> shifter;
  ASSERT_TRUE(shifter.prepare({48000, 3, 257, 0}));
  ASSERT_TRUE(shifter.set_shift_hz(-200));
  auto source = tones(10000, 48000);
  std::vector<float> left(source.begin(), source.end()), right(left.size()), silent(left.size());
  for (size_t i = 0; i < left.size(); ++i) right[i] = -0.5f * left[i];
  std::array<const float *, 3> in{left.data(), right.data(), silent.data()};
  std::array<float *, 3> out{left.data(), right.data(), silent.data()};
  ASSERT_TRUE(shifter.process(in, out, left.size()));
  for (size_t i = 0; i < left.size(); ++i) {
    EXPECT_EQ(right[i], -0.5f * left[i]);
    EXPECT_EQ(silent[i], 0);
    EXPECT_TRUE(std::isfinite(left[i]));
  }
}

TEST(Dsp_FrequencyShifter, SmoothingMatchesClosedFormAndControlsRetainPhase) {
  for (double seconds : {0., 0.01}) {
    Shifter shifter;
    ASSERT_TRUE(shifter.prepare({48000, 1, 31, seconds}));
    const auto a = seconds == 0 ? 0 : std::exp(-1 / (48000 * seconds));
    double initial_shift = 0;
    double initial_phase = 0;
    size_t elapsed = 0;
    for (double target : {300., -500., 0.}) {
      ASSERT_TRUE(shifter.set_shift_hz(target));
      std::vector<double> input(1000, 0.25);
      const auto output = run(shifter, input);
      // Sum the geometric progression of smoothed frequencies independently
      // of the implementation's sample-by-sample recursion.
      const auto phase_at = [&](size_t n) {
        const auto integrated = target * static_cast<double>(n) +
            (initial_shift - target) * a * (1 - std::pow(a, static_cast<double>(n))) / (1 - a);
        return initial_phase + riw::two_pi<double> * integrated / 48000;
      };
      // A DC step has a Hilbert startup transient. Compare the oscillator once
      // the entire FIR history contains DC and its quadrature component is zero.
      for (size_t i = 0; i < output.size(); ++i)
        if (elapsed + i >= shifter.config().filter_size - 1)
          EXPECT_NEAR(output[i], 0.25 * std::cos(phase_at(i)), 2e-13);
      initial_phase = phase_at(input.size());
      initial_shift = target + (initial_shift - target) * std::pow(a, static_cast<double>(input.size()));
      elapsed += input.size();
    }
  }
}

TEST(Dsp_FrequencyShifter, ResetClearsHistoryAndSnapsTargetAndReprepareResetsShift) {
  Shifter a, fresh;
  ASSERT_TRUE(a.prepare({48000, 1, 257, 0.02}));
  ASSERT_TRUE(fresh.prepare(a.config()));
  ASSERT_TRUE(a.set_shift_hz(350));
  ASSERT_TRUE(fresh.set_shift_hz(-170));
  auto input = tones(5000, 48000);
  run(a, input);
  ASSERT_TRUE(a.set_shift_hz(-170));
  a.reset(); fresh.reset();
  EXPECT_EQ(run(a, input), run(fresh, input));
  ASSERT_TRUE(a.prepare({96000, 1, 511, 0}));
  ASSERT_TRUE(fresh.prepare(a.config()));
  EXPECT_EQ(run(a, input), run(fresh, input));
}

TEST(Dsp_FrequencyShifter, InvalidCallsAreTransactionalAndEmptyBlocksDoNotAdvance) {
  Shifter a, b;
  EXPECT_FALSE(a.set_shift_hz(10));
  EXPECT_FALSE(a.process({}, {}, 0));
  EXPECT_EQ(a.latency_samples(), 0);
  ASSERT_TRUE(a.prepare({48000, 1, 31, 0}));
  ASSERT_TRUE(b.prepare(a.config()));
  ASSERT_TRUE(a.set_shift_hz(150)); b.set_shift_hz(150);
  const auto input = tones(1000, 48000);
  EXPECT_EQ(run(a, input), run(b, input));
  for (int field = 0; field < 4; ++field) {
    for (double value : {0., -1., std::numeric_limits<double>::infinity(),
                         std::numeric_limits<double>::quiet_NaN()}) {
      auto config = a.config();
      if (field == 0) config.sample_rate = value;
      if (field == 1) config.channels = 0;
      if (field == 2) config.filter_size = 32;
      if (field == 3) config.smoothing_seconds = value == 0 ? -1 : value;
      EXPECT_FALSE(a.prepare(config));
    }
  }
  for (size_t channels : {size_t{9}, std::numeric_limits<size_t>::max()}) {
    auto config = a.config(); config.channels = channels;
    EXPECT_FALSE(a.prepare(config));
  }
  for (size_t n : {0, 5, 4097}) {
    auto config = a.config(); config.filter_size = n;
    EXPECT_FALSE(a.prepare(config));
  }
  for (double hz : {24001., -24001., std::numeric_limits<double>::infinity(),
                    std::numeric_limits<double>::quiet_NaN()})
    EXPECT_FALSE(a.set_shift_hz(hz));
  double sentinel = 42;
  std::array<double *, 1> out{&sentinel};
  std::array<const double *, 1> null{nullptr};
  EXPECT_FALSE(a.process({}, out, 1));
  EXPECT_FALSE(a.process(null, out, 1));
  EXPECT_EQ(sentinel, 42);
  std::array<double *, 1> null_out{nullptr};
  EXPECT_TRUE(a.process(null, null_out, 0));
  EXPECT_EQ(run(a, input), run(b, input));
  EXPECT_TRUE(a.set_shift_hz(-24000));
  EXPECT_TRUE(a.set_shift_hz(24000));
}

TEST(Dsp_FrequencyShifter, NonfiniteAndTinyInputAreSilence) {
  Shifter shifter;
  ASSERT_TRUE(shifter.prepare({48000, 1, 257, 0}));
  ASSERT_TRUE(shifter.set_shift_hz(130));
  std::vector<double> input(2000);
  input[0] = std::numeric_limits<double>::quiet_NaN();
  input[1] = std::numeric_limits<double>::infinity();
  input[2] = -std::numeric_limits<double>::infinity();
  input[3] = std::numeric_limits<double>::denorm_min();
  for (auto value : run(shifter, input)) EXPECT_EQ(value, 0);
}
