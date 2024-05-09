#pragma once
#include <cstddef>
#include "game/util/resource.hpp"
#include "util/file/file.hpp"
#include "util/math/num-types.hpp"

// данные аудио файла
struct Audio: public Resource {
  // чем сжаты данные
  enum class Compression {
    raw_pcm_f32,
    raw_pcm_s16,
    raw_pcm_u8,
    opus,
    flac,
    vorbis
  } compression {};
  uint channels {}; // число каналов звука
  Bytes data {};
};

// через этот идентификатор можно управлять воспроизведением звука
using Audio_id = std::uintptr_t;
inline constexpr Audio_id BAD_AUDIO = 0;
