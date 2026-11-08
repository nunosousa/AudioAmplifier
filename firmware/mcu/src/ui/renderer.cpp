#include "renderer.hpp"

namespace audio_amplifier::ui {

ScreenView makeScreenView(const UiState& state) {
  ScreenView view{};
  view.activeParameter = state.encoderParameter;
  view.dsp = state.dsp;
  view.parameterUpdatePending = state.parameterUpdatePending;
  view.requestedValue = state.requestedValue;

  if (state.screenContext == ScreenContext::Playback) {
    switch (state.playbackMode) {
      case PlaybackMode::Spectrum:
        view.kind = ScreenKind::Spectrum;
        return view;
      case PlaybackMode::StereoVu:
        view.kind = ScreenKind::StereoVu;
        return view;
      case PlaybackMode::Stealth:
        view.kind = ScreenKind::Stealth;
        return view;
    }
  }

  switch (state.encoderParameter) {
    case Parameter::Volume:
      view.kind = ScreenKind::Volume;
      break;
    case Parameter::Bass:
    case Parameter::Treble:
      view.kind = ScreenKind::Tone;
      break;
    case Parameter::SubLevel:
      view.kind = ScreenKind::SubLevel;
      break;
    case Parameter::Crossover:
      view.kind = ScreenKind::Crossover;
      break;
  }

  return view;
}

}  // namespace audio_amplifier::ui
