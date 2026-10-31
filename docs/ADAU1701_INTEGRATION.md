# ADAU1701 integration plan

This is a schematic-preparation guide for integrating the ADAU1701 into the
main 2.1 amplifier. It is not a finished schematic or a substitute for the
current Analog Devices datasheet and SigmaStudio documentation.

The package pin names/numbers below are transcribed from the embedded KiCad
symbol in [`dsp.kicad_sch`](../ECAD/AudioAmplifier/dsp.kicad_sch). Confirm every
pin function, electrical limit, recommended connection, component value, and
layout rule against the official
[ADAU1701 datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/ADAU1701.pdf)
before editing the circuit or releasing a board. Datasheet retrieval was not
available while preparing this plan, so no unverified component values are
specified here.

## Intended audio-channel mapping

The top-level sheet currently proposes these signal interfaces:

| Audio channel | Proposed DSP DAC | DSP-sheet outputs | Destination |
| --- | --- | --- | --- |
| Left | VOUT0 (pin 46) | `OUT_HIGH_P_LEFT`, `OUT_HIGH_N_LEFT` | Left differential input of stereo amplifier |
| Right | VOUT1 (pin 45) | `OUT_HIGH_P_RIGHT`, `OUT_HIGH_N_RIGHT` | Right differential input of stereo amplifier |
| Subwoofer | VOUT2 (pin 44) | `OUT_LOW_P`, `OUT_LOW_N` | Differential input of mono amplifier |
| Spare | VOUT3 (pin 43) | Not assigned | Reserve until channel needs are confirmed |

This is a proposed allocation, not a confirmed SigmaStudio routing. Verify it
against the DSP program before relying on the labels. The ADAU1701 has four
single-ended DAC outputs, whereas the three TPA3116D2 channels are presented
with differential input pairs. Therefore, the three proposed output pairs
require suitable analog output-driver circuits; the positive and negative
inputs must not be assumed to be separate DAC outputs or connected directly
to ground.

## Current SigmaStudio processing

The project owner reports that the current SigmaStudio design contains:

1. Bass and treble adjustment on the stereo input path. Each uses a DC Input
   Entry parameter to control a second-order filter block.
2. One DC Input Entry parameter controlling the cutoff frequency for the
   crossover filters: paired second-order high-pass sections for the stereo
   outputs (fourth-order overall) and paired second-order low-pass sections
   for the subwoofer output (fourth-order overall).
3. Separate master-volume blocks for the stereo outputs and the subwoofer
   output.

These are functional processing blocks in the SigmaStudio project; the
hardware/control path that will set their parameters is still undecided. A
SigmaStudio DC Input Entry value does not, by itself, define a physical knob,
controller, user interface, or persistent setting.

Before calling the crossover Linkwitz–Riley, inspect its actual biquad
coefficients and response. Cascading two identical second-order Butterworth
sections at the same cutoff yields a fourth-order Linkwitz–Riley response
(nominally -6 dB per branch at crossover); a generic fourth-order Butterworth
filter has different section Q values and a different crossover sum. Confirm
that the high- and low-pass branches use the intended response, share the same
cutoff parameter, and sum correctly in magnitude and phase with the intended
speaker polarity and acoustic alignment.

The cutoff control should update all affected stereo high-pass and mono
low-pass coefficients together. Check the SigmaStudio implementation to ensure
the coefficient changes remain stable throughout the allowed range and do not
cause audible zipper noise; use suitable parameter smoothing or a controlled
update if needed. Limit the adjustment range to values supported by the
filters, sample rate, speakers, and output-stage headroom.

The owner intends one shared master volume for all outputs plus a separate
subwoofer level trim. Map the stereo and subwoofer volume blocks to that
behavior (the stereo block provides master volume; the subwoofer block
combines master volume with the additional subwoofer trim, or an equivalent
DSP arrangement). Verify mute behavior, startup gain, parameter limits, and
clipping/headroom across all outputs.

## Pin-by-pin connection checklist

“Required treatment” describes what the schematic must account for. It does
not prescribe a final strap, rail, or component value.

