# Firmware

- `mcu/` — the main controller application.
- `shared/` — reusable project code when it is shared by more than one target.
- `tools/` — host-side flashing, generation, or diagnostic utilities.

Do not commit compiler output, IDE caches, or generated build directories.

The current UI state-machine and renderer interfaces are in `mcu/src/ui/`.
They are platform-neutral C++ so the Pico SDK/display-driver integration can be
added after selecting the exact hardware.
