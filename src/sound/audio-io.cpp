#include <filesystem>
#include <fstream>
#include <unordered_map>
#include "audio-io.hpp"
#include "audio.hpp"
#include "util/str-util.hpp"
#include "util/error.hpp"
#include "util/file/file.hpp"

Audio load_audio_from_archive(CN<Str> file_name) {
  return {}; // TODO
}

// угадать формат файла по имени
[[nodiscard]] Audio::Compression quess_format(CN<Str> file_name) {
  iferror(file_name.empty(), "empty audio file name");

  auto ext = std::filesystem::path(file_name).extension().string();
  ext = str_tolower(ext);
  static const std::unordered_map<Str, Audio::Compression> extension_table {
    {".vorbis", Audio::Compression::vorbis},
    {".ogg", Audio::Compression::vorbis}, // TODO тут и Opus может быть
    {".opus", Audio::Compression::opus},
    {".flac", Audio::Compression::flac},
    {".raw", Audio::Compression::raw},
    {".dat", Audio::Compression::raw},
    {".bin", Audio::Compression::raw},
  };

  try {
    return extension_table.at(ext);
  } catch(...) {}

  error("error format detection for audio file \"" << file_name << "\"");
  return {};
} // quess_format

Audio load_raw(CN<Str> file_name) {
  Audio ret;
  ret.set_path(file_name);
  // предположительные данные, потому что с RAW не идёт инфы о треке
  ret.channels = 1;
  ret.compression = Audio::Compression::raw;
  ret.frequency = 44'100;
  ret.format = Audio::Format::raw_pcm_u8;
  ret.data = mem_from_file(file_name);
  ret.samples = ret.data.size();
  return ret;
}

Audio load_flac(CN<Str> file_name) {
  // TODO
  error("need impl for FLAC loader");
  Audio ret;
  ret.set_path(file_name);
  return ret;
}

Audio load_opus(CN<Str> file_name) {
  // TODO
  error("need impl for Opus loader");
  Audio ret;
  ret.set_path(file_name);
  return ret;
}

Audio load_vorbis(CN<Str> file_name) {
  error("need impl for Vorbis loader");
  Audio ret;
  ret.set_path(file_name);
  return ret;
}

Audio load_audio(CN<Str> file_name) {
  cauto format = quess_format(file_name);
  switch (format) {
    default:
    case Audio::Compression::raw:    return load_raw(file_name);    break;
    case Audio::Compression::flac:   return load_flac(file_name);   break;
    case Audio::Compression::opus:   return load_opus(file_name);   break;
    case Audio::Compression::vorbis: return load_vorbis(file_name); break;
  }
  error("audio file \"" << file_name << "\" not loaded");
  return {};
}
