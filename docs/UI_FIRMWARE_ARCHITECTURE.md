# RP2350 UI firmware architecture

This is the implementation contract for the 320 × 240 front-panel UI. It keeps
input handling, context/state, display rendering, persistence, and DSP
communication independent so a display-driver choice does not alter interaction
behaviour.

## Module boundaries

```text
panel inputs ──> input decoder ──> UI controller ──> view model ──> renderer ──> display driver
                                         │                 ▲
                                         ▼                 │
                                     DSP bridge ───────────┘
                                         │
                                  persistent settings
```

| Module | Responsibility | Must not own |
| --- | --- | --- |
| Input decoder | Debounce buttons; decode encoder direction/speed and long presses | DSP values or pixels |
| UI controller | Select active parameter; apply the idle timeout; select playback mode; issue commands | Display-bus details |
| DSP bridge | Send parameter/mute commands and receive status, values, and telemetry | UI timing or drawing |
| View model | Convert acknowledged DSP state and UI context into screen data | GPIO or transport details |
| Renderer | Draw reusable header, footer, panels, graphs, bars, meters, and text | Input and DSP transport |
| Persistence | Store playback visualisation and optional brightness setting | Live DSP parameters unless explicitly required later |

The starter interfaces are in `firmware/mcu/src/ui/`. They deliberately have
no Pico SDK or display-library dependency, so the interaction logic can be
tested on a host and the eventual display transport can be selected separately.

## State and event rules

1. At boot, load the persisted playback mode and brightness preference, then
   begin in playback context with volume assigned to the encoder.
2. `BASS`, `TREBLE`, `SUB`, and `XOVER` select their parameter immediately,
   reset the 2.5-second timer, and request their adjustment screen.
3. An encoder turn resets the timer and generates a DSP set-parameter command
   for the currently assigned parameter. Use 0.5 dB steps for volume, bass,
   treble, and sub level. Let the DSP configuration provide the crossover
   range and slow/fast encoder step sizes.
4. `DISPLAY` updates and persists only the playback visualisation. It never
   changes the adjustment parameter or opens a menu.
5. `MUTE` sends its command immediately in every state and does not reset or
   otherwise alter the selected adjustment context.
6. After 2.5 seconds without an adjustment-button or encoder event, return to
   the saved playback visualisation and restore volume as the encoder target.
7. The renderer uses parameter and status telemetry acknowledged by the DSP.
   It may show a short pending indicator after a local request but must replace
   it with the returned DSP value.

## DSP bridge contract

The bridge needs a request/response protocol with sequence numbers. At a
minimum it must carry:

- Set volume, bass, treble, sub level, or crossover value.
- Toggle/set mute.
- DSP ready/fault state and current acknowledged parameter values.
- Left/right level data for VU meters.
- Spectrum bands and/or sufficient data to derive them, if spectrum mode is
  enabled.

Define value ranges, filter topology, crossover curve, and graph sample points
in the DSP configuration. This prevents the UI from graphing assumptions that
do not match the actual ADAU1701 program.

## Renderer components

`IRenderer` is intentionally small. Its implementation should compose reusable
components rather than encode a screen as one large drawing routine:

- `HeaderRenderer` and `FooterRenderer`
- `PanelRenderer` and bitmap `TextRenderer`
- `SegmentBarRenderer` and `NumericValueRenderer`
- `GraphRenderer`, including log-frequency axes and gridlines
- `SpectrumRenderer` and `VuMeterRenderer`

Draw only changed dynamic regions at a fixed, modest visual refresh rate.
Prioritise control response and audio/DSP communication over display-frame
rate; stealth mode should avoid continuous drawing altogether.

## Bring-up sequence

1. Validate inputs, display reset/backlight, and a static header/footer.
2. Implement volume, mute, and DSP acknowledgement round-trip.
3. Add the four adjustment contexts and timeout behaviour.
4. Add persisted playback mode and reduced-brightness handling.
5. Add VU telemetry, then spectrum telemetry after rate/bandwidth validation.
6. Verify all fast digital/display activity for audible coupling, encoder
   debounce, DSP communication failure handling, and power-up mute behaviour.