| Pin | Name | Planned treatment | Status in current DSP sheet |
| ---: | --- | --- | --- |
| 1 | AGND | Connect to the analog ground reference per the vendor layout guidance. | Open |
| 2 | ADC0 | Condition and connect one line-input channel; verify common-mode, range, and anti-alias filtering. | Open |
| 3 | ADC_RES | Fit the exact reference/resistor connection specified by the datasheet. | Open |
| 4 | ADC1 | Condition and connect the other line-input channel; verify common-mode, range, and anti-alias filtering. | Open |
| 5 | `~{RESET}` | Define reset state and release timing; choose pull-up/reset supervision as needed. | Open |
| 6 | SELFBOOT | Strap for the selected startup method; document whether external EEPROM self-boot is used. | Open |
| 7 | ADDR0 | Strap/select only as required by the chosen control-bus configuration. | Open |
| 8 | MP4 | Assign a documented GPIO/serial function or set a defined unused state. | Open |
| 9 | MP5 | Assign a documented GPIO/serial function or set a defined unused state. | Open |
| 10 | MP1 | Assign a documented GPIO/serial function or set a defined unused state. | Open |
| 11 | MP0 | Assign a documented GPIO/serial function or set a defined unused state. | Open |
| 12 | DGND | Connect to the ground reference per the vendor layout guidance. | Open |
| 13 | DVDD | Connect to the datasheet-specified digital supply and local decoupling. | Open |
| 14 | MP7 | Assign a documented GPIO/serial function or set a defined unused state. | Open |
| 15 | MP6 | Assign a documented GPIO/serial function or set a defined unused state. | Open |
| 16 | MP10 | Assign a documented GPIO/serial function or set a defined unused state. | Open |
| 17 | VDRIVE | Use only as specified for the supported interface voltage; do not treat as a general-purpose supply. | Open |
| 18 | IOVDD | Connect to the datasheet-specified I/O supply and local decoupling. | Open |
| 19 | MP11 | Assign a documented GPIO/serial function or set a defined unused state. | Open |
| 20 | ADDR1/CDATA/WB | Configure for the selected control/self-boot interface; make the strap and bus function explicit. | Open |
| 21 | CLATCH/WP | Connect/configure for the selected control/self-boot interface. | Open |
| 22 | SDA/COUT | Connect/configure for the selected control/self-boot interface; add required pull-ups if applicable. | Open |
| 23 | SCL/CCLK | Connect/configure for the selected control/self-boot interface; add required pull-ups if applicable. | Open |
| 24 | DVDD | Connect to the specified digital supply and local decoupling. | Open |
| 25 | DGND | Connect to the ground reference per the vendor layout guidance. | Open |
| 26 | MP9 | Assign a documented GPIO/serial function or set a defined unused state. | Open |
| 27 | MP8 | Assign a documented GPIO/serial function or set a defined unused state. | Open |
| 28 | MP3 | Assign a documented GPIO/serial function or set a defined unused state. | Open |
| 29 | MP2 | Assign a documented GPIO/serial function or set a defined unused state. | Open |
| 30 | RSVD | Apply the exact reserved-pin treatment in the datasheet; do not infer a connection from the symbol’s passive type. | Open |
| 31 | OSCO | Connect only as required by the selected crystal/clock topology. | Open |
| 32 | MCLKI | Provide the master clock using a datasheet-compliant source and layout. | Open |
| 33 | PGND | Connect to the specified ground reference with the vendor-recommended layout. | Open |
| 34 | PVDD | Connect to the specified supply and local decoupling. | Open |
| 35 | PLL_LF | Add the specified loop-filter network with vendor-recommended placement. | Open |
| 36 | AVDD | Connect to the specified analog supply and local decoupling. | Open |
| 37 | AGND | Connect to the analog ground reference per the vendor layout guidance. | Open |
| 38 | PLL_MODE0 | Strap for the selected master-clock/PLL operating mode. | Open |
| 39 | PLL_MODE1 | Strap for the selected master-clock/PLL operating mode. | Open |
| 40 | CM | Add the specified common-mode bypass/filter connection. | Open |
| 41 | FILTD | Add the specified DAC filter connection and place it close to the IC. | Open |
| 42 | AGND | Connect to the analog ground reference per the vendor layout guidance. | Open |
| 43 | VOUT3 | Reserve as the spare DAC unless the channel plan changes; provide the specified output network if used. | Open |
| 44 | VOUT2 | Proposed subwoofer DAC; add its output network and differential driver. | Open |
| 45 | VOUT1 | Proposed right-channel DAC; add its output network and differential driver. | Open |
| 46 | VOUT0 | Proposed left-channel DAC; add its output network and differential driver. | Open |
| 47 | FILTA | Add the specified ADC filter connection and place it close to the IC. | Open |
| 48 | AVDD | Connect to the specified analog supply and local decoupling. | Open |

The repeated supply and ground pins are intentional: connect every package pin.
Use the datasheet's recommended decoupling, ground, and placement scheme rather
than treating the repeated pins as optional.

## Circuit blocks to draw

### 1. Power and reference

- Choose the system input supply and derive every ADAU1701 rail from the
  datasheet limits and the rest of the design.
- Show each supply domain separately (`AVDD`, `DVDD`, `PVDD`, `IOVDD`) even
  where they ultimately share a source. Identify any filtering or ferrite
  isolation, and place decoupling at the corresponding device pins.
- Connect all ground pins. Plan a continuous, low-impedance reference and
  partition noisy Class-D return-current paths by placement/routing rather
  than introducing an unreviewed ground split.
- Add the ADC reference, PLL loop filter, DAC/ADC filter capacitors, and
  common-mode bypass exactly as specified by the current datasheet.

**Still needed:** supply voltage/current budget, rail architecture, regulator
choices, and datasheet-verified component values.

### 2. Clock, reset, boot, and control

