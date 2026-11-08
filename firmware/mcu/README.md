# RP2350 controller firmware

`src/ui/` contains the platform-neutral front-panel state machine, view model,
and renderer boundary. It implements the immediate, no-menu interaction model
while leaving GPIO, display transport, DSP protocol, persistence storage, and
Pico SDK startup code as hardware adapters.

Before creating the RP2350 build project, select the exact display controller
and interface, define the DSP parameter/telemetry protocol, and record the
MCU package, pin assignment, and board power/debug design.
