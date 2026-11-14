# Main 2.1 amplifier: design status

This document records the intended architecture and the known state of the
design files. It distinguishes stated goals from implemented and verified
hardware; schematic labels and existing generated artifacts are not proof that
the circuit is complete.

## Target architecture

The intended system accepts either a stereo analog line input or stereo USB
audio and routes the selected source through a common ADAU1701 DSP chain to
the stereo and subwoofer amplifier stages. The RP2350 is the proposed system
controller and USB audio receiver. The initial audio target is fixed-rate
48 kHz stereo, with the ADAU1701 owning the audio clock domain.

The proposed clock/data arrangement uses a 12.288 MHz ADAU1701 master clock,
ADAU1701-generated 3.072 MHz BCLK and 48 kHz LRCLK, and an RP2350 PIO/DMA
transmitter operating as an I²S data slave. USB audio is intended as a
48 kHz USB Audio Class 1.0 stereo sink, with asynchronous feedback to match
the ADAU1701's actual consumption rate. Clock details, PIO timing, and USB
stack behavior remain to be verified; see the
[system architecture](./SYSTEM_ARCHITECTURE.md).

## Intended signal path

The main product is a stereo-plus-subwoofer amplifier. Its proposed paths are:

1. Accept left/right analog line-level input or stereo USB audio.
2. Condition analog signals for the ADAU1701 ADC inputs; transmit USB audio
   from the RP2350 to the ADAU1701 serial input.
3. Select the source and perform master volume, bass/treble adjustment, and
   stereo-to-2.1 crossover processing in the ADAU1701.
4. Produce left, right, and subwoofer audio outputs.
5. Condition/drive those signals into the differential input interfaces of
   the stereo and mono TPA3116D2 amplifier stages.

This describes the target architecture, not implemented or verified
functionality. In particular, the current schematic does not yet establish
USB audio, RP2350 I²S, source selection, or a complete ADAU1701 circuit.

The standalone analog crossover is no longer the target implementation.
[`crossover.kicad_sch`](../electronics/audio-amplifier/crossover.kicad_sch) is retained
in the repository as a reference, but is not part of the current
`AudioAmplifier.kicad_sch` sheet hierarchy.

## What is present

- The top-level KiCad sheet includes DSP, preamplifier, power, control, and
  stereo/mono amplifier sheets.
- The DSP sheet contains an ADAU1701 symbol and hierarchical labels for stereo
  inputs and six proposed differential amplifier-input signals.
- The preamplifier sheet has been reduced from its previous tone/volume-control
  design. Its intended remaining role is input conditioning; that function
  still needs to be checked against the DSP ADC requirements.
