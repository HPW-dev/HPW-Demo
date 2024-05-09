#include <utility>
#include "sound-manager.hpp"
#include "util/log.hpp"
#include "util/error.hpp"

struct Sound_mgr::Impl {
  Audio_id m_uid {}; // для генерации audio_id

  explicit Impl() = default;

  inline Audio_id play(CN<Str> sound_name, const Vec3 source_position,
  const Vec3 source_velocity, const real amplify, const bool repeat) {
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
    // TODO
  }

  inline void move_audio(CN<Str> sound_name, Audio&& sound) {
    // TODO
  }

  inline Audio_id make_id() {
    ++m_uid;
    // скпинуть bad audio id
    if (m_uid == BAD_AUDIO)
      ++m_uid;
    return m_uid;
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
