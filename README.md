# AudioAmplifier

An in-development hardware design for a stereo-plus-subwoofer (2.1) audio
amplifier. The design is centered on an ADAU1701 SigmaDSP, with TPA3116D2
Class-D power-amplifier stages.

> **Status: work in progress.** The current schematics describe an architectural
> transition to DSP-based processing; the DSP circuit is not yet wired or ready
> for fabrication. The PCB, simulation, and exported schematic PDF are also
> incomplete or out of date. See [Design status](docs/DESIGN_STATUS.md) before
> relying on any design files.

## Intended signal path

```text
Stereo line inputs
    → analog input conditioning
    → ADAU1701 ADCs
    → DSP volume, bass/treble EQ, and 2.1 crossover
    → three DAC channels and output conditioning
    → stereo and mono TPA3116D2 stages
    → left, right, and subwoofer outputs
```

The preamp is intended to provide input conditioning only. User-adjustable
volume, bass, treble, and crossover functions are intended to run in the DSP.
The current front-panel concept uses an RP2350, a 2.4-inch 320 × 240 colour
display, one main encoder, and dedicated parameter/display/mute buttons. The
display is a context-sensitive instrumentation UI, not a menu system; see the
[front-panel specification](docs/FRONT_PANEL.md) and
[UI firmware architecture](docs/UI_FIRMWARE_ARCHITECTURE.md).

## Open the design

Open [`electronics/audio-amplifier/AudioAmplifier.kicad_pro`](electronics/audio-amplifier/AudioAmplifier.kicad_pro)
in **KiCad 10 or a compatible version**. The project includes a project-local
symbol library table; its custom amplifier symbols are in
[`Amplifier_Audio_custom.kicad_sym`](electronics/audio-amplifier/Amplifier_Audio_custom.kicad_sym).

The SigmaStudio project and its generated artifacts are in
[`electronics/audio-amplifier/dsp/`](electronics/audio-amplifier/dsp/). The
LTspice audio-path file is in
[`electronics/audio-amplifier/simulation/LTspice/`](electronics/audio-amplifier/simulation/LTspice/).
Neither folder currently provides a verified end-to-end validation of the
latest schematic.

## Repository map

| Path | Contents |
| --- | --- |
| [`electronics/audio-amplifier/`](electronics/audio-amplifier/) | Main 2.1 KiCad project |
| `AudioAmplifier.kicad_sch` | Top-level sheet hierarchy and signal interfaces |
| `dsp.kicad_sch` | ADAU1701 symbol and proposed DSP I/O sheet |
| `preamplifier.kicad_sch` | Analog input-stage design, intended to be reduced to input conditioning |
| `stereo_amp.kicad_sch`, `mono_amp.kicad_sch` | TPA3116D2 power-amplifier stages |
| `power.kicad_sch`, `control.kicad_sch` | Power and amplifier mute/control circuits |
| `crossover.kicad_sch` | Standalone analog crossover design retained as a reference; not in the current top-level hierarchy |
| `headphone_amp.kicad_sch` | Retained sheet not in the current top-level hierarchy |
| `dsp/` | ADAU1701 SigmaStudio project, compiler outputs, and generated program data |
| `simulation/` | Filter-design spreadsheet and LTspice files |
| [`electronics/datasheets/`](electronics/datasheets/) | Component reference material |
| [`mechanical/enclosure/`](mechanical/enclosure/) | Enclosure CAD source, exchange exports, and drawings |
| [`firmware/`](firmware/) | MCU firmware, shared code, and development tools |

## Current validation

- **KiCad ERC/DRC:** not yet run on the current design.
- **PCB:** currently reset for redesign; it has no placed footprints or routed copper.
- **LTspice:** `Audio_path.asc` currently contains only the LTspice file header.
- **DSP:** committed SigmaStudio outputs exist, but have not been verified against
  the current DSP schematic and intended signal path.
- **PDF:** `AudioAmplifier.pdf` predates the latest schematic changes; treat it
  as stale until regenerated.

See [docs/DESIGN_STATUS.md](docs/DESIGN_STATUS.md) for known gaps, open
engineering decisions, and a suggested completion sequence.
For a pin-by-pin checklist of the DSP hardware to complete, see
[docs/ADAU1701_INTEGRATION.md](docs/ADAU1701_INTEGRATION.md).
