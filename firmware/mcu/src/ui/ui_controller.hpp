#pragma once

#include "ui_types.hpp"

namespace audio_amplifier::ui {

class IDspCommandSink {
 public:
  virtual ~IDspCommandSink() = default;
  virtual void submit(const DspCommand& command) = 0;
};

class IUiPreferences {
 public:
  virtual ~IUiPreferences() = default;
  virtual PlaybackMode loadPlaybackMode() const = 0;
  virtual bool loadReducedBrightness() const = 0;
  virtual void savePlaybackMode(PlaybackMode mode) = 0;
  virtual void saveReducedBrightness(bool enabled) = 0;
};

// Pure interaction state machine. GPIO, debouncing, DSP transport, timers, and
// display drawing live behind the interfaces surrounding this class.
class UiController {
 public:
  UiController(const UiConfiguration& configuration, IDspCommandSink& dsp,
               IUiPreferences& preferences);

  void restorePreferences();
  void onButton(Button button, bool longPress, Millis nowMs);
  void onEncoderTurn(const EncoderTurn& turn, Millis nowMs);
  void onDspState(const DspState& state);
  void tick(Millis nowMs);

  const UiState& state() const { return state_; }

 private:
  void selectParameter(Parameter parameter, Millis nowMs);
  void setRequestedParameter(float requestedValue);
  void returnToPlayback();
  bool adjustmentTimedOut(Millis nowMs) const;

  const UiConfiguration& configuration_;
  IDspCommandSink& dsp_;
  IUiPreferences& preferences_;
  UiState state_{};
  std::uint32_t nextSequence_ = 0;
};

}  // namespace audio_amplifier::ui
