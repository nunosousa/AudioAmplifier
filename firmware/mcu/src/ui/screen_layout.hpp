#pragma once

#include <cstdint>

namespace audio_amplifier::ui::layout {

struct Rect {
  std::int16_t x;
  std::int16_t y;
  std::int16_t width;
  std::int16_t height;
};

inline constexpr std::int16_t kDisplayWidth = 320;
inline constexpr std::int16_t kDisplayHeight = 240;
inline constexpr std::int16_t kOuterMargin = 6;
inline constexpr std::int16_t kHeaderHeight = 20;
inline constexpr std::int16_t kFooterHeight = 24;

inline constexpr Rect kHeader{0, 0, kDisplayWidth, kHeaderHeight};
inline constexpr Rect kContent{kOuterMargin, 22, kDisplayWidth - (2 * kOuterMargin),
                               190};
inline constexpr Rect kFooter{0, 216, kDisplayWidth, kFooterHeight};

static_assert(kContent.y + kContent.height <= kFooter.y);

}  // namespace audio_amplifier::ui::layout
