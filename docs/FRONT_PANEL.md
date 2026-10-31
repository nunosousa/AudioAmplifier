# Front-panel concept

**Status:** simplified, revisable UI proposal. Exact display module, parameter
ranges, MCU, and control gestures are not finalized.

![Compact amplifier front panel with five labeled push-turn controls, individual LED segments forming a numeric value, and system-status LEDs](front-panel.svg)

## Concept

Keep the interface direct and compact:

- Five dedicated knobs, each labeled with its function and unit.
- Press and turn a knob to adjust its setting; the numeric display shows that
  setting's current value while it is being adjusted.
- A small set of separate LEDs indicates system state.

The concept intentionally drops the RGB matrix, VU/spectrogram views, and
display-mode selection. As a result, the user interface does not require an
audio sample or spectrum-data path from the DSP to the MCU. The SVG shows an
illustrative `-6.0` value; the units are conveyed by the label on the active
knob, not repeated on the display.

## Controls

Use five labeled rotary controls with push switches:

| Knob label | Function | Unit shown on knob |
| --- | --- | --- |
| `MASTER [dB]` | Shared level for left, right, and subwoofer outputs | dB attenuation |
| `BASS [dB]` | Low-frequency EQ adjustment | Signed dB relative to neutral |
| `TREBLE [dB]` | High-frequency EQ adjustment | Signed dB relative to neutral |
| `SUB LEVEL [dB]` | Additional trim on top of shared master volume | Signed dB relative to neutral |
| `CROSSOVER [Hz]` | Shared stereo high-pass / subwoofer low-pass cutoff | Hz |

The proposed interaction is press-and-turn, with the value visible during
adjustment. Confirm whether the press must be held while turning or whether a
press selects the control and a subsequent turn adjusts it. Define any
single-press action—such as master mute—so it cannot conflict with adjustment.

## Numeric display

Use individual LED segments arranged into seven-segment digit characters.
Only the adjusted quantity is shown; the active knob label already identifies
the parameter and its unit. The concept shows `-6.0` without a unit suffix.
Choose enough digit positions and decimal-point placement to support every
parameter range, including negative EQ/sub-trim values, master attenuation,
and crossover frequency.

The active control is identified by the knob the user is pressing and turning;
the numeric value is the feedback. Keep the display otherwise blank or
dimmed, rather than adding a permanent menu or additional mode. Decide whether
the value disappears immediately on release or remains briefly for
confirmation.

The master value applies to all outputs. Sub level remains an additional
subwoofer trim. Set display precision and encoder step size together so each
detent produces clear, useful feedback.

## Status indicators and startup

Use a few independent LEDs for essential state:

- **POWER:** control supply is present.
- **DSP READY:** DSP startup/configuration is complete and parameter
  communication is available.
- **MUTE:** audio is intentionally muted.
- **FAULT:** optional; include only if amplifier fault signals or another
  protection condition are actually monitored.

On power-up, keep the amplifier muted, initialize the MCU and DSP, load
validated defaults or saved settings, then indicate DSP Ready and unmute with
a controlled ramp. Define mute and recovery behavior for any detected faults.
Smooth gain and filter coefficient changes to avoid zipper noise, clicks,
unstable coefficients, and transient clipping.

## MCU and hardware implications

The simplified interface needs GPIO or encoder inputs for five controls,
push-switch handling, the DSP control interface, a small seven-segment display
driver, and a few status LEDs. It no longer needs RGB matrix refresh or an
audio stream/FFT path. This makes a low-cost MCU more plausible; choose the
exact part after checking its available interfaces, GPIO count, display-driver
choice, and firmware/debug support.

An STM32G0 remains a candidate; an RP2040 is still usable but is not required
by the display workload alone. A J-Link EDU is available. Reserve SWD access
and confirm target-voltage and probe requirements for the selected board.

Use a suitable display driver or multiplexing circuit for the individual
segment LEDs. Check logic compatibility, LED current and brightness,
decoupling, and switching-noise coupling into the analog audio path. Select
display color and brightness for legibility in the console rack.

## Not finalized

- Exact display digit count, module type, driver, color, and brightness.
- Display formatting, decimal precision, parameter ranges, and encoder steps.
- Press-and-turn semantics, release behavior, and whether a press has a
  separate action such as mute.
- Exact MCU, display-driver circuit, and DSP control/boot interface.
- Defaults, saved-setting policy, startup sequence, and mute/fault recovery.
- Which amplifier fault signals are monitored.
