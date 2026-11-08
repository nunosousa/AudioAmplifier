#pragma once

#include "ui_types.hpp"

namespace audio_amplifier::ui {

enum class ScreenKind : std::uint8_t {
  Spectrum,
  StereoVu,
  Stealth,
  Volume,
  Tone,
  SubLevel,
  Crossover,
};

struct ScreenView {
  ScreenKind kind = ScreenKind::Spectrum;
  Parameter activeParameter = Parameter::Volume;
  DspState dsp{};
  bool parameterUpdatePending = false;
  float requestedValue = 0.0F;
};

// The platform adapter turns this view into pixels. Implement it by composing
// reusable header/footer, panel, graph, meter, numeric-value, and bar renderers.
class IRenderer {
 public:
  virtual ~IRenderer() = default;
  virtual void render(const ScreenView& view) = 0;
  virtual void setReducedBrightness(bool enabled) = 0;
};

ScreenView makeScreenView(const UiState& state);

}  // namespace audio_amplifier::ui
