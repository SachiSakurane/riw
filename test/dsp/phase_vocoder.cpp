#include <riw/dsp/phase_vocoder.hpp>

#include <array>
#include <atomic>
#include <cstdlib>
#include <limits>
#include <new>
#include <random>
#include <type_traits>

#include <gtest/gtest.h>

namespace {
std::atomic<bool> watch_allocations{false};
std::atomic<size_t> allocations{0}, deallocations{0};
}
void *operator new(std::size_t size) {
  if (watch_allocations.load()) ++allocations;
  if (auto p = std::malloc(size ? size : 1)) return p;
  throw std::bad_alloc();
}
void *operator new[](std::size_t size) { return ::operator new(size); }
void operator delete(void *p) noexcept {
  if (p && watch_allocations.load()) ++deallocations;
  std::free(p);
}
void operator delete[](void *p) noexcept { ::operator delete(p); }

namespace {
using Shifter = riw::phase_vocoder_pitch_shifter<double>;
static_assert(noexcept(std::declval<Shifter &>().reset()));
static_assert(noexcept(std::declval<Shifter &>().set_pitch_ratio(1)));
static_assert(noexcept(std::declval<Shifter &>().process({}, {}, 0)));

std::vector<double> run(Shifter &shifter, const std::vector<double> &input, size_t block = 137) {
  std::vector<double> output(input.size());
  for (size_t i = 0; i < input.size(); i += block) {
    std::array<const double *, 1> in{input.data() + i};
    std::array<double *, 1> out{output.data() + i};
    EXPECT_TRUE(shifter.process(in, out, std::min(block, input.size() - i)));
  }
  return output;
}

std::vector<double> sine(size_t count, double rate, double frequency) {
  std::vector<double> result(count);
  for (size_t i = 0; i < count; ++i)
    result[i] = 0.5 * std::sin(riw::two_pi<double> * frequency * i / rate);
  return result;
}

// Independent time-domain frequency measurement: maximize a windowed sinusoidal
// projection, searching +/- 8 Hz around the requested frequency, then refine.
double measured_frequency(const std::vector<double> &signal, double rate, double expected) {
  const auto begin = signal.size() / 2;
  const auto count = signal.size() - begin;
  const auto power = [&](double frequency) {
    const auto step = std::polar(1.0, riw::two_pi<double> * frequency / rate);
    std::complex<double> oscillator{1}, sum{};
    for (size_t i = 0; i < count; ++i) {
      const auto window = 0.5 - 0.5 * std::cos(riw::two_pi<double> * i / (count - 1));
      sum += signal[begin + i] * window * oscillator;
      oscillator *= step;
    }
    return std::norm(sum);
  };
  double best = expected, best_power = -1;
  for (double f = expected - 8; f <= expected + 8; f += 0.5) {
    const auto p = power(f);
    if (p > best_power) { best = f; best_power = p; }
  }
  double lo = best - 0.5, hi = best + 0.5;
  for (int i = 0; i < 24; ++i) {
    const auto a = (2 * lo + hi) / 3, b = (lo + 2 * hi) / 3;
    if (power(a) < power(b)) lo = a; else hi = b;
  }
  return (lo + hi) / 2;
}
}

TEST(Dsp_PhaseVocoder, UnityReconstructsImpulseSineAndNoiseAtDeclaredLatency) {
  std::mt19937 random(42);
  std::uniform_real_distribution<double> distribution(-0.5, 0.5);
  for (size_t n : {64, 256, 1024}) {
    for (size_t h : {n / 4, n / 8}) {
      for (int signal = 0; signal < 3; ++signal) {
        Shifter shifter;
        ASSERT_TRUE(shifter.prepare({48000, 1, n, h}));
        ASSERT_EQ(shifter.latency_samples(), n);
        std::vector<double> input(n * 5);
        if (signal == 0) {
          input[0] = 1; input[h - 1] = -0.75; input[n + 3] = 0.5;
        } else if (signal == 1) {
          input = sine(input.size(), 48000, 713.7);
        } else {
          for (auto &v : input) v = distribution(random);
        }
        const auto original = input;
        input.resize(input.size() + n, 0);
        const auto output = run(shifter, input);
        for (size_t i = 0; i < n; ++i) EXPECT_NEAR(output[i], 0, 2e-12);
        for (size_t i = 0; i < original.size(); ++i)
          ASSERT_NEAR(output[i + n], original[i], 2e-11) << n << '/' << h << " at " << i;
      }
    }
  }
}

