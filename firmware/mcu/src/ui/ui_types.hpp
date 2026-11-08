#pragma once

#include <cstdint>

namespace audio_amplifier::ui {

using Millis = std::uint32_t;

constexpr Millis kAdjustmentTimeoutMs = 2500;

enum class PlaybackMode : std::uint8_t {
  Spectrum,
  StereoVu,
  Stealth,
};

enum class ScreenContext : std::uint8_t {
  Playback,
  Adjustment,
};

enum class Parameter : std::uint8_t {
  Volume,
  Bass,
  Treble,
  SubLevel,
  Crossover,
};

enum class Button : std::uint8_t {
  Bass,
  Treble,
  Sub,
  Crossover,
  Display,
  Mute,
};

enum class EncoderSpeed : std::uint8_t {
  Slow,
  Fast,
};

struct ParameterValues {
  float volumeDb = 0.0F;
  float bassDb = 0.0F;
  float trebleDb = 0.0F;
  float subLevelDb = 0.0F;
  float crossoverHz = 0.0F;
};

// Limits and increments are supplied by the validated DSP configuration.
// All dB controls use 0.5 dB increments; crossover can use a faster step.
struct ParameterLimits {
  float minimum = 0.0F;
  float maximum = 0.0F;
  float slowStep = 0.5F;
  float fastStep = 0.5F;
};

struct UiConfiguration {
  ParameterLimits volume;
  ParameterLimits bass;
  ParameterLimits treble;
  ParameterLimits subLevel;
  ParameterLimits crossover;
};

// Values in this snapshot are acknowledged by the DSP and are the values the
// renderer displays. Level/spectrum telemetry is added by the DSP bridge.
struct DspState {
  ParameterValues parameters{};
  bool ready = false;
  bool muted = true;
  bool fault = false;
  std::uint32_t lastAppliedSequence = 0;
};

struct DspCommand {
  enum class Type : std::uint8_t {
    SetParameter,
    ToggleMute,
  };

  Type type = Type::SetParameter;
  Parameter parameter = Parameter::Volume;
  float value = 0.0F;
  std::uint32_t sequence = 0;
};

struct EncoderTurn {
  int detents = 0;
  EncoderSpeed speed = EncoderSpeed::Slow;
};

struct UiState {
  PlaybackMode playbackMode = PlaybackMode::Spectrum;
  ScreenContext screenContext = ScreenContext::Playback;
  Parameter encoderParameter = Parameter::Volume;
  DspState dsp{};
  bool reducedBrightness = false;
  bool parameterUpdatePending = false;
  float requestedValue = 0.0F;
  std::uint32_t pendingSequence = 0;
  Millis lastAdjustmentMs = 0;
};

}  // namespace audio_amplifier::ui
