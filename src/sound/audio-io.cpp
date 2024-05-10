#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <stb/stb_vorbis.c>
#include "audio-io.hpp"
#include "audio.hpp"
#include "util/str-util.hpp"
#include "util/error.hpp"
#include "util/log.hpp"

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
  // load Vorbis, &samplerate, &output);*/
  int error;
  auto mem = mem_from_file(file_name);
  auto vorbis_file = stb_vorbis_open_memory(rcast<const unsigned char*>(mem.data()), 
    mem.size(), &error, {});
  cauto vorbis_info = stb_vorbis_get_info(vorbis_file);
  cauto stream_length_in_samples = stb_vorbis_stream_length_in_samples(vorbis_file);
  stb_vorbis_close(vorbis_file);

  Audio ret;
  ret.set_path(file_name);
  ret.channels = vorbis_info.channels;
  ret.compression = Audio::Compression::vorbis;
  ret.format = Audio::Format::pcm_f32;
  ret.frequency = vorbis_info.sample_rate;
  ret.data = std::move(mem);
  ret.samples = stream_length_in_samples;
  detailed_log("channels: "    << ret.channels << '\n');
  detailed_log("compression: " << scast<int>(ret.compression) << '\n');
  detailed_log("format: "      << scast<int>(ret.format) << '\n');
  detailed_log("frequency: "   << ret.frequency << '\n');
  detailed_log("data.sz: "     << ret.data.size() << '\n');
  detailed_log("samples: "     << ret.samples << '\n');
  return ret;
} // load_vorbis

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
