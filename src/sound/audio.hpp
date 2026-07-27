#pragma once
#include <cstddef>
#include "game/util/resource.hpp"
#include "util/file/file.hpp"
#include "util/math/num-types.hpp"

// данные аудио файла
struct Audio: public Resource {
  // чем сжаты данные
  enum class Compression {
    raw,    // без сжатия
    opus,   // libopus
    mp3,    // dr_mp3
    flac,   // dr_flac
    vorbis, // stb_vorbis
  };

  // формат звука
  enum class Format {
    pcm_f32,     // fltp audio
    raw_pcm_s16, // signed 16 bit audio
    raw_pcm_u8,  // unsigned 8 bit audio
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
