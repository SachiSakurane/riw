# riw phase-vocoder pitch shifter

## Objective and scope
Implement and verify a reusable real-time C++20 streaming phase-vocoder pitch
shifter for the CuteAudioPoC Micro-Unison FX DSP foundation. Application voice
distribution, pan, time offset, mixing and CuteAudioPoC integration are excluded.
Push explicitly requested on 2026-10-07 after the implementation handoff.
PR creation explicitly requested after push on 2026-10-07.
No merge or deployment requested. No delegation authorized.

## Checkout and baseline
- Independent checkout: /Users/sachi/Develop/riw; initial branch main, clean.
- Work branch: codex/phase-vocoder.
- Baseline: 79add5e45de714fa4611cd2e8f392468a840d4e4.
- Current implementation is an uncommitted diff; final file hashes will identify
  the verified state. CuteAudioPoC's riw submodule is not touched.

## Design
- Reuse riw's symmetric Hann window and radix-2 FFT kernel. Add a unitary,
  allocation-free span in-place FFT without changing the existing FFT API.
- Planar synchronous 1–8 channels, power-of-two N=16–65536, H divides N,
  H <= N/4. Defaults N=2048, H=256, 48 kHz, 20 ms ratio smoothing.
- Streaming trailing STFT frames, phase-difference frequency estimates, shared
  per-source-bin phase rotations, nearest destination-bin mapping, real IFFT,
  normalized weighted overlap-add. Strongest channel selects each bin's
  frequency estimate; each channel retains its own complex coefficient.
- Ratio range [0.5, 2.0]. Off-band bins dropped. No formant preservation,
  transient correction or high-rate modulation features.
- Frame input start t-N+1 is scheduled at output t+1, giving exactly N samples
  of fixed algorithmic delay. Actual unity reconstruction must confirm this.
- prepare allocates; process/set_pitch_ratio/reset are noexcept and do not
  allocate/free, lock or perform I/O. Double internal state; values below 1e-30
  and nonfinite inputs are zeroed; finite output is bounded to sample type.
- prepare resets to unity and clears history; reset clears history and snaps
  to current ratio target. Calls require processing-thread ownership.

## Completion checklist
- [x] Public implementation and usage/limitations documentation.
- [x] Unity impulse/sine/noise reconstruction at declared latency.
- [x] Pitch accuracy including +/-30 cents, range endpoints, 44.1/48/96 kHz.
- [x] Block splitting, in-place processing, reset and reprepare.
- [x] Pitch-change continuity and linked stereo/channel separation.
- [x] No processing allocation/deallocation; long-run finite output; denormals.
- [x] Full Bazel suite, static guarantees and final validation identity.
- [x] Architecture, complexity and CuteAudioPoC integration handoff.

## Verification and outstanding work
Implementation, documentation and verification complete within the agreed riw scope.
- Standard Bazel command in sandbox failed to create /var/tmp cache.
- /private/tmp cache in sandbox failed to bind Bazel's localhost server.
- Escalated exact full-suite command reached compilation, but Xcode's
  SDKSettings.json was rejected as an undeclared absolute include. Zero tests ran.
- Disabling dependency_file was rejected by Bazel because the expected .d file
  was missing; no tests ran. This workaround was abandoned.
- The generated toolchain lists the CommandLineTools SDK root as a builtin
  include directory. Specifying that SDK with -isysroot resolved the environment
  failure while preserving dependency tracking. No repo build configuration
  was changed to accommodate the SDK.
- DSP test target size changed from small to medium for the roughly 53-second
  unoptimized signal/stream verification suite.


## Final verification results
- Command: `bazel --output_user_root=/private/tmp/riw-phase-vocoder-bazel test //... --test_output=all --copt=-isysroot --copt=/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk`.
- All 9 test targets passed; final DSP target contains 14 runtime tests (10 new
  phase-vocoder tests plus 4 existing window tests). Final run re-executed the
  modified DSP target and reused valid passes for unchanged targets.
