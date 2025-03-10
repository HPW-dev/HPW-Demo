#pragma once
#include "util/bytes.hpp"
#include "util/math/num-types.hpp"
#include "util/str-util.hpp"

enum class Audio_compression {
  unknown = 0,
  raw,
  flac,
  //opus,
  //vorbis,
  //mp3,
  //wav,
};

enum class Sample_format {
  unknown = 0,
  f32, // float 32 bit
  s16, // int 16 bit
  u8, // byte
};

// декодированные данные трека
struct Audio_buffer final {
  Str source_path {}; // путь к оригинальному файлу трека
  Audio_compression compression {}; // тип сжатия
  Sample_format format {}; // формат выборок
  uint channels {}; // число аудио-каналов
  uint frequency {}; // частота трека
  uint samples {}; // число выборок в треке
  Bytes data {}; // данные выборок
};