- Select either a crystal/resonator or external master-clock source and define
  its frequency and startup behavior. Configure both PLL mode pins accordingly.
- Choose and document the boot path: for example, self-boot from an external
  EEPROM or program/configure the DSP from a controller. Check that the selected
  strap states and pin multiplexing support the chosen path.
- Define the roles of the multifunction pins before assigning GPIOs or serial
  audio ports. Give every unused input or multifunction pin a documented,
  datasheet-compliant state.
- The current UI proposal uses an ARM MCU to read five rotary encoders and
  update master volume, bass, treble, sub trim, and crossover parameters. An
  seven-segment display shows the adjusted value during press-and-turn
  interaction, with separate status LEDs. RP2040 and STM32G0 remain MCU
  candidates, but the simplified display does not need an audio-data path from
  the DSP. Compare candidates against display driving, parameter control, and
  GPIO needs. A J-Link EDU is available; reserve SWD access, including reset
  as appropriate, and verify target-voltage and probe requirements.
- Specify control-bus wiring, pull-ups/logic voltage, address straps, reset
  ownership, DSP parameter-update mechanism, and relationship to amplifier
  mute and power control.

**Still needed:** boot method, exact MCU, master-clock source, and confirmed
parameter interface.

### 3. Stereo ADC input conditioning

- Preserve left/right channel identity from the input connector through the
  preamp to ADC0/ADC1 and into the SigmaStudio project.
- Define input protection, impedance, AC/DC coupling, bias/reference, and
  filtering from the intended source and the ADAU1701 ADC circuit
  recommendations.
- Check the worst-case instantaneous input—including the source's DC offset,
  maximum level, component tolerance, and any preamp gain—against the ADC
  limits. Keep normal input level well below clipping.
- Document expected levels at the connector, preamp output, and ADC pin.

The previous preamp note (0.2 Vrms minimum, 0.5 Vrms nominal, 5.65 Vpp maximum,
and up to +1 V DC component) is inherited, not an accepted specification.

### 4. Three DAC output-driver channels

For each of left, right, and subwoofer:

1. Assign one DAC channel and record the matching SigmaStudio output.
2. Add the datasheet-recommended DAC output network.
3. Design a driver that provides the required differential signal to one
   TPA3116D2 input pair, including correct amplitude, polarity, bias/common
   mode, and source impedance.
4. Verify the amplifier input limits and gain together with DSP output level
   and any intended equalization boost.
5. Add mute/startup behavior that avoids audible transients.

Do not treat an output pair named `P`/`N` in the hierarchy as a differential
output of the ADAU1701 itself: it is the proposed output of the external analog
driver. Keep a clear spare-channel policy for VOUT3.

**Still needed:** intended amplifier gain, required DAC-to-amplifier level,
driver topology, component availability, and whether a shared or per-channel
mute is required.

## Requirements to settle before final values

- Source type, connector, source impedance, minimum/nominal/maximum input level,
  and whether input DC offset is possible.
- Required supply input, available power, speaker impedance, and target output
  power per channel.
- Crossover frequency and response, subwoofer summing rule, EQ range/boost,
  and master-volume range.
- Front-panel implementation details (press-and-turn behavior, display
  format/driver, parameter ranges, startup defaults, and saved-setting policy).
- Whether the design needs selectable inputs, status indication, fault
  reporting, or firmware-updatable DSP parameters.

## Bring-up and validation checklist

- [ ] Review each of the 48 package pins against the current datasheet.
- [ ] Run KiCad ERC; resolve errors and document intentional exceptions.
- [ ] Generate the KiCad netlist and confirm ADC, DAC, supply, clock, reset,
      control-bus, and mute nets.
- [ ] Verify the SigmaStudio design has the same ADC/DAC channel mapping and
      startup parameter values as the schematic.
- [ ] Test input clipping margin and output levels at minimum/nominal/maximum
      input.
- [ ] Measure left/right/sub frequency response, crossover summing, phase,
      channel balance, noise, and mute/startup transients.
- [ ] Check the DSP and output-driver supplies for noise under Class-D load.

## Current repository-specific findings

- The parent hierarchy currently carries `IN_LEFT` and `IN_RIGHT` into the DSP
  sheet from the reduced preamp, and carries six proposed differential outputs
  toward the stereo and mono TPA3116D2 sheets.
- The DSP symbol is placed, but the DSP sheet has no wires or power symbols;
  its hierarchical labels are not connected to the chip pins.
- The owner-reported SigmaStudio chain includes bass and treble filters, a
  shared adjustable crossover cutoff, and separate stereo/subwoofer volume
  blocks. The intended control behavior is one shared master volume plus a
  subwoofer trim. Verify that the actual SigmaStudio graph implements that
  behavior, and check the generated files together before hardware bring-up.
- The parent `MCU` sheet is empty. The controller need is therefore an
  architecture decision, not an implemented feature.
- The current PCB is empty and needs to be rebuilt after the DSP sheet and its
  analog interface are complete.