- Unity double impulse/sine/noise: N=64/256/1024, H=N/4,N/8, error <=2e-11,
  initial N samples silent, exact declared N delay. Float independent stereo
  unity error <=2e-7.
- Pitch: 220.3/997.1 Hz inputs, +/-30 and +/-1200 cents, 44.1/48/96 kHz,
  acceptance <=0.5 cent; observed maximum absolute error
  0.00018015115903733463 cents (GTest XML property).
- Block splitting (1 vs whole vs in-place) identical; automation at fixed
  sample positions agrees for 63 vs 509 sample blocks. +/-30-cent transitions
  have maximum adjacent-sample change <0.1 for a 0.5-amplitude 440 Hz sine.
- Silent/scaled/inverted stereo retained; quadrature tone phase/amplitude within
  0.01 rad / 0.01 relative tolerance. Separate stereo noise unity reconstructs.
- Invalid config and ratio retain state; reset reproducible; sample rates
  repeatedly reprepare, float denormals and NaN inputs produce zero.
- Heap probe: 3,084,000 samples/channel, ordinary new/delete both zero; finite
  output throughout. Reset and ratio updates are included in the probe.
- Optional stress source work/phase-vocoder/stress.cpp compiled with clang++
  -std=c++20 -O2 -Iinclude. Stereo 48 kHz, N=2048,H=256, 28,800,191 samples/channel
  (600.003979 seconds audio), ratio sweeps over [0.5,2.0], no reset:
  all calls successful, all samples finite, allocations=deallocations=0,
  latency=2048, peak=0.678679049. Observed wall time 37.430175 seconds;
  this is not a per-callback deadline measurement.
- Direct clang++ -std=c++20 -O1 -g -fsanitize=address,undefined build of the DSP
  and FFT tests plus existing GoogleTest sources: all 16 tests pass, no sanitizer
  diagnostics. GoogleTest itself emits char8_t conversion warnings with this
  compiler; no new dependency or third-party code modifications.
- git diff --check passed. Last header edit after Bazel/sanitizer runs changed
  only the introductory allocation comment; the ten-minute stress compiled
  the final header. All executable implementation/test code is unchanged from
  the passing suite.

## Algorithm refinements and known limits
- Remap around the symmetric window center. Silent bins reset phase correction
  on reactivation, verified by identical impulse responses after different
  silence lengths (offsets are multiples of H).
- Host compensation is the fixed N-sample frame alignment. Shifted impulse
  peak/centroid positions are signal-dependent; the FFT/hop schedule does not
  change with ratio or block size. Non-unity transient shape/timing is not
  guaranteed, and returning to unity retains phase offsets until reset.
- Basic nearest-bin mapping can color audio, merge bins, ripple amplitude and
  alter general stereo geometry. No formant/transient preservation, peak phase
  locking or strong Nyquist antialias filtering. Supported range establishes
  API bounds and tone-pitch behavior, not a perceptual quality guarantee.
- CPU: O(C*N*log(N)/H) average per sample, burst O(C*N*log(N)) per hop;
  memory O(C*N), approximately 162 KiB mono / 242 KiB stereo at defaults.

## Handoff and remaining work
- Public API: include/riw/dsp/phase_vocoder.hpp. Usage, algorithm, latency,
  complexity, constraints and integration: docs/phase-vocoder.md (README link).
- CuteAudioPoC: one synchronous stereo shifter per voice; prepare outside audio
  callback, apply exp2(cents/1200), keep common N/H, compensate dry by N samples,
  report N to host, reprepare on rate/format change while processing is stopped.
- No outstanding implementation/verification items in this riw scope.
- CuteAudioPoC integration and deployment-hardware callback/aggregate CPU
  measurement remain with the original Micro-Unison FX task; not performed here.
