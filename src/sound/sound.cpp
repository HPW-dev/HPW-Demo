#include <cassert>
#include "sound.hpp"
#include "util/error.hpp"
#include "util/log.hpp"

namespace sound {

Info _info {};
Config _config {};

// проверяет настройки аудио-буффера
inline static void _check_audio_buffer(cr<Buffer> buf) {
  iferror(buf.channels < 1 || buf.channels > 2, "bad count of audio-channels: " << buf.channels);
  iferror(buf.compression == Compression::unknown, "unknown audio compression format");
  iferror(buf.format == Format::unknown, "unknown audio sample format");
  iferror(buf.frequency == 0, "frequency is 0");
  iferror(buf.frequency >= 600'000, "frequency >= 600K");
  iferror(buf.samples == 0, "audio samples is empty");
  iferror(buf.data.size() >= 1024 * 1024 * 1024 * 1.5, "audio data >= 1.5 Gb");
  iferror(buf.data.empty(), "data is empty");
}

// init OpenAL
inline static void _init_oal() {
  // TODO
}

void init(cr<Config> cfg) {
  log_debug << "init sound system:";
  hpw::logger.config.print_source = false;
  log_debug << "- enabled: " << yn2s(cfg.enabled);
  log_debug << "- buffer size: " << cfg.buffer_sz << " bytes ";
  log_debug << "- buffers: " << cfg.buffers;
  log_debug << "- max sounds: " << cfg.max_sounds;
  hpw::logger.config.print_source = true;

  iferror(cfg.buffer_sz == 0 || cfg.buffer_sz >= 1024 * 1024 * 1024, "bad audio-buffer size (" << cfg.buffer_sz << ")");
  iferror(cfg.buffers == 0 || cfg.buffers >= 500, "bad count of audio-buffers (" << cfg.buffers << ")");
  iferror(cfg.max_sounds == 0 || cfg.max_sounds >= 4'000'000, "bad max_sounds (" << cfg.max_sounds << ")");

  _config = cfg;
  _init_oal();
  _info.enabled = _config.enabled;
  log_debug << "sound system is initialized";
}

void update() {
  ret_if(!_info.enabled);
  // TODO
}

Info info() { return _info; }

Shared<Track> play(cr<Str> path) {
  ret_if(!_info.enabled, {});
  return {}; // TODO
}

Shared<Track> play(cr<Buffer> buf) {
  ret_if(!_info.enabled, {});
  _check_audio_buffer(buf);
  return {}; // TODO
}

Tracks all_tracks() {
  return {}; // TODO
}

Track::Config Track::get_config() const {
  return {}; // TODO
}

void Track::set_config(cr<Track::Config> cfg) {
  // TODO
}

} // sound ns
