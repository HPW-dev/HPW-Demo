/// @file OpenAL реализация звуковой системы
#ifndef DISABLE_SOUND
#include <OpenAL-soft/AL/al.h>
#include <OpenAL-soft/AL/alc.h>
#include <cassert>
#include "sound.hpp"
#include "sound-io.hpp"
#include "util/error.hpp"
#include "util/log.hpp"
#include "util/file/file.hpp"
#include "game/util/resource-helper.hpp"

namespace sound {

Info _info {};
Config _config {};

namespace oal {
bool initialized {};
bool eax_2_0_compat {}; // совместимость с OpenAL EAX 2.0
} // oal ns

inline static Str _decode_oal_error(const ALenum error_enum) {
  switch (error_enum) {
    case AL_NO_ERROR: return "OAL no errors"; break;
    case AL_INVALID_NAME: return "OAL invalid name"; break;
    case AL_INVALID_ENUM: return "OAL invalid enum"; break;
    case AL_INVALID_VALUE: return "OAL invalid value"; break;
    case AL_INVALID_OPERATION: return "OAL invalid operation"; break;
    case AL_OUT_OF_MEMORY: return "OAL out of memory"; break;
  }

  return "unknown OAL error";
}

inline static void _check_oal_error(cr<Str> func_name) {
  if (cauto error = alGetError(); error != AL_NO_ERROR)
    error("OAL error in " + func_name + ": " + _decode_oal_error(error));
}

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

inline static void _close_oal() {
  ret_if (!oal::initialized);
  log_debug << "disable OpenAL...";

  log_warning << "TODO need impl";
  /* TODO
  for (crauto audio_info: m_audio_infos) {
    disable(audio_info.first);
    alDeleteSources(1, &audio_info.second.oal_source_id);
    alDeleteBuffers(m_config.buffers, audio_info.second.oal_buffer_ids.data());
  }
  */

  auto oal_context = alcGetCurrentContext();
  auto oal_device = alcGetContextsDevice(oal_context);
  alcMakeContextCurrent({});
  alcDestroyContext(oal_context);
  alcCloseDevice(oal_device); 
  _info.device_name.clear();
  oal::initialized = false;
}

// init OpenAL
inline static void _init_oal() {
  _close_oal();
  log_debug << "OpenAL init...";

  // открыть аудио устройство по умолчанию (TODO сделать выбор)
  auto oal_device = alcOpenDevice({});
  iferror(!oal_device, "OAL error in alcOpenDevice({})" + _decode_oal_error(alGetError()));

  auto oal_context = alcCreateContext(oal_device, {});
  iferror(!oal_context || !alcMakeContextCurrent(oal_context),
    "error while creation OAL context" + _decode_oal_error(alGetError()));

  // Check for EAX 2.0 support
  oal::eax_2_0_compat = alIsExtensionPresent("EAX2.0"); 
  log_debug << Str("EAX 2.0 support: ") + s2yn(oal::eax_2_0_compat);
  alGetError(); // clear error code 

  // получение имени устройства воспроизведения
  cauto device_name = alcGetString(oal_device, ALC_DEVICE_SPECIFIER);
  if (device_name) {
    _info.device_name = device_name;
    log_debug << "Device name: " << _info.device_name;
  } else {
    log_warning << "error while getting OAL device name";
  }

  oal::initialized = true;
  log_debug << "OpenAL initialized";
}

inline static void _finalize() {
  ret_if (!_info.enabled);
  log_debug << "disable sound system...";
  log_warning << "TODO need impl"; // TODO
}

void init(cr<Config> cfg) {
  _finalize();

  log_debug << "init sound system:";
  _info = {};

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

  _info.enabled = true;
  _info.enabled &= oal::initialized;
  _info.enabled &= _config.enabled;
  log_debug << "sound system initialized: " << yn2s(_info.enabled);
}

void update() {
  ret_if(!_info.enabled);
  log_warning << "TODO need impl"; // TODO
}

Info info() { return _info; }

Shared<Track> play(cr<Str> path) {
  ret_if(!_info.enabled, {});
  cauto file = load_res(path);
  cauto buffer = file_to_buffer(file);
  return play(buffer);
}

Shared<Track> play(cr<Buffer> buf) {
  ret_if(!_info.enabled, {});

  log_debug << "play audio-buffer \"" << buf.source_path << "\":";
  hpw::logger.config.print_source = false;
  log_debug << "- channels: " << buf.channels;
  log_debug << "- frequency: " << buf.frequency;
  log_debug << "- samples: " << buf.samples;
  log_debug << "- data size: " << buf.data.size() << " bytes";
  hpw::logger.config.print_source = true;

  _check_audio_buffer(buf);
  log_warning << "TODO need impl"; // TODO
  return {}; // TODO
}

Tracks all_tracks() {
  log_warning << "TODO need impl"; // TODO
  return {}; // TODO
}

Track::Config Track::get_config() const {
  log_warning << "TODO need impl"; // TODO
  return {}; // TODO
}

void Track::set_config(cr<Track::Config> cfg) {
  log_warning << "TODO need impl"; // TODO
}

struct Autoclear { inline ~Autoclear() { _close_oal(); } };
static Autoclear _autoclear {};

} // sound ns
#endif // DISABLE_SOUND
