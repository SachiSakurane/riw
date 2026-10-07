# Streaming phase-vocoder pitch shifting

`riw::phase_vocoder_pitch_shifter<Sample>` in `<riw/dsp/phase_vocoder.hpp>`
accepts continuous planar audio and emits exactly one sample per input sample.
`Sample` is a floating-point type; working buffers and phase arithmetic use
`double`. There are no added dependencies. This is a spectral effect suitable
for modest detuning; output quality is not guaranteed to match a studio pitch
shifter on all material.

```cpp
#include <array>
#include <riw/dsp/phase_vocoder.hpp>

riw::phase_vocoder_pitch_shifter<float> shifter;
// On a non-audio thread, while processing is stopped:
bool ok = shifter.prepare({48000.0, 2, 2048, 256, 0.02});
// Check ok before enabling the effect. Allocation failure can throw in prepare.
shifter.set_pitch_ratio(std::exp2(30.0 / 1200.0));
shifter.reset(); // Snap to target before starting, with cleared stream history.

// On the audio thread; pointers each address `frames` samples:
std::array<const float *, 2> input{left_input, right_input};
std::array<float *, 2> output{left_output, right_output};
ok = shifter.process(input, output, frames);
// Report shifter.latency_samples() to the host (2048 for this configuration).
```

## Configuration and control

- Pitch ratio is frequency_out / frequency_in, inclusive range **0.5–2.0**.
  For cents use `exp2(cents / 1200.0)`; +/-30 cents is approximately
  0.982821–1.017480. Invalid/nonfinite ratios return false and keep the old target.
- FFT size N must be a power of two in [16, 65536]. Hop H must divide N and
  satisfy 1 <= H <= N/4. Larger windows improve frequency resolution and
  increase latency; smaller hops increase overlap and processing cost.
- Channels: 1–8, processed synchronously. Sample rate must be finite and >0;
  44.1, 48 and 96 kHz are covered by tests. Sample rate controls smoothing time;
  phase-frequency calculations otherwise use radians/sample.
- Smoothing time must be finite and >=0. Each hop applies an exponential
  coefficient exp(-H / (sample_rate * smoothing_seconds)), independent of
  process block sizes. Zero means immediate hop-boundary target changes.
- `prepare(config)` validates before replacement, allocates all storage,
  clears history and resets ratio to unity. A false result preserves the old
  prepared object. Allocation failure may throw while preparing, with the old
  state intact. Reprepare on sample-rate or FFT/hop/channel changes while the
  callback is stopped, then reapply the desired pitch ratio.
- `reset()` keeps configuration and target, clears the stream and phase history,
  and snaps the smoothed ratio to the target. It does not allocate.
- `process` supports arbitrary frame counts, including zero. Pointer span sizes
  must match channels; nonempty calls need nonnull channel pointers. Bad shape
  or an unprepared instance returns false without changing state or output.
  Callers are responsible for backing storage sizes. Same-channel in-place
  processing is supported; other input/output overlap is prohibited.
- Copying, ownership transfer, destruction and preparation belong outside the
  active callback. Reprepare a moved-from object before using it again.
- Calls are single-thread-owned, with no mutex. Schedule automation by splitting
  the callback at the desired sample position and calling `set_pitch_ratio`
  there. Do not mutate the object concurrently from the UI/control thread.

## Algorithm and fixed latency

Analysis frames use the existing symmetric Hann window, a positive-sign,
unitary radix-2 FFT and overlap H. Each positive-frequency source bin estimates
instantaneous frequency from the wrapped phase difference after subtracting
expected bin phase advance. The strongest channel supplies the frequency
estimate, using that channel's own previous phase. A bounded shared phase
rotation integrates the frequency change `(ratio - 1) * frequency * H`.
Inactive bins restart their phase rotation when audio returns, preventing silent
history from shifting later attacks. The remapping uses the window center as
its time origin. The rotated complex coefficients are summed into nearest destination bins
`round(k * ratio)`. DC is retained; destination bins above Nyquist are discarded.
Hermitian symmetry produces real inverse FFT frames. Windowed synthesis uses
precomputed sums of squared windows to normalize weighted overlap-add.

For a frame finishing at sample t, its analysis starts at t-N+1 and its
synthesis starts at t+1. The frame is therefore translated by **N samples**.
Every frame needed to reconstruct source sample s completes before output
sample s+N is emitted. The first N output samples are zero at unity. The ring
schedule never depends on ratio or callback size: `latency_samples()` is N
when prepared and 0 before preparation. This is a consequence of this causal
buffer schedule, rather than a general claim that phase vocoders have latency N.
At N=2048, latency is 46.44 ms at 44.1 kHz, 42.67 ms at 48 kHz and 21.33 ms at
96 kHz. Other ratios retain this frame-alignment delay. A shifted impulse is spread
and its peak/energy centroid need not occur exactly at N; these signal-dependent
changes are a phase-vocoder artifact, not a ratio-dependent buffering delay.
Host compensation uses the fixed frame alignment; it cannot align every
transformed transient perfectly.

After prepare/reset at unity, the effect reconstructs impulse, sine and noise
within floating-point error when delayed by N. Returning to unity after a pitch
excursion retains accumulated phase offsets to keep the stream continuous;
it is not a transparent bypass. Use a separately delayed dry path for bypass,
and crossfade outside this class. Resetting during playback clears history and
can produce a discontinuity.