- The existing stereo and mono power-amplifier sheets use TPA3116D2 devices.
- The SigmaStudio project has an initial DSP signal chain: bass and treble
  adjustment, an adjustable 2.1 crossover, and separate stereo/subwoofer
  output volume controls. See the [DSP processing notes](./ADAU1701_INTEGRATION.md#current-sigmastudio-processing).
- Generated ADAU1701 program files are present, but have not yet been verified
  against the latest SigmaStudio project and hardware interface.
- The project contains a custom KiCad symbol library for amplifier devices.

<<<<<<< HEAD
The current front-panel proposal uses an RP2350, a 2.4-inch 320 × 240 colour
display, one main rotary encoder, dedicated BASS/TREBLE/SUB/XOVER/DISPLAY/MUTE
buttons, and a context-sensitive instrumentation UI. Animated spectrum and VU
modes require suitable DSP telemetry. See the
[front-panel specification](./FRONT_PANEL.md) and
[UI firmware architecture](./UI_FIRMWARE_ARCHITECTURE.md).
=======
The current front-panel proposal uses five labeled push-turn controls (master,
bass, treble, sub level, crossover), a compact seven-segment value display,
and separate status LEDs. The RP2350 is the proposed controller to read the
controls, drive the display, and update the ADAU1701; its exact package and the
display driver remain open. The simplified display does not require an
audio-data path from the DSP. See the [front-panel concept](./FRONT_PANEL.md).
>>>>>>> 4c9065f (Refine documentation for system architecture, design status, and front panel; clarify input paths and controller details)

See the [ADAU1701 integration plan](./ADAU1701_INTEGRATION.md) for a package
pin-by-pin connection checklist, proposed channel mapping, and bring-up checks.

## Blocking gaps

### DSP schematic and analog interfaces

[`dsp.kicad_sch`](../electronics/audio-amplifier/dsp.kicad_sch) is not a complete
circuit. The ADC input paths now have 18 kohm series resistors and partial
pin wiring, but the external ADC_RES resistor and tolerance/matching details
are still missing. Power, clock, reset, boot/configuration, and other required
external circuitry are also incomplete. Complete and check the actual pin-level
circuit before considering the DSP electrically implemented.

The top-level interface proposes three differential outputs, while the
ADAU1701 provides four DAC outputs. Determine the output-channel mapping and
design the required analog output stages to drive the TPA3116D2 differential
inputs. Do not assume that connecting DAC pins directly to amplifier inputs
will provide the required level, polarity, or common-mode behavior.

### DSP controls and program

<<<<<<< HEAD
The interface uses an RP2350, but the
[`mcu.kicad_sch`](../electronics/audio-amplifier/mcu.kicad_sch) sheet is still
empty. Select its exact package, define display/control-port wiring, encoder
and button circuits, DSP telemetry transport, and SWD access, then add the
hardware and firmware. See the [front-panel specification](./FRONT_PANEL.md).
=======
The proposed controller is an RP2350, but the
[`mcu.kicad_sch`](../ECAD/AudioAmplifier/mcu.kicad_sch) sheet is still empty.
Select the exact package and confirm GPIO, USB, debug, and board-routing
requirements. Define the DSP control-port wiring, USB Audio Class 1.0 sink and
feedback behavior, PIO/DMA audio path, seven-segment display interface, and
user-input circuits, then add hardware and firmware. The USB stack and
asynchronous feedback feasibility are blocking firmware questions. See the
[system architecture](./SYSTEM_ARCHITECTURE.md) and
[front-panel concept](./FRONT_PANEL.md).
>>>>>>> 4c9065f (Refine documentation for system architecture, design status, and front panel; clarify input paths and controller details)

The reported SigmaStudio processing chain is a starting point, not verified
evidence that the committed program artifacts match the current project. Its
committed compiler report describes a 48 kHz build using 232 of 1024
instructions, 124 of 2048 data-RAM units, and 752 of 1024 parameter-RAM units.
Verify the project routing, filter response, gain, parameter limits, and
artifact freshness, then regenerate and identify the corresponding program
artifacts together.

### PCB, simulation, and exports

- [`AudioAmplifier.kicad_pcb`](../electronics/audio-amplifier/AudioAmplifier.kicad_pcb)
  currently has no placed footprints. Rebuild the board from the updated
  schematic; no previous placement or routing should be assumed to remain.
- [`Audio_path.asc`](../electronics/audio-amplifier/simulation/LTspice/Audio_path.asc)
  is only an LTspice header. Create a simulation that exercises the relevant
  input conditioning, DSP/output-stage assumptions, and amplifier input
  interfaces where models permit.
- [`AudioAmplifier.pdf`](../electronics/audio-amplifier/AudioAmplifier.pdf) predates
  the latest schematic. Regenerate it after the schematic stabilizes.
- KiCad ERC and PCB DRC have not been run on the current design.

## Decisions to make

Resolve these with the intended input sources, speakers, enclosure, controls,
and supply before finalizing the circuit or PCB:

| Topic | Decision / verification needed |
| --- | --- |
| Input range | Pair of unbalanced RCA line inputs; expected source impedance 50 ohm to 2.5 kohm. Levels: 0.5 Vrms nominal, 0.2 Vrms minimum, 5.65 Vpp maximum, with 0 to +1 V superimposed DC. Confirm source-specific impedance/level (especially SCART adapters), input protection, and whether maximum AC and DC occur together. |
| ADC interface | At the selected 48 kHz rate, ADAU1701 ADCs are current inputs with about 1.5 V pin common mode; 18 kohm external ADC_RES and 18 kohm external input resistors set nominal 2.0 Vrms full scale. Confirm the AC-coupled interface from the +4.5 V-biased buffer and test clipping/tolerance margin before wiring ADC0/ADC1. |
| Output interface | DAC channel assignment, output level, differential-driver topology, filtering, and amplifier input compatibility |
| DSP behavior | Crossover frequency and response, channel summing, EQ range, boost/headroom policy, and volume range |
<<<<<<< HEAD
| User controls | One main encoder; BASS, TREBLE, SUB, XOVER, DISPLAY, and MUTE buttons; and a 320 × 240 colour display are specified. Confirm display module/controller, electrical interface, and enclosure layout. |
| Volume behavior | One shared master volume for all outputs, plus a separate subwoofer level trim |
| Startup and faults | DSP boot/configuration method, amplifier mute/enable sequencing, fault response, and pop suppression |
| MCU | RP2350 specified; confirm exact package, board architecture, debug access, power, and DSP/display interfaces |
=======
| User controls | Five labeled push-turn controls, a seven-segment value display, and separate status LEDs are proposed; press behavior and display details remain open |
| Volume behavior | One shared master volume for all outputs, plus a separate subwoofer level trim |
| Startup and faults | DSP boot/configuration method, amplifier mute/enable sequencing, fault response, and pop suppression |
| MCU | RP2350 is the proposed system controller; exact package, GPIO allocation, USB stack, and firmware remain open |
>>>>>>> 4c9065f (Refine documentation for system architecture, design status, and front panel; clarify input paths and controller details)
| Power | Input supply range/current, analog/DSP rails, decoupling, grounding, and separation from Class-D switching currents |
| Amplifier use | Speaker impedance, target output power, thermal limits, heatsinking, and protection behavior |

The input levels above are the current project specification. The preamp
schematic implements the proposed conditioning network, but the stated maximum
signal and DC offset must be interpreted together and checked against the
source, component, op-amp, and ADAU1701 ADC limits. In particular, the
schematic's +4.5 V bias would put the unity-buffer output near +4.5 V DC; the
ADC common-mode compatibility and full-scale level are blocking checks before
connecting it. See the input-stage review in the
[ADAU1701 integration plan](./ADAU1701_INTEGRATION.md#3-stereo-adc-input-conditioning).

## Suggested completion sequence

1. Confirm the analog input source/connector and source impedance, interpret
   the combined maximum AC/DC input condition, and settle the supply, speaker
   loads/output target, DSP behavior, and user-control approach.
2. Finish the input conditioning and ADAU1701 schematic, including power,
   12.288 MHz master-clock source, serial clock configuration/loopbacks,
   boot/configuration, reset, ADC and DAC circuitry, and output drivers.
3. Wire the parent hierarchy consistently; run ERC and resolve or document
   every remaining warning.
4. Verify the USB Audio Class 1.0 asynchronous-sink and host-feedback approach,
   then implement and test the RP2350 PIO/DMA I²S path and audio buffering.
5. Update and verify the SigmaStudio project, then regenerate program artifacts
   from that source.
6. Build the PCB around the verified schematic, with deliberate grounding,
   decoupling, thermal management, and separation of low-level audio from
   switching power paths; run DRC.
7. Add simulation and bench checks for frequency response, clipping/headroom,
   noise, channel balance, mute/startup behavior, thermal performance, and
   fault handling, including switching between analog and USB sources.
8. Regenerate the schematic PDF and record the KiCad, SigmaStudio, and
   simulation tool versions used for the release.

## Editing and validation notes

- Open [`AudioAmplifier.kicad_pro`](../electronics/audio-amplifier/AudioAmplifier.kicad_pro)
  with KiCad 10 or a compatible version. The current files use KiCad 10 file
  formats.
- Keep custom symbols in
  [`Amplifier_Audio_custom.kicad_sym`](../electronics/audio-amplifier/Amplifier_Audio_custom.kicad_sym)
  and preserve the project-local library reference in `sym-lib-table`.
- Treat SigmaStudio-generated program/export files as outputs of the
  corresponding `.dspproj`; regenerate them after DSP changes rather than
  editing generated data by hand. Generated headers contain Analog Devices
  use and redistribution terms; review and preserve the supplied notices
  before copying or sharing those files.
- This repository currently has no automated hardware checks or build/test
  scripts. Record manual ERC, DRC, simulation, and bench-validation results
  when they become available.
