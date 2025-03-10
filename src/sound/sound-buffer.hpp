#pragma once
#include "util/bytes.hpp"
#include "util/math/num-types.hpp"
#include "util/str-util.hpp"

namespace sound {
enum class Compression {
  unknown = 0,
  raw,
  flac,
  //opus,
  //vorbis,
  //mp3,
  //wav,
};

enum class Format {
  unknown = 0,
  f32, // float 32 bit
  s16, // int 16 bit
  u8, // byte
};

// декодированные данные трека
struct Buffer final {
  Str source_path {}; // путь к оригинальному файлу трека
  Compression compression {}; // тип сжатия
  Format format {}; // формат выборок
  uint channels {}; // число аудио-каналов
  uint frequency {}; // частота трека
  uint samples {}; // число выборок в треке
  Bytes data {}; // данные выборок
};

} // sound ns