- Initial handoff contained no commit or push. Follow-up authorization now
  covers committing the verified artifacts and pushing codex/phase-vocoder
  to origin (https://github.com/SachiSakurane/riw.git).
- PR creation subsequently authorized; merge, deployment and task archival
  remain outside the requested scope.

## Verified uncommitted artifact identity
Baseline commit remains 79add5e45de714fa4611cd2e8f392468a840d4e4 on branch
codex/phase-vocoder. SHA-256 aggregate of the sorted lines below:
`aff888915a45df3fa59850b1a4dbc398b19a0efcfe8d7b8b2ee60c3a433d1993`.
The ledger itself is excluded to avoid a recursive hash.

```text
954cbe1b722e287e12c4e86f878c709a614e7878dabb65f553a6dad17c8af0bb  README.md
357afdd0fa673adff820b09408e816f3be0cd69e465c48a83d7023f27d8417ff  docs/phase-vocoder.md
b34239b8f4316070f88003cff02fcf9ee4f5ba755624757fc7108394ea947b61  include/riw/dsp/phase_vocoder.hpp
6140e53f791d24a304eec1704eb5cbed1a3f44646cf0efdcfe2cc0d2e155213f  include/riw/math/fft.hpp
40e9f60bc1973a3cd64d9c3bd2cc7aa02758de0a9e181b7dec51715343b5179f  static_test/headers/phase_vocoder.cpp
45fe1afb293a712d1190035afbfaff8eed4ce046adead37038a0b575b062ce49  test/dsp/BUILD
24357f200f8d84e83411e1da898d939d4aa6d7d9dd0cbb021c56e3cadbb43cea  test/dsp/phase_vocoder.cpp
c501d8906225a408bea5417bf913b8ebfb2f361e16d0d9ce8c2be90e9e7f223c  test/math/fft.cpp
cb7f89e7e965b5235a9bacaabaee3837a5720e261fa1fbf0b863c34e08f925de  work/phase-vocoder/stress.cpp
```

## Push follow-up (2026-10-07)
The user requested a push of the completed implementation. All recorded artifact
hashes still match the verified files; no implementation changes or new test
runs were necessary. Delivery target is origin/codex/phase-vocoder, using a
normal push with upstream tracking. The delivery commit includes this ledger;
its identifier and remote confirmation are reported in the chat after push.

## PR handoff (2026-10-07)
- Implementation commit 944adc9abaa2131f31adf41336c99fe911ac4f61 was pushed to
  origin/codex/phase-vocoder; local and upstream commit identifiers matched.
- PR #2: https://github.com/SachiSakurane/riw/pull/2
- Title: Add streaming phase-vocoder pitch shifter.
- Base main, head codex/phase-vocoder. Remote branch inspection confirmed main
  exists and develop does not; there was no existing open PR for this head.
- PR covers all riw implementation/documentation/verification completion items.
  Validation remains the recorded source hashes and passing tests above;
  this follow-up changes only this ledger and requires no additional DSP tests.
- Review/CI on the PR can be inspected in GitHub; no merge or deployment was
  requested. CuteAudioPoC integration and deployment-hardware CPU measurement
  remain with the original Micro-Unison task.

## Windows CI correction (2026-10-07, in progress)
- User reported Windows build failure on PR #2. Run 37598594414 at head
  81383a01b5709d1a3842d019e56597cd14c6163f failed in the GoogleTest git-clone
  ExternalProject download step, before compiling riw tests. Other OS jobs
  were cancelled by fail-fast; their success is not established.
- ExternalProject defaults an omitted GIT_TAG to master. GoogleTest's remote
  advertises main and no master. CMake now pins v1.15.2, the same dependency
  version already specified by MODULE.bazel, and emits failed external-build
  log output directly into CI logs.
- Added test code contained size_t-to-double conversions and signed initializer
  lists. Explicit conversions and size_t arrays retain numeric behavior and
  remove conversion diagnostics under strict compilation.
- Existing template_string_literal construction used a signed loop index against
  unsigned Size; local CMake emitted sign-compare warnings. The loop now uses
  std::size_t, preserving the public length type and behavior while removing
  the corresponding Windows /W4 /WX warning hazard.
- Strict C++20 clang -Wall -Wextra -Wconversion -Werror syntax checks passed
  for modified DSP/FFT tests. CMake Debug dependency download/configure/build
  succeeded with the pinned tag. The full Bazel suite passes all 9 targets, and the CMake Debug runtime
  executable passes all 38 tests (84.045 seconds, including existing UBSan).
- Windows Debug/Release CI confirmation remains pending; no claim of completion
  is made for this follow-up until those jobs pass.

Windows correction validation input (SHA-256, excluding ledger):
```text
67a02e84fdc8fb8ed14af72f1d9d966209b4d05ce78f00eecb4f8042e631fc40  test/External_GTest.cmake
08938c166d268dd70ec498310d26ec854767419973f013a2f36cfc3676a04dca  test/dsp/phase_vocoder.cpp
72029bed46515a8d9046e39cd406745e0228a29f562b53a25acec23d5361832e  test/math/fft.cpp
422c17287913cb3313a2508a39ae5e212c158e9fc775257e4dd0700b6f666a77  include/riw/utility/template_string_literal.hpp
```

### Windows CI second stage
- Run 37605826732 at d9ef2f7001fc74ef56ae1c15a24415708ed96675 successfully
  downloaded/configured/built GoogleTest, then MSVC reported C2124 in
  hann_inplace<std::array<double,1>> at window.hpp:20. The constant divisor
  size-1 is zero even though that runtime path follows the size==1 early return.
- The denominator expression now uses size>1 ? size-1 : 1. Existing empty,
  single-sample and normal Hann behavior is unchanged; the expression is valid
  for all extents under MSVC's template diagnostics.
- CMake rebuilt successfully; all 4 window tests passed. Strict C++20
  -Wall -Wextra -Wconversion -Werror checks pass for DSP/window/FFT tests.
- Full Bazel suite now passes all 9 targets for this stage (DSP 50.950 seconds).
  Windows Debug/Release confirmation remains pending; the previous Windows CI
  failure is not marked as passed.
- Current window.hpp SHA-256: `b9817a11cff7210a60a161e82e687004904c9cd9a3ee6719a02b7a87e599e6db`.

### Windows CI link stage
- Run 37606363140 at 9ffd1a99217f92d388ca206a98a7c1783a9a8dac compiled all
  riw test translation units, confirming the singleton-Hann fix. Linking then
  failed with LNK1104 for lib/Debug/;libgtestd.lib.
- Existing External_GTest.cmake interpolated CMAKE_FIND_LIBRARY_PREFIXES (a
  candidate list) into every imported-library path. On Windows this produced
  a semicolon-containing filename. Use the singular CMAKE_STATIC_LIBRARY_PREFIX
  for gtest/gtest_main/gmock/gmock_main paths, matching the produced archive.
- This changes only dependency library paths, not DSP or test behavior.
  Windows Debug/Release link and runtime verification are still pending.
- CMake Debug reconfigure/build and all 4 window tests pass after this path
  correction. Executable C++ files are unchanged from the passing full suites.
- External_GTest.cmake SHA-256: `9155963aff752474f1ce015dde07fc6052a203d315b24b7bc682b1677f43fac3`.

### Windows Debug library names
- Run 37606746059 at 9d330138bd6f2ba1369cbd2b14c0abaaf500ecbe:
  Windows Release build and runtime tests succeeded. Debug compiled but failed
  to link gtestd.lib. Debug job 112743969528's Fail Dump explicitly lists
  lib/Debug/gmock.lib, gtest.lib and gtest_main.lib without a d postfix.
- The old imported-library path logic assumed a d postfix. Remove this assumed
  postfix for the pinned GoogleTest v1.15.2, retaining Debug/Release directory
  selection. This addresses the remaining Debug-specific link error.
- Pending: final Windows Debug build/runtime and Release reconfirmation.
- Local CMake Debug reconfigure/build passes. No executable-code changes since
  the passing full suites. Final External_GTest.cmake SHA-256:
  `6d37937110474dcab0136e7ec7d675de435f8e110f0b946adfedde622768c43a`.
