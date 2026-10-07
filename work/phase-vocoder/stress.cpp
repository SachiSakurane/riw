// Optional ten-minute default-configuration streaming verification.
// clang++ -std=c++20 -O2 -Iinclude work/phase-vocoder/stress.cpp -o /tmp/riw-stress
#include <riw/dsp/phase_vocoder.hpp>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <new>

namespace {
bool observing = false;
size_t allocations = 0, deallocations = 0;
}
void *operator new(std::size_t size) {
  if (observing) ++allocations;
  if (auto p = std::malloc(size ? size : 1)) return p;
  throw std::bad_alloc();
}
void *operator new[](std::size_t size) { return ::operator new(size); }
void operator delete(void *p) noexcept {
  if (observing && p) ++deallocations;
  std::free(p);
}
void operator delete[](void *p) noexcept { ::operator delete(p); }

int main() {
  riw::phase_vocoder_pitch_shifter<float> shifter;
  if (!shifter.prepare({48000, 2, 2048, 256})) return 1;
  std::array<float, 257> left{}, right{}, out_left{}, out_right{};
  for (size_t i = 0; i < left.size(); ++i) {
    left[i] = static_cast<float>(0.5 * std::sin(i * 0.09));
    right[i] = static_cast<float>(0.3 * std::cos(i * 0.17));
  }
  const std::array<double, 7> ratios{0.5, 0.7, std::exp2(-30. / 1200), 1,
                                    std::exp2(30. / 1200), 1.4, 2};
  std::array<const float *, 2> input{left.data(), right.data()};
  std::array<float *, 2> output{out_left.data(), out_right.data()};
  constexpr size_t samples = 48000 * 600;
  constexpr size_t blocks = (samples + left.size() - 1) / left.size();
  bool success = true, finite = true;
  double peak = 0;
  const auto start = std::chrono::steady_clock::now();
  observing = true;
  for (size_t block = 0; block < blocks; ++block) {
    if (block % 1000 == 0)
      success &= shifter.set_pitch_ratio(ratios[(block / 1000) % ratios.size()]);
    success &= shifter.process(input, output, left.size());
    for (size_t i = 0; i < left.size(); ++i) {
      finite &= std::isfinite(out_left[i]) && std::isfinite(out_right[i]);
      peak = std::max(peak, static_cast<double>(std::max(std::abs(out_left[i]),
                                                       std::abs(out_right[i]))));
    }
  }
  observing = false;
  const auto seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
  std::printf("samples_per_channel=%zu audio_seconds=%.6f wall_seconds=%.6f peak=%.9f "
              "finite=%d success=%d allocations=%zu deallocations=%zu latency=%zu\n",
              blocks * left.size(), blocks * left.size() / 48000., seconds, peak,
              finite, success, allocations, deallocations, shifter.latency_samples());
  return !(success && finite && allocations == 0 && deallocations == 0);
}
