#include "ui_controller.hpp"

namespace audio_amplifier::ui {
namespace {

const ParameterLimits& limitsFor(const UiConfiguration& configuration,
                                 Parameter parameter) {
  switch (parameter) {
    case Parameter::Volume:
      return configuration.volume;
    case Parameter::Bass:
      return configuration.bass;
    case Parameter::Treble:
      return configuration.treble;
    case Parameter::SubLevel:
      return configuration.subLevel;
    case Parameter::Crossover:
      return configuration.crossover;
  }

  return configuration.volume;
}

float valueFor(const ParameterValues& values, Parameter parameter) {
  switch (parameter) {
    case Parameter::Volume:
      return values.volumeDb;
    case Parameter::Bass:
      return values.bassDb;
    case Parameter::Treble:
      return values.trebleDb;
    case Parameter::SubLevel:
      return values.subLevelDb;
    case Parameter::Crossover:
      return values.crossoverHz;
  }

  return values.volumeDb;
}

float clamp(float value, const ParameterLimits& limits) {
  if (value < limits.minimum) {
    return limits.minimum;
  }
  if (value > limits.maximum) {
    return limits.maximum;
  }
  return value;
}

PlaybackMode nextPlaybackMode(PlaybackMode mode) {
  switch (mode) {
    case PlaybackMode::Spectrum:
      return PlaybackMode::StereoVu;
    case PlaybackMode::StereoVu:
      return PlaybackMode::Stealth;
    case PlaybackMode::Stealth:
      return PlaybackMode::Spectrum;
  }

  return PlaybackMode::Spectrum;
}

}  // namespace

UiController::UiController(const UiConfiguration& configuration,
                           IDspCommandSink& dsp, IUiPreferences& preferences)
    : configuration_(configuration), dsp_(dsp), preferences_(preferences) {}

void UiController::restorePreferences() {
  state_.playbackMode = preferences_.loadPlaybackMode();
  state_.reducedBrightness = preferences_.loadReducedBrightness();
}

void UiController::onButton(Button button, bool longPress, Millis nowMs) {
  switch (button) {
    case Button::Bass:
      selectParameter(Parameter::Bass, nowMs);
      return;
    case Button::Treble:
      selectParameter(Parameter::Treble, nowMs);
      return;
    case Button::Sub:
      selectParameter(Parameter::SubLevel, nowMs);
      return;
    case Button::Crossover:
      selectParameter(Parameter::Crossover, nowMs);
      return;
    case Button::Display:
      if (longPress) {
        state_.reducedBrightness = !state_.reducedBrightness;
        preferences_.saveReducedBrightness(state_.reducedBrightness);
      } else {
        state_.playbackMode = nextPlaybackMode(state_.playbackMode);
        preferences_.savePlaybackMode(state_.playbackMode);
      }
      return;
    case Button::Mute:
      // Mute is deliberately independent of screen context and timeout state.
      dsp_.submit({DspCommand::Type::ToggleMute, Parameter::Volume, 0.0F,
                   ++nextSequence_});
      return;
  }
}

void UiController::onEncoderTurn(const EncoderTurn& turn, Millis nowMs) {
  if (turn.detents == 0) {
    return;
  }

  tick(nowMs);
  const ParameterLimits& limits =
      limitsFor(configuration_, state_.encoderParameter);
  const float step = turn.speed == EncoderSpeed::Fast ? limits.fastStep
                                                       : limits.slowStep;
  const float current = valueFor(state_.dsp.parameters, state_.encoderParameter);
  setRequestedParameter(clamp(current + (step * turn.detents), limits));

  state_.screenContext = ScreenContext::Adjustment;
  state_.lastAdjustmentMs = nowMs;
}

void UiController::onDspState(const DspState& state) {
  state_.dsp = state;
  // The DSP acknowledgement is authoritative for the value shown on-screen.
  if (state_.parameterUpdatePending &&
      state.lastAppliedSequence >= state_.pendingSequence) {
    state_.parameterUpdatePending = false;
  }
}

void UiController::tick(Millis nowMs) {
  if (adjustmentTimedOut(nowMs)) {
    returnToPlayback();
  }
}

void UiController::selectParameter(Parameter parameter, Millis nowMs) {
  state_.encoderParameter = parameter;
  state_.screenContext = ScreenContext::Adjustment;
  state_.lastAdjustmentMs = nowMs;
}

void UiController::setRequestedParameter(float requestedValue) {
  state_.requestedValue = requestedValue;
  state_.parameterUpdatePending = true;
  state_.pendingSequence = ++nextSequence_;
  dsp_.submit({DspCommand::Type::SetParameter, state_.encoderParameter,
               requestedValue, state_.pendingSequence});
}

void UiController::returnToPlayback() {
  state_.screenContext = ScreenContext::Playback;
  state_.encoderParameter = Parameter::Volume;
  state_.parameterUpdatePending = false;
  state_.pendingSequence = 0;
}

bool UiController::adjustmentTimedOut(Millis nowMs) const {
  return state_.screenContext == ScreenContext::Adjustment &&
         static_cast<Millis>(nowMs - state_.lastAdjustmentMs) >=
             kAdjustmentTimeoutMs;
}

}  // namespace audio_amplifier::ui
