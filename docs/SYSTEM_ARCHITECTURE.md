# System architecture

**Status:** Preliminary target architecture; implementation and interfaces
remain subject to schematic, firmware, and bench validation.

This document records the intended system-level architecture for the desktop
2.1 amplifier. It is a design target, not a description of completed hardware.
The current schematics and firmware do not yet implement the full architecture.
See the [design status](./DESIGN_STATUS.md) for the verified state of the
repository and the [ADAU1701 integration plan](./ADAU1701_INTEGRATION.md) for
device-level circuit requirements.

## System overview

The amplifier is intended to accept either a stereo analog line input or
stereo USB audio from a computer or compatible phone. Both sources feed the
same ADAU1701 processing and output chain. The RP2350 is the proposed system
controller, handling USB audio, audio-data transfer, DSP control, and the
front-panel interface.

| Block | Intended role |
| --- | --- |
| RP2350 | System control, USB Audio Class sink, PIO/DMA I²S transmission, and user interface |
| ADAU1701 | Analog conversion, source selection, audio processing, and DAC output |
| 12.288 MHz oscillator | Proposed external ADAU1701 master-clock reference for 48 kHz operation |
| TPA3116D2 × 2 | Stereo and subwoofer Class-D amplification; exact output configuration remains open |
| Front panel | Five push-turn controls, numeric display, and system-status LEDs |