TEST(Dsp_PhaseVocoder, PitchAccuracyAndSampleRateReprepare) {
  Shifter shifter;
  double worst_error = 0;
  for (double rate : {44100., 48000., 96000.}) {
    for (double cents : {-1200., -30., 30., 1200.}) {
      ASSERT_TRUE(shifter.prepare({rate, 1, 2048, 256}));
      const auto ratio = std::exp2(cents / 1200);
      ASSERT_TRUE(shifter.set_pitch_ratio(ratio));
      shifter.reset();
      for (double frequency : {220.3, 997.1}) {
        shifter.reset();
        const auto output = run(shifter, sine(static_cast<size_t>(rate * 1.5), rate, frequency));
        const auto measured = measured_frequency(output, rate, frequency * ratio);
        const auto error = 1200 * std::log2(measured / (frequency * ratio));
        worst_error = std::max(worst_error, std::abs(error));
        EXPECT_NEAR(error, 0, 0.5) << rate << " Hz, " << cents << " cents, " << frequency;
        EXPECT_EQ(shifter.latency_samples(), 2048);
      }
    }
  }
  RecordProperty("maximum_pitch_error_cents", worst_error);
}

TEST(Dsp_PhaseVocoder, BlockSplittingAndInPlaceAgree) {
  Shifter a, b, c;
  ASSERT_TRUE(a.prepare({48000, 1, 256, 32}));
  ASSERT_TRUE(b.prepare(a.config()));
  ASSERT_TRUE(c.prepare(a.config()));
  a.set_pitch_ratio(1.017); b.set_pitch_ratio(1.017); c.set_pitch_ratio(1.017);
  auto input = sine(10000, 48000, 723.4);
  const auto one = run(a, input, input.size());
  const auto split = run(b, input, 1);
  std::array<const double *, 1> in{input.data()};
  std::array<double *, 1> out{input.data()};
  ASSERT_TRUE(c.process(in, out, input.size()));
  EXPECT_EQ(one, split);
  EXPECT_EQ(one, input);
}

TEST(Dsp_PhaseVocoder, LinkedStereoPreservesProportionalAndSilentChannels) {
  for (double gain : {0., 1., -0.5}) {
    Shifter shifter;
    ASSERT_TRUE(shifter.prepare({48000, 2, 1024, 128}));
    shifter.set_pitch_ratio(std::exp2(30. / 1200));
    shifter.reset();
    auto left = sine(20000, 48000, 453.2), right = left;
    for (auto &v : right) v *= gain;
    std::vector<double> out_left(left.size()), out_right(left.size());
    std::array<const double *, 2> in{left.data(), right.data()};
    std::array<double *, 2> out{out_left.data(), out_right.data()};
    ASSERT_TRUE(shifter.process(in, out, left.size()));
    for (size_t i = 0; i < left.size(); ++i)
      ASSERT_NEAR(out_right[i], gain * out_left[i], 1e-12);
  }
}

TEST(Dsp_PhaseVocoder, PitchChangesAreSmoothedWithoutResetOrBlockDependence) {
  Shifter a, b;
  ASSERT_TRUE(a.prepare({48000, 1, 2048, 256}));
  ASSERT_TRUE(b.prepare(a.config()));
  const auto input = sine(48000, 48000, 440);
  std::vector<double> combined;
  for (double ratio : {1., std::exp2(30. / 1200), std::exp2(-30. / 1200), 1.}) {
    a.set_pitch_ratio(ratio); b.set_pitch_ratio(ratio);
    const auto out = run(a, input, 63);
    EXPECT_EQ(out, run(b, input, 509));
    combined.insert(combined.end(), out.begin(), out.end());
    EXPECT_EQ(a.latency_samples(), 2048);
  }
  double largest_step = 0;
  for (size_t i = 1; i < combined.size(); ++i)
    largest_step = std::max(largest_step, std::abs(combined[i] - combined[i - 1]));
  EXPECT_LT(largest_step, 0.1); // A 0.5 amplitude 440 Hz sine has a ~0.029 step.
}

TEST(Dsp_PhaseVocoder, ValidationAndReset) {
  Shifter shifter;
  EXPECT_FALSE(shifter.process({}, {}, 0));
  EXPECT_EQ(shifter.latency_samples(), 0);
  ASSERT_TRUE(shifter.prepare({48000, 1, 128, 16}));
  for (auto config : {Shifter::configuration{0}, {48000, 0}, {48000, 9},
                     {48000, 1, 100}, {48000, 1, 128, 0}, {48000, 1, 128, 17},
                     {48000, 1, 128, 64}, {48000, 1, 128, 16, -1}})
    EXPECT_FALSE(shifter.prepare(config));
  EXPECT_EQ(shifter.latency_samples(), 128);
  for (double ratio : {0., 0.499, 2.001, std::numeric_limits<double>::infinity(),
                       std::numeric_limits<double>::quiet_NaN()})
    EXPECT_FALSE(shifter.set_pitch_ratio(ratio));
  EXPECT_FALSE(shifter.process({}, {}, 1));
  auto input = sine(1024, 48000, 440);
  const auto first = run(shifter, input);
  shifter.reset();
  EXPECT_EQ(first, run(shifter, input));
}

