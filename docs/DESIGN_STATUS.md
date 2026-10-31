# Main 2.1 amplifier: design status

This document records the intended architecture and the known state of the
design files. It distinguishes stated goals from implemented and verified
hardware; schematic labels and existing generated artifacts are not proof that
the circuit is complete.

## Design intent

The main product is a stereo-plus-subwoofer amplifier. The intended signal
path is:

1. Accept left and right analog line-level inputs.
2. Condition and bias those signals for the ADAU1701 ADC inputs.
3. Perform master volume, bass and treble adjustment, and stereo-to-2.1
   crossover processing in the ADAU1701.
4. Produce left, right, and subwoofer audio outputs.
5. Condition/drive those signals into the differential input interfaces of
   the stereo and mono TPA3116D2 amplifier stages.

The standalone analog crossover is no longer the target implementation.
[`crossover.kicad_sch`](../ECAD/AudioAmplifier/crossover.kicad_sch) is retained
in the repository as a reference, but is not part of the current
`AudioAmplifier.kicad_sch` sheet hierarchy. The separate
[`ECAD/HeadphoneAmplifier/`](../ECAD/HeadphoneAmplifier/) project is outside
the current scope. The older `headphone_amp.kicad_sch` under the main project
directory is also not in the current top-level hierarchy.

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

The current front-panel proposal uses five rotary encoders (master, bass,
treble, sub level, crossover), a shared four-digit 7-segment display, and
parameter/status LEDs. The master control also acts as mute. An ARM MCU is
intended to read the controls and update the ADAU1701; STM32G0 is the current
candidate family, not a finalized part. See the [front-panel concept](./FRONT_PANEL.md).

See the [ADAU1701 integration plan](./ADAU1701_INTEGRATION.md) for a package
pin-by-pin connection checklist, proposed channel mapping, and bring-up checks.

## Blocking gaps

### DSP schematic and analog interfaces

[`dsp.kicad_sch`](../ECAD/AudioAmplifier/dsp.kicad_sch) is not a complete
circuit. The ADAU1701 symbol is not wired to the sheet's audio labels or to
power, clock, reset, boot/configuration, or required external components.
Complete and check the actual pin-level circuit before considering the
top-level DSP labels electrically implemented.

The top-level interface proposes three differential outputs, while the
ADAU1701 provides four DAC outputs. Determine the output-channel mapping and
design the required analog output stages to drive the TPA3116D2 differential
inputs. Do not assume that connecting DAC pins directly to amplifier inputs
will provide the required level, polarity, or common-mode behavior.

### DSP controls and program

The proposed interface needs an ARM MCU, but the
[`mcu.kicad_sch`](../ECAD/AudioAmplifier/mcu.kicad_sch) sheet is still empty.
The current candidate is STM32G0, with a J-Link EDU available for development.
Select an exact MCU/package, define the DSP control-port wiring and user-input
circuits, then add hardware and firmware. See the [front-panel concept](./FRONT_PANEL.md).

The reported SigmaStudio processing chain is a starting point, not verified
evidence that the committed program artifacts match the current project. Its
committed compiler report describes a 48 kHz build using 232 of 1024
instructions, 124 of 2048 data-RAM units, and 752 of 1024 parameter-RAM units.
Verify the project routing, filter response, gain, parameter limits, and
artifact freshness, then regenerate and identify the corresponding program
artifacts together.

### PCB, simulation, and exports

- [`AudioAmplifier.kicad_pcb`](../ECAD/AudioAmplifier/AudioAmplifier.kicad_pcb)
  currently has no placed footprints. Rebuild the board from the updated
  schematic; no previous placement or routing should be assumed to remain.
- [`Audio_path.asc`](../ECAD/AudioAmplifier/Circuit%20simulation/LTspice/Audio_path.asc)
  is only an LTspice header. Create a simulation that exercises the relevant
  input conditioning, DSP/output-stage assumptions, and amplifier input
  interfaces where models permit.
- [`AudioAmplifier.pdf`](../ECAD/AudioAmplifier/AudioAmplifier.pdf) predates
  the latest schematic. Regenerate it after the schematic stabilizes.
- KiCad ERC and PCB DRC have not been run on the current design.

## Decisions to make

Resolve these with the intended input sources, speakers, enclosure, controls,
and supply before finalizing the circuit or PCB:

| Topic | Decision / verification needed |
| --- | --- |
| Input range | Minimum, nominal, and maximum input voltage; source impedance; connector; protection requirements |
| ADC interface | ADAU1701 ADC bias/common-mode, input impedance, filtering, and clipping margin |
| Output interface | DAC channel assignment, output level, differential-driver topology, filtering, and amplifier input compatibility |
| DSP behavior | Crossover frequency and response, channel summing, EQ range, boost/headroom policy, and volume range |
| User controls | Five rotary encoders and a shared 7-segment display with parameter/status LEDs are proposed; exact MCU and implementation remain open |
| Volume behavior | One shared master volume for all outputs, plus a separate subwoofer level trim |
| Startup and faults | DSP boot/configuration method, amplifier mute/enable sequencing, fault response, and pop suppression |
| MCU | ARM MCU intended; STM32G0 is the current candidate family, with exact part and firmware still open |
| Power | Input supply range/current, analog/DSP rails, decoupling, grounding, and separation from Class-D switching currents |
| Amplifier use | Speaker impedance, target output power, thermal limits, heatsinking, and protection behavior |

The older preamp schematic contains an input note describing nominal 0.5 Vrms,
minimum 0.2 Vrms, maximum 5.65 Vpp, and a possible DC component up to +1 V.
Treat these as inherited notes, not approved requirements; confirm the source
specification and check worst-case voltage at the ADAU1701 ADC.

## Suggested completion sequence

1. Agree on the input range, supply, speaker loads/output target, DSP behavior,
   and user-control approach.
2. Finish the input conditioning and ADAU1701 schematic, including power,
   clock, boot/configuration, reset, ADC and DAC circuitry, and output drivers.
3. Wire the parent hierarchy consistently; run ERC and resolve or document
   every remaining warning.
4. Update and verify the SigmaStudio project, then regenerate program artifacts
   from that source.
5. Build the PCB around the verified schematic, with deliberate grounding,
   decoupling, thermal management, and separation of low-level audio from
   switching power paths; run DRC.
6. Add simulation and bench checks for frequency response, clipping/headroom,
   noise, channel balance, mute/startup behavior, thermal performance, and
   fault handling.
7. Regenerate the schematic PDF and record the KiCad, SigmaStudio, and
   simulation tool versions used for the release.

## Editing and validation notes

- Open [`AudioAmplifier.kicad_pro`](../ECAD/AudioAmplifier/AudioAmplifier.kicad_pro)
  with KiCad 10 or a compatible version. The current files use KiCad 10 file
  formats.
- Keep custom symbols in
  [`Amplifier_Audio_custom.kicad_sym`](../ECAD/AudioAmplifier/Amplifier_Audio_custom.kicad_sym)
  and preserve the project-local library reference in `sym-lib-table`.
- Treat SigmaStudio-generated program/export files as outputs of the
  corresponding `.dspproj`; regenerate them after DSP changes rather than
  editing generated data by hand. Generated headers contain Analog Devices
  use and redistribution terms; review and preserve the supplied notices
  before copying or sharing those files.
- This repository currently has no automated hardware checks or build/test
  scripts. Record manual ERC, DRC, simulation, and bench-validation results
  when they become available.
