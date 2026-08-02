#include <dr_libs/dr_flac.h>
#include <filesystem>
#include <string>
#include <utility>
#include <unordered_map>
#include "audio-io.hpp"
#include "audio.hpp"
#include "util/str-util.hpp"
#include "util/error.hpp"
#include "util/log.hpp"
#include "util/file/file-io.hpp"
#include "game/util/resource-helper.hpp"
#define STB_VORBIS_HEADER_ONLY
#include <stb/stb_vorbis.c>

// угадать формат файла по имени
[[nodiscard]] Audio::Compression quess_format(cr<Str> file_name) {
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

Audio load_raw(cr<Bytes> mem, cr<Str> path) {
  Audio ret;
  ret.set_path(path);
  // предположительные данные, потому что с RAW не идёт инфы о треке
  ret.channels = 1;
  ret.compression = Audio::Compression::raw;
  ret.frequency = 44'100;
  ret.format = Audio::Format::raw_pcm_u8;
  ret.data = mem;
  ret.samples = ret.data.size();
  return ret;
}

Audio load_flac(cr<Bytes> mem, cr<Str> path) {
  // узнать инфу о FLAC файле
  auto flac_info = drflac_open_memory(mem.data(), mem.size(), {});
  Audio ret;
  ret.channels = flac_info->channels;
  ret.frequency = flac_info->sampleRate;
  ret.samples = flac_info->totalPCMFrameCount;
  switch (flac_info->bitsPerSample) {
    case 8: ret.format = Audio::Format::raw_pcm_u8; break;
    case 16: ret.format = Audio::Format::raw_pcm_s16; break;
    case 32: ret.format = Audio::Format::pcm_f32; break;
    default: error("Error while reading FLAC sample format");
  }
  ret.set_path(path);
  ret.data = std::move(mem);
  ret.compression = Audio::Compression::flac;
  drflac_free(flac_info, {});

  log_debug << "channels: "    << ret.channels;
  log_debug << "compression: " << scast<int>(ret.compression);
  log_debug << "format: "      << scast<int>(ret.format);
  log_debug << "frequency: "   << ret.frequency;
  log_debug << "data.sz: "     << ret.data.size();
  log_debug << "samples: "     << ret.samples;
  return ret;
}

Audio load_opus(cr<Bytes> mem, cr<Str> path) {
  // TODO
  error("need impl for Opus loader");
  Audio ret;
  ret.set_path(path);
  return ret;
}

Audio load_vorbis(cr<Bytes> mem, cr<Str> path) {
  int error;
  auto vorbis_file = stb_vorbis_open_memory(rcast<const unsigned char*>(mem.data()), 
    mem.size(), &error, {});
  cauto vorbis_info = stb_vorbis_get_info(vorbis_file);
  cauto stream_length_in_samples = stb_vorbis_stream_length_in_samples(vorbis_file);
  stb_vorbis_close(vorbis_file);

  Audio ret;
  ret.set_path(path);
  ret.channels = vorbis_info.channels;
  ret.compression = Audio::Compression::vorbis;
  ret.format = Audio::Format::pcm_f32;
  ret.frequency = vorbis_info.sample_rate;
  ret.data = std::move(mem);
  ret.samples = stream_length_in_samples;
  log_debug << "channels: "    << ret.channels;
  log_debug << "compression: " << scast<int>(ret.compression);
  log_debug << "format: "      << scast<int>(ret.format);
  log_debug << "frequency: "   << ret.frequency;
  log_debug << "data.sz: "     << ret.data.size();
  log_debug << "samples: "     << ret.samples;
  return ret;
} // load_vorbis

Audio load_audio_from_memory(cr<File> file) {
  cauto path = file.get_path();
  iferror(path.empty(), "load_audio_from_memory: file.path is empty");
  cauto format = quess_format(path);
  switch (format) {
    case Audio::Compression::raw:    return load_raw(file.data, path);    break;
    case Audio::Compression::flac:   return load_flac(file.data, path);   break;
    case Audio::Compression::opus:   return load_opus(file.data, path);   break;
    case Audio::Compression::vorbis: return load_vorbis(file.data, path); break;
    default: error("unsupported audio compression format");
  }
  error("audio file \"" << path << "\" not loaded (from memory)");
  return {};
}

Audio load_audio(cr<Str> file_name) {
  cauto format = quess_format(file_name);
  cauto file = load_res(file_name);
  switch (format) {
    case Audio::Compression::raw:    return load_raw(file.data, file_name);    break;
    case Audio::Compression::flac:   return load_flac(file.data, file_name);   break;
    case Audio::Compression::opus:   return load_opus(file.data, file_name);   break;
    case Audio::Compression::vorbis: return load_vorbis(file.data, file_name); break;
    default: error("unsupported audio compression format");
  }
  error("audio file \"" << file_name << "\" not loaded");
  return {};
}