TEST(Dsp_PhaseVocoder, ProcessingHasNoHeapTrafficAndRemainsFiniteForLongStreams) {
  riw::phase_vocoder_pitch_shifter<float> shifter;
  ASSERT_TRUE(shifter.prepare({96000, 2, 256, 32}));
  std::array<float, 257> left{}, right{}, out_left{}, out_right{};
  for (size_t i = 0; i < left.size(); ++i) {
    left[i] = static_cast<float>(0.5 * std::sin(i * 0.09));
    right[i] = static_cast<float>(0.3 * std::cos(i * 0.17));
  }
  std::array<const float *, 2> in{left.data(), right.data()};
  std::array<float *, 2> out{out_left.data(), out_right.data()};
  allocations = deallocations = 0;
  bool success = true, finite = true;
  watch_allocations = true;
  for (int block = 0; block < 12000; ++block) {
    if (block % 100 == 0) success &= shifter.set_pitch_ratio(0.5 + (block % 300) / 200.);
    success &= shifter.process(in, out, left.size());
    for (size_t i = 0; i < left.size(); ++i)
      finite &= std::isfinite(out_left[i]) && std::isfinite(out_right[i]);
  }
  shifter.reset();
  left.fill(std::numeric_limits<float>::denorm_min());
  right.fill(std::numeric_limits<float>::quiet_NaN());
  success &= shifter.process(in, out, left.size());
  watch_allocations = false;
  EXPECT_TRUE(success); EXPECT_TRUE(finite);
  EXPECT_EQ(allocations.load(), 0); EXPECT_EQ(deallocations.load(), 0);
  for (auto v : out_left) EXPECT_EQ(v, 0);
  for (auto v : out_right) EXPECT_EQ(v, 0);
}

TEST(Dsp_PhaseVocoder, FloatUnityAndIndependentStereoReconstruct) {
  riw::phase_vocoder_pitch_shifter<float> shifter;
  ASSERT_TRUE(shifter.prepare({44100, 2, 512, 64}));
  std::vector<float> left(4096), right(left.size());
  std::mt19937 random(9);
  for (size_t i = 0; i < left.size() - 512; ++i) {
    left[i] = static_cast<float>(std::sin(i * 0.317));
    right[i] = static_cast<float>(static_cast<int>(random() % 2001) - 1000) / 1000;
  }
  const auto original_left = left, original_right = right;
  std::array<const float *, 2> in{left.data(), right.data()};
  std::array<float *, 2> out{left.data(), right.data()};
  ASSERT_TRUE(shifter.process(in, out, left.size()));
  for (size_t i = 512; i < left.size(); ++i) {
    ASSERT_NEAR(left[i], original_left[i - 512], 2e-7);
    ASSERT_NEAR(right[i], original_right[i - 512], 2e-7);
  }
}

TEST(Dsp_PhaseVocoder, StereoToneKeepsQuadraturePhase) {
  Shifter shifter;
  ASSERT_TRUE(shifter.prepare({48000, 2, 2048, 256}));
  const auto ratio = std::exp2(-30. / 1200);
  shifter.set_pitch_ratio(ratio); shifter.reset();
  auto left = sine(48000, 48000, 997.1), right = left;
  for (size_t i = 0; i < right.size(); ++i)
    right[i] = 0.5 * std::cos(riw::two_pi<double> * 997.1 * i / 48000);
  std::array<const double *, 2> in{left.data(), right.data()};
  std::array<double *, 2> out{left.data(), right.data()};
  ASSERT_TRUE(shifter.process(in, out, left.size()));
  std::complex<double> l{}, r{};
  for (size_t i = left.size() / 2; i < left.size(); ++i) {
    const auto oscillator = std::polar(1., riw::two_pi<double> * 997.1 * ratio * i / 48000);
    l += left[i] * oscillator; r += right[i] * oscillator;
  }
  EXPECT_NEAR(std::arg(r * std::conj(l)), -riw::two_pi<double> / 4, 0.01);
  EXPECT_NEAR(std::abs(r) / std::abs(l), 1, 0.01);
}

TEST(Dsp_PhaseVocoder, SilenceDurationDoesNotShiftAttackTiming) {
  for (double ratio : {0.5, std::exp2(-30. / 1200), std::exp2(30. / 1200), 2.}) {
    Shifter a, b;
    ASSERT_TRUE(a.prepare({48000, 1, 512, 64}));
    ASSERT_TRUE(b.prepare(a.config()));
    a.set_pitch_ratio(ratio); b.set_pitch_ratio(ratio); a.reset(); b.reset();
    std::vector<double> early(2048), late(2048 + 4096);
    early[512] = late[512 + 4096] = 1;
    const auto first = run(a, early), second = run(b, late);
    for (size_t i = 0; i < first.size(); ++i)
      ASSERT_NEAR(first[i], second[i + 4096], 1e-12);
  }
}
