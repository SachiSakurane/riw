# Streaming frequency shifting

`riw::frequency_shifter<Sample>` in `<riw/dsp/frequency_shifter.hpp>` moves
each positive-frequency component by a signed offset in Hz. For example,
+100 Hz moves 1000/2000 Hz tones to 1100/2100 Hz; it does not preserve harmonic
ratios as a pitch shifter would. It accepts planar synchronous channels and
produces one output sample per input sample.

```cpp
#include <array>
#include <riw/dsp/frequency_shifter.hpp>

riw::frequency_shifter<float> shifter;
// Outside the audio callback, while processing is stopped:
bool ok = shifter.prepare({48000.0, 2, 257, 0.02});
// Check ok before using the stream; prepare may throw on allocation failure.
ok = shifter.set_shift_hz(100.0);
shifter.reset(); // Clear history and start immediately at the target.

// Audio thread: pointers each address frames samples.
std::array<const float *, 2> input{left_input, right_input};
std::array<float *, 2> output{left_output, right_output};
ok = shifter.process(input, output, frames);
// Host/dry-path compensation: shifter.latency_samples() == 128.
```

## Configuration and streaming contract

- Sample rate must be finite and positive; channels must be 1–8.
- `filter_size` must be odd and in [7, 4095]. Default: 257 taps, with a group
  delay of `(filter_size - 1)/2` samples (128 samples, about 2.67 ms at 48 kHz).
  Longer filters improve sideband rejection near DC and Nyquist at the cost of
  delay and CPU. Very short filters are supported but have poor separation.
- `smoothing_seconds` must be finite and nonnegative. Default: 0.02 seconds.
  A one-pole smoother updates the shift every sample with
  `1-exp(-1/(sample_rate*smoothing_seconds))`; zero makes changes immediate.
  `set_shift_hz` retains oscillator phase even with immediate changes.
- `set_shift_hz` requires preparation and a finite value in
  `[-sample_rate/2, sample_rate/2]`. Invalid values preserve the current target.
  The accepted range is a control range, not a guarantee of alias-free output.
- Successful `prepare` allocates buffers and resets the shift to zero, clears
  history, and starts phase at zero. Invalid configuration returns false with
  the old stream intact. Allocation failure can throw, also preserving it.
- `reset` retains configuration and target, clears history and oscillator phase,
  and snaps the smoother to the target. Reset during playback can click.
- `process` accepts arbitrary frame counts, including zero, and exactly one
  input/output pointer per configured channel. Nonempty calls require nonnull
  pointers. Invalid shape/unprepared calls return false without modifying state
  or output. Callers provide sufficiently large backing storage.
- Same-channel in-place processing is supported; other overlap is prohibited.
  Identical, scaled, inverted and silent channels retain those relationships
  within sample precision because they share smoothing and oscillator phase.
- `process`, `reset`, and `set_shift_hz` are noexcept and perform no allocation,
  locking or I/O. Preparation, copies, ownership transfers and destruction belong
  outside the callback. Reprepare a moved-from object before using it.
- The object is single-thread-owned. Schedule changes on the processing thread,
  splitting blocks at the desired sample; do not change it concurrently.

At zero shift after preparation/reset, output is exactly the delayed input
apart from numerical sanitization. Returning to zero after modulation retains
the accumulated oscillator phase, so it is not a transparent bypass. For a dry/wet
mix, delay the dry path by `latency_samples()`. Feed `filter_size - 1` zeros to
flush all FIR history at the end of a shifted stream; at zero shift the group
delay alone suffices. For nonzero shifts the FIR transient can start before the
group delay, as with other causal linear-phase filters.

## Algorithm and quality limits

The FIR uses the ideal discrete Hilbert kernel `2/(pi*m)` for odd offsets m,
zero for even offsets, truncated with a symmetric Blackman window. Its imaginary
path and the delayed real path form an approximate analytic signal. Multiplying
by a shared complex oscillator and taking the real part gives
`delayed_input*cos(phase) - hilbert_input*sin(phase)`. Phase remains bounded and
advances by `2*pi*smoothed_shift/sample_rate` per sample. The analytic-signal
construction is described in the
[SciPy Hilbert documentation](https://docs.scipy.org/doc/scipy/reference/generated/scipy.signal.hilbert.html)
and the windowed FIR design in Julius O. Smith's
[Hilbert transform design example](https://www.dsprelated.com/freebooks/sasp/Hilbert_Transform_Design_Example.html).

Finite FIR length leaves an unwanted opposite sideband, especially near DC and
Nyquist. There is no ideal broadband rejection guarantee. DC has zero Hilbert
component, so DC input modulates as a cosine. Downward-shifted components crossing
DC appear at the absolute frequency; upward-shifted components crossing Nyquist
alias. No band limiting, oversampling, antialias filtering, pitch/formant
preservation or output limiter is supplied. Keep source frequencies and shifted
frequencies away from these boundaries when clean single-sideband output matters.

Work is O(channels * filter_size) per sample; odd antisymmetric tap pairs reduce
the multiplication count. Storage is O(channels * filter_size), independent of
block length. Measure callback cost on the target hardware before deploying
many voices. Computation and buffers use double; nonfinite input and magnitudes
below 1e-30 are replaced with zero, and output is clamped to the sample type's
representable bounds. This is numerical protection, not audio saturation.

## Verification

Runtime tests cover exact zero-shift impulse/noise/DC delay at multiple filter
sizes; additive shifts of +/-300 Hz for simultaneous 2/5 kHz tones at
44.1/48/96 kHz; intended-tone amplitude and opposite-sideband amplitude;
sample-by-sample versus whole-block processing during smoothed automation;
closed-form smoothing and phase continuity after controls change;
in-place operation; float stereo proportionality and silence; reset/reprepare;
invalid controls/configuration/pointers and empty blocks; and nonfinite/tiny
input. A separate translation unit verifies a self-contained header and noexcept
processing/control/reset signatures.

The eight frequency-shifter runtime tests also pass with AddressSanitizer and
UndefinedBehaviorSanitizer. On the development Mac, the full Bazel suite uses
an invocation-only writable output root and SDK override:

```sh
bazel --output_user_root=/private/tmp/riw-phase-vocoder-bazel test //... \
  --test_output=all --copt=-isysroot \
  --copt=/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk
```
