#pragma once
#include <cstddef>
#include "game/util/resource.hpp"
#include "util/file/file.hpp"
#include "util/math/num-types.hpp"

// данные аудио файла
struct Audio: public Resource {
  // чем сжаты данные
  enum class Compression {
    raw, // без сжатия
    opus,
    flac,
    vorbis
  };
  enum class Format {
    pcm_f32,
    raw_pcm_s16,
    raw_pcm_u8,
  };
  Compression compression {};
  Format format {};
  uint channels {}; // число каналов звука
  uint samples {}; // сколько аудио выборок (без учёта стерео)
  uint frequency {}; // частота звука
  Bytes data {};
};

// через этот идентификатор можно управлять воспроизведением звука
using Audio_id = std::uintptr_t;
inline constexpr Audio_id BAD_AUDIO = 0;