The intended DSP functions are source selection, shared master volume, bass
and treble adjustment, stereo/subwoofer crossover, subwoofer generation, and
output-level management. The SigmaStudio graph and parameter mapping must be
verified against these behaviors; see the [DSP integration notes](./ADAU1701_INTEGRATION.md#current-sigmastudio-processing).

## Audio paths

**Analog input**

```text
Stereo line input → input conditioning/protection → ADAU1701 ADCs
```

**USB input**

```text
USB host → RP2350 USB Audio sink → buffered audio → RP2350 PIO I²S transmitter
         → ADAU1701 serial input
```

**Common output**

```text
ADAU1701 DSP → DAC outputs → analog output drivers → TPA3116D2 stages
             → left/right speakers and subwoofer
```

Both sources are intended to use the same DSP processing and output stages.
The analog and USB inputs are a proposed selectable pair; the current
schematic's analog input path does not yet establish that complete behavior.
Input selection, channel mapping, output-driver topology, and amplifier
interfaces must be checked together with the SigmaStudio project.

The existing ADC input design notes identify a critical analog-interface
check: the preamp buffer's +4.5 V bias must not be passed as DC to an ADAU1701
ADC pin, whose input common mode is about +1.5 V. Preserve the required
AC-coupling/current-input interface and complete the checks in the
[ADC conditioning plan](./ADAU1701_INTEGRATION.md#3-stereo-adc-input-conditioning)
before connecting or powering the ADC path.

## Audio format and clock ownership

The initial target is fixed-rate 48 kHz stereo I²S, with up to 24 meaningful
bits in 32-bit slots (64 BCLK cycles per stereo frame). 44.1 kHz and automatic
sample-rate switching are out of scope for the initial design.

The proposed clock arrangement is:

- A dedicated 12.288 MHz oscillator supplies ADAU1701 MCLK (256 × 48 kHz).
- The ADAU1701 is the I²S clock master; its serial output clock generation
  provides 3.072 MHz BCLK and 48 kHz LRCLK.
- Those clocks are looped back to the ADAU1701 serial input clock pins and
  connected to RP2350 GPIO inputs. The RP2350 is an I²S data transmitter and
  clock slave; it does not generate BCLK or LRCLK.
- The RP2350 uses PIO for serial data timing and DMA to feed audio buffers to
  the PIO transmit FIFO. Buffering, channel alignment, the I²S one-bit delay,
  and data setup/hold margin require implementation and measurement.

The exact oscillator, ADAU1701 multifunction-pin configuration, clock
dividers, and RP2350 PIO implementation are not finalized. Confirm all clock
and serial-port settings against the ADAU1701 datasheet, SigmaStudio
configuration, RP2350 package, and measured timing before treating this
topology as implemented.

The provisional signal mapping is:

| Signal | ADAU1701 | RP2350 |
| --- | --- | --- |
| LRCLK output | MP10 / OUTPUT_LRCLK, looped to MP4 / INPUT_LRCLK | GPIO11 input |
| BCLK output | MP11 / OUTPUT_BCLK, looped to MP5 / INPUT_BCLK | GPIO10 input |
| Audio data input | MP0 / SDATA_IN0 | GPIO12 output |

These multifunction-pin assignments and GPIO numbers are proposals, not
verified schematic connections. Check the selected pin modes, package
availability, and signal integrity before layout.

The PIO instruction engine uses the RP2350 system clock; at the proposed
150 MHz system-clock rate there are about 49 system-clock periods per BCLK
period. Treat this as a timing budget to validate, not proof of adequate edge
detection or data setup/hold margin.

## USB audio and buffering

The intended USB function is a USB Audio Class 1.0 stereo sink at 48 kHz,
initially supporting 16-bit and potentially 24-bit samples. Recording,
microphone, and USB audio-output functions are not in scope.

The ADAU1701 owns the audio clock domain. USB packet delivery therefore has to
track the actual audio consumption rate. The asynchronous sink's host
feedback mechanism, supported sample formats, and compatibility must be
confirmed with the selected RP2350 USB device stack; these are feasibility
items, not completed firmware decisions.

Audio buffers and DMA must tolerate USB packet timing without CPU-driven
per-bit work. Firmware must detect and recover from underruns and manage
synchronization between USB delivery and the ADAU1701 clock. USB does not
provide galvanic isolation, so grounding, power distribution, and
analog/digital noise coupling remain design concerns.

## System control and user interface

The RP2350 is the proposed controller for:

- USB audio reception and I²S/DMA buffering.
- ADAU1701 boot, configuration, and parameter updates (I²C is the current
  control-port proposal).
- Five labeled push-turn controls for master, bass, treble, sub level, and
  crossover.
- A compact seven-segment numeric display and separate status LEDs.
- Amplifier mute/enable and controlled startup/shutdown sequencing.

The master setting applies to left, right, and subwoofer outputs; sub level is
an additional subwoofer trim. Keep the amplifier muted during initialization
and apply validated DSP defaults before unmuting. Define parameter limits,
saved-setting behavior, mute/fault recovery, and smoothing of gain and filter
changes. See the [front-panel concept](./FRONT_PANEL.md) for the proposed
interaction and unresolved UI details.

GPIO assignments, controller package, DSP control-port wiring, display
driver, amplifier fault monitoring, and power sequencing remain to be
finalized. The proposed RP2350 audio GPIOs (GPIO10 BCLK, GPIO11 LRCLK, GPIO12
DATA) are provisional and must be checked against the selected package and
board routing.

## Open design and validation work

Before the architecture can be considered implemented:

- Complete and review the ADAU1701 power, clock, reset, boot, control,
  multifunction-pin, ADC, and DAC circuitry.
- Confirm clock-master configuration, clock loopbacks, I²S framing, PIO edge
  behavior, and data alignment.
- Select the RP2350 package and verify GPIO, USB, debug, and board-routing
  requirements.
- Prove the USB Audio Class 1.0 asynchronous sink and host-feedback behavior
  on the chosen USB stack; validate supported hosts and formats.
- Implement DMA buffering and underrun handling, then measure synchronization
  and timing margin.
- Finalize analog input protection/conditioning and verify ADC headroom,
  common mode, noise, and clipping.
- Verify the SigmaStudio source selection, control mapping, crossover,
  channel routing, output levels, and generated program artifacts.
- Finalize DAC output drivers, the TPA3116D2 operating/output configuration,
  mute/enable sequencing, power, grounding, and thermal design.
- Validate audio response, noise, distortion, switching between sources,
  startup/shutdown transients, and USB compatibility.

For board-specific blockers and validation status, consult
[design status](./DESIGN_STATUS.md). Do not infer completion from this
architecture proposal or from existing generated artifacts.