The FFT addition `fft_inplace(std::span<std::complex<T>>, bool forward=true)`
uses the same unitary normalization/sign as `riw::fft`. It requires a nonempty
power-of-two span and returns false without mutation otherwise. It performs
no allocation; the existing arbitrary-size FFT API remains available unchanged.

## Real-time cost and numerical behavior

For C channels, every H samples requires C forward and C inverse FFTs plus
O(CN) spectral/window work. Average time is O(C N log N / H) per sample, with
O(C N log N) bursts at hop boundaries. No CPU deadline guarantee is implied;
measure callback timing on the deployment hardware, especially for many voices.

Storage (excluding object/vector metadata) is approximately
`24*N + 32*C*N + 16*C*(N/2+1) + 32*(N/2+1) + 8*H` bytes on platforms with
8-byte double/size_t and 16-byte complex<double>. At N=2048,H=256 this is about
162 KiB mono and 242 KiB stereo. Memory does not grow with callback size or
stream duration. Ring positions and phases remain bounded.

`process`, `set_pitch_ratio` and `reset` are noexcept, with no heap operations,
locks or I/O. Nonfinite input is treated as silence; values smaller than 1e-30
are suppressed at input and accumulation boundaries. This floor prevents
persistent denormal tails for float/double without changing thread-wide FP
flags. Output is clipped only at the representable sample-type bounds. This
is numerical protection, not saturation modeling or an audio limiter.

## Stereo behavior and limitations

All channels share per-source-bin rotation and destination mapping, while
retaining separate complex coefficients and time-domain buffers. A silent
channel stays silent; identical, polarity-inverted and scaled channels retain
those relationships. Inter-channel phase differences are retained within a
source bin. This avoids independent phase accumulators drifting apart.
For unrelated material competing in the same bin, the strongest channel controls
frequency estimation; collisions at a destination bin can change the resulting
phase relationship. The algorithm does not guarantee preservation of arbitrary
broadband stereo geometry or inter-channel time delays after shifting.

Nearest-bin mapping can cause amplitude ripple, spectral coloration and
changes when bins cross a mapping boundary. Pitch changes are smoothed and
frames overlap; this reduces clicks but does not promise artifact-free modulation,
especially over large ranges. Low notes and closely spaced partials need longer
windows. No peak phase locking, transient preservation, formant preservation,
resampling antialias filter or perceptual loudness compensation is provided.
Shifting up discards out-of-band destination bins, with finite-window leakage
near Nyquist. Shifting down can merge bins. Source frequency estimates can be
ambiguous for noisy/rapidly varying material. These are quality limits, distinct
from the supported ratio range.

The general STFT phase-propagation basis is described in
[Laroche and Dolson's phase-vocoder paper](https://www.ee.columbia.edu/~dpwe/papers/LaroD99-pvoc.pdf).
This implementation is a basic source-bin method, not their peak-based algorithm.

## CuteAudioPoC integration

Create one shifter for each voice, keeping stereo channels in the same shifter.
Prepare outside the callback with the host sample rate, channel count and agreed
N/H. Apply detune in cents via the ratio formula above. Keep N/H identical across
voices to align their fixed delays. Call ratio control on the audio thread and
retain stream history across callbacks. Reprepare while processing is stopped
when sample rate or format changes, and update the host's latency if N changes.

Voice distribution, pan, time offset and dry/wet mixing remain in CuteAudioPoC.
Delay the dry path by N samples when mixing with the processed path. Feed zeros
to render the tail when input ends; N zeros suffice to recover a unity stream's
remaining samples. Test and measure aggregate CPU use before selecting a voice
count. This riw task does not modify CuteAudioPoC or its submodule revision.

## Verification

The runtime suite covers unity impulses at different hop offsets, sine and seeded
noise at N=64/256/1024 with H=N/4 and N/8; double reconstruction tolerance is
2e-11. Float stereo unity is checked to 2e-7. Tone pitch is measured independently
by windowed sinusoidal projection at 220.3 and 997.1 Hz, ratios for +/-30 and
+/-1200 cents, and sample rates 44.1/48/96 kHz, with a 0.5-cent acceptance limit.
The observed largest error on the development machine was 0.000181 cents.

Other tests cover sample-rate reprepare, reset, invalid configuration/ratios,
exact one-sample versus whole-block versus in-place agreement, smoothing with
different block sizes, silent/scaled/inverted stereo channels, quadrature phase,
and identical attack timing after different silent histories. The callback heap
probe observes ordinary new/delete across 3,084,000 samples per channel; no
allocation or deallocation occurred, and all outputs stayed finite. Denormal
and NaN inputs are checked as silence. The FFT addition matches the existing
API and round-trips, and the new header compiles independently with static
noexcept guarantees. ASan/UBSan checks pass for the DSP and FFT tests.

The full Bazel suite passed all 9 test targets. On the development Mac, the
unmodified invocation encountered an Xcode SDK dependency-tracking error;
verification retained dependency tracking and used this invocation-only override:

```sh
bazel --output_user_root=/private/tmp/riw-phase-vocoder-bazel test //... \
  --test_output=all --copt=-isysroot \
  --copt=/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk
```

`work/phase-vocoder/stress.cpp` supplies an optional ten-minute stereo stream
at 48 kHz with the default N=2048,H=256, sweeping the supported ratio range.
Build/run it from the repository root with:

```sh
clang++ -std=c++20 -O2 -Iinclude work/phase-vocoder/stress.cpp -o /tmp/riw-stress
/tmp/riw-stress
```

This is a correctness/stability exercise, not a callback deadline benchmark.
