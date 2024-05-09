#include <cassert>
#include <utility>
#include <unordered_map>
#include <OpenAL-soft/AL/al.h>
#include <OpenAL-soft/AL/alc.h>
#include "sound-manager.hpp"
#include "util/log.hpp"
#include "util/error.hpp"

struct Sound_mgr::Impl {
  Audio_id m_uid {}; // для генерации audio_id
  std::unordered_map<Str, Audio> m_store {}; // хранит исходники звуков с привязкой по имени

  inline explicit Impl() {
    init_openal();
  }

  inline Audio_id play(CN<Str> sound_name, const Vec3 source_position,
  const Vec3 source_velocity, const real amplify, const bool repeat) {
    cnauto sound = find_audio(sound_name);
    // TODO
    return BAD_AUDIO;
  }

  inline void set_listener_pos(const Vec3 listener_pos) {
    // TODO
  }
  
  inline void set_listener_dir(const Vec3 listener_dir) {
    // TODO
  }

  inline bool is_playing(const Audio_id sound_id) const {
    check_audio_ctx(sound_id);
    return false; // TODO
  }

  inline void stop(const Audio_id sound_id) {
    if ( !is_playing(sound_id)) {
      detailed_log("попытка остановить звук, который уже не играет (ID: "
        << sound_id << ")\n");
      return;
    }
  }

  inline void check_audio_ctx(const Audio_id sound_id) const {
    iferror(sound_id == BAD_AUDIO, "bad audio ID");
  }

  inline void set_amplify(const Audio_id sound_id, const real amplify) {
    if ( !is_playing(sound_id)) {
      detailed_log("попытка остановить звук, который уже не играет (ID: "
        << sound_id << ")\n");
      return;
    }
    // TODO
  }

  inline void set_position(const Audio_id sound_id, const Vec3 new_pos) {
    if ( !is_playing(sound_id)) {
      detailed_log (
        "попытка изменить позицию звуку, который уже не играет (ID: "
        << sound_id << ")\n");
      return;
    }
    // TODO
  }

  inline void set_velocity(const Audio_id sound_id, const Vec3 new_vel) {
    if ( !is_playing(sound_id)) {
      detailed_log (
        "попытка изменить скорость движения звуку, который уже не играет (ID: "
        << sound_id << ")\n");
      return;
    }
    // TODO
  }

  inline void add_audio(CN<Str> sound_name, CN<Audio> sound) {
    check_audio(sound);
    m_store[sound_name] = sound;
  }

  inline void move_audio(CN<Str> sound_name, Audio&& sound) {
    check_audio(sound);
    m_store[sound_name] = std::move(sound);
  }

  inline Audio_id make_id() {
    ++m_uid;
    // скпинуть bad audio id
    if (m_uid == BAD_AUDIO)
      ++m_uid;
    return m_uid;
  }

  // проверить что звук корректно создан
  inline void check_audio(CN<Audio> sound) const {
    assert(sound.channels != 0);
    assert(sound.compression != Audio::Compression::raw_pcm_u8); // need impl
    assert(sound.compression != Audio::Compression::raw_pcm_s16); // need impl
    assert(sound.compression != Audio::Compression::opus); // need impl
    assert(sound.compression != Audio::Compression::flac); // need impl
    assert(sound.compression != Audio::Compression::vorbis); // need impl
    assert(!sound.data.empty());
    assert(sound.samples != 0);
  }

  inline CN<Audio> find_audio(CN<Str> sound_name) const {
    try {
      return m_store.at(sound_name);
    } catch(...) {
      error("Sound \"" << sound_name << "\" not finded in Sound Manager");
    }
    return *(Audio*)0; // заглушка для анализатора
  }

  inline void init_openal() {
    detailed_log("init OpenAL");
    // открыть аудио устройство по умолчанию (TODO сделать выбор)
    auto oal_device = alcOpenDevice({});
    iferror(!oal_device, "alcOpenDevice error " + decode_oal_error());
    auto oal_context = alcCreateContext(oal_device, {});
    alcMakeContextCurrent(oal_context);
    // Check for EAX 2.0 support
    //cauto COMPAT_EAX_2_0 = alIsExtensionPresent("EAX2.0"); 
    alGetError(); // clear error code 
  }

  inline Str decode_oal_error() const {
    switch (alGetError()) {
      default:
      case AL_NO_ERROR: return "OAL no errors"; break;
      case AL_INVALID_NAME: return "OAL invalid name"; break;
      case AL_INVALID_ENUM: return "OAL invalid enum"; break;
      case AL_INVALID_VALUE: return "OAL invalid value"; break;
      case AL_INVALID_OPERATION: return "OAL invalid operation"; break;
      case AL_OUT_OF_MEMORY: return "OAL out of memory"; break;
    }
    return "unknown OAL error";
  }
}; // Sound_mgr::Impl

Sound_mgr::Sound_mgr(): impl {new_unique<Impl>()} {}
Sound_mgr::~Sound_mgr() {}
Audio_id Sound_mgr::play(CN<Str> sound_name, const Vec3 source_position, const Vec3 source_velocity,
const real amplify, const bool repeat)
  { return impl->play(sound_name, source_position, source_velocity, amplify, repeat); }
void Sound_mgr::set_listener_pos(const Vec3 listener_pos) { impl->set_listener_pos(listener_pos); }
void Sound_mgr::set_listener_dir(const Vec3 listener_dir) { impl->set_listener_dir(listener_dir); }
bool Sound_mgr::is_playing(const Audio_id sound_id) const { return impl->is_playing(sound_id); }
void Sound_mgr::set_amplify(const Audio_id sound_id, const real amplify) { impl->set_amplify(sound_id, amplify); }
void Sound_mgr::set_position(const Audio_id sound_id, const Vec3 new_pos) { impl->set_position(sound_id, new_pos); }
void Sound_mgr::set_velocity(const Audio_id sound_id, const Vec3 new_vel) { impl->set_velocity(sound_id, new_vel); }
void Sound_mgr::stop(const Audio_id sound_id) { impl->stop(sound_id); }
void Sound_mgr::add_audio(CN<Str> sound_name, CN<Audio> sound) { impl->add_audio(sound_name, sound); }
void Sound_mgr::move_audio(CN<Str> sound_name, Audio&& sound) { impl->move_audio(sound_name, std::move(sound)); }
