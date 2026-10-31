# Front-panel concept

**Status:** revisable UI proposal. Matrix technology, dimensions, display
views, parameter ranges, and MCU are not finalized.

![Retro-style front-panel mockup with five rotary controls and one RGB LED panel shown in spectrogram, VU, blank, and contextual adjustment modes](front-panel.svg)

## Concept

Use one RGB LED panel for normal-play visuals and temporary control feedback:

- **VU-style meter:** show left, right, and subwoofer levels in distinct bands.
- **Spectrogram:** show frequency content over time using color for intensity.
- **Blank:** turn off the matrix during playback when a visual is not wanted.
- **Adjustment context:** when a knob moves, temporarily show which parameter
  changed and its current value; then return to the selected normal-play view.

The SVG illustrates the three normal-play choices and an example adjustment
readout. It is not to scale and does not commit the design to a particular
matrix size, pixel pitch, protocol, color mapping, or enclosure.

How the user selects VU, spectrogram, or blank is not decided. Options include
a small dedicated VIEW button or a deliberate long-press/press gesture on an
existing control. Prefer an explicit, discoverable gesture; do not make a
normal setting change depend on an undocumented mode.

## Controls

Use five labeled detented rotary encoders:

| Control | Function | Proposed value meaning |
| --- | --- | --- |
| Master | Shared level for left, right, and subwoofer outputs | Attenuation or normalized level; choose one convention |
| Bass | Low-frequency EQ adjustment | Signed dB relative to neutral |
| Treble | High-frequency EQ adjustment | Signed dB relative to neutral |
| Sub level | Additional trim on top of shared master volume | Signed dB relative to neutral |
| Crossover | Shared stereo high-pass / subwoofer low-pass cutoff | Frequency in Hz |

Push the Master encoder to toggle mute. Keep mute visible in every display
mode, either through a separate indicator or an unmistakable matrix overlay.

## Display behavior and content

- At power-up, use a selected default visual (suggested: VU) after DSP startup.
- The VU view should identify L/R/SUB and use consistent, documented
  ballistics and level ranges.
- The spectrogram should define frequency scale, time direction, and intensity
  color mapping. Avoid implying calibrated acoustic SPL or speaker power.
- Blank means the RGB panel is off; essential state such as mute, DSP failure,
  or a real amplifier fault should remain indicated separately.
- Turning any encoder immediately overrides the normal visual with contextual
  information: parameter name, value, and units. Return to the previously
  selected visual after a short idle timeout.
- Use a compact custom font and concise labels such as `BASS`, `TREB`, `SUB`,
  `XOVR`, and `MAST`. Keep abbreviations and units consistent. Verify that
  signed values, decimals, and crossover frequencies fit the actual matrix.
- The master value applies to all outputs; the sub level is an additional
  subwoofer trim.

## Audio data for the visuals

A VU-style meter needs a level estimate for each displayed channel. A
spectrogram needs sampled audio or sufficiently detailed spectral data. These
are different data requirements:

- For the VU view, investigate whether the SigmaDSP design can provide
  smoothed level values through a documented and adequately fast control
  interface. Do not assume its control port can stream audio-rate data.
- For the spectrogram, determine how audio samples or FFT bins reach the MCU.
  Candidate architectures include a supported serial audio path from the DSP,
  an additional audio ADC feeding the MCU, or FFT/spectrum processing within
  the DSP with a suitable data-readback mechanism. Verify interface pins,
  bandwidth, DSP/MCU resources, and signal integrity before selecting one.
- An RP2040 can render the matrix and has resources for a modest FFT, but it
  cannot compute a spectrogram without an audio sample source. Treat the
  spectrogram feature as conditional on solving that signal path.
- Metering a DSP tap indicates signal level at that point, not speaker
  voltage, acoustic SPL, or amplifier output power.

## Indicators and startup

Keep essential status independent of the selected visual:

- **POWER:** control supply is present.
- **DSP READY:** DSP startup/configuration is complete and parameter
  communication is available.
- **MUTE:** audio is intentionally muted.
- **FAULT:** only if amplifier fault signals or another protection condition
  are actually monitored; label it to match what is detected.

On power-up, keep the amplifier muted, initialize the MCU and DSP, load
validated defaults or saved settings, then indicate DSP Ready and unmute with a
controlled ramp. Define mute and recovery behavior for any detected faults.

Smooth gain and filter coefficient changes to avoid zipper noise, clicks,
unstable coefficients, and transient clipping. If settings are saved in MCU
nonvolatile memory, debounce writes and define first-boot and invalid-setting
behavior.

## MCU direction

An inexpensive, widely available ARM MCU remains the goal. The richer RGB
matrix and possible spectrogram make an **RP2040** a reasonable candidate to
evaluate alongside STM32G0. RP2040 has PIO and DMA resources useful for LED
panel refresh and enough processing capability for a modest display-side FFT,
but its external audio-data path must be designed separately. It also requires
external QSPI flash for program storage.

An STM32G0 may still be sufficient if the matrix driver handles refresh and
the DSP provides low-rate meter data. Compare actual interface requirements,
not just processor speed:

- LED-panel protocol, resolution, refresh rate, brightness, and current.
- VU/spectrogram data path and update rate.
- DSP control-bus access and parameter-update latency.
- Five encoder inputs, optional VIEW button, status LEDs, and settings storage.
- Supply/logic levels, package availability, firmware toolchain, and debug
  support.

A J-Link EDU is available. RP2040 supports SWD debugging; reserve SWDIO, SWCLK,
ground, target-voltage reference, and reset access as required by the selected
board/probe setup. Confirm probe configuration and licensing for the intended
use. The J-Link is a development tool, not a product component.

## LED panel and integration

Select the panel and driver after estimating resolution, pixel pitch, viewing
distance, refresh requirements, brightness, and peak current. A HUB75-style
panel or a serial-addressable matrix are possible classes of solution; the
specific interface is open. Do not assume a panel can be driven directly by
the MCU without a suitable driver, level compatibility, and power design.

RGB panel refresh can generate substantial switching current and EMI. Plan
its supply, decoupling, return-current path, brightness control, and physical
placement so it does not contaminate the analog input and DSP paths. Verify
readability and the color scheme in the console rack lighting.

## Not finalized

- Matrix dimensions, pixel pitch, technology/interface, brightness, and
  viewing angle.
- VU ballistics, calibration, color thresholds, and meter data source.
- Spectrogram audio source, FFT implementation, frequency scale, and update
  rate.
- How users select VU, spectrogram, and blank modes.
- Custom font, abbreviations, units, and formatting for all parameter ranges.
- Exact MCU and display-driver architecture; RP2040 and STM32G0 remain options.
- Encoder type, detent count, rotation acceleration, and push-switch behavior
  beyond master mute.
- Parameter ranges, steps, defaults, and saved-setting policy.
- DSP boot method, parameter interface, and update/smoothing implementation.
- Which amplifier fault signals are monitored and the resulting recovery
  behavior.
