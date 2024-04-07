#include "sound-manager.hpp"
#include "util/log.hpp"
#include "util/error.hpp"

struct Sound_mgr::Impl {
  explicit Impl() = default;

  inline Audio_ctx play(CN<Audio> sound, const bool repeat, const real amplify,
  const Vec3 listener, const Vec3 pos, const Vec3 vel) {
    // TODO
    return BAD_AUDIO;
  }

  inline bool is_playing(const Audio_ctx context) const {
    check_audio_ctx(context);
    return false; // TODO
  }

  inline void stop(const Audio_ctx context) {
    if ( !is_playing(context)) {
      detailed_log("попытка остановить звук, который уже не играет (ID: "
        << context << ")\n");
      return;
    }
  }

  inline void check_audio_ctx(const Audio_ctx context) const {
    iferror(context == BAD_AUDIO, "bad audio ID");
  }

  inline void set_amplify(const Audio_ctx context, const real amplify) {
    if ( !is_playing(context)) {
      detailed_log("попытка остановить звук, который уже не играет (ID: "
        << context << ")\n");
      return;
    }
    // TODO
  }

  inline void set_position(const Audio_ctx context, const Vec3 new_pos) {
    if ( !is_playing(context)) {
      detailed_log (
        "попытка изменить позицию звуку, который уже не играет (ID: "
        << context << ")\n");
      return;
    }
    // TODO
  }

  inline void set_velocity(const Audio_ctx context, const Vec3 new_vel) {
    if ( !is_playing(context)) {
      detailed_log (
        "попытка изменить скорость движения звуку, который уже не играет (ID: "
        << context << ")\n");
      return;
    }
    // TODO
  }
};

Sound_mgr::Sound_mgr(): impl {new_unique<Impl>()} {}
Sound_mgr::~Sound_mgr() {}
Audio_ctx Sound_mgr::play(CN<Audio> sound, const bool repeat, const real amplify,
  const Vec3 listener, const Vec3 pos, const Vec3 vel)
  { return impl->play(sound, repeat, amplify, listener, pos, vel); }
bool Sound_mgr::is_playing(const Audio_ctx context) const { return impl->is_playing(context); }
void Sound_mgr::set_amplify(const Audio_ctx context, const real amplify) { impl->set_amplify(context, amplify); }
void Sound_mgr::set_position(const Audio_ctx context, const Vec3 new_pos) { impl->set_position(context, new_pos); }
void Sound_mgr::set_velocity(const Audio_ctx context, const Vec3 new_vel) { impl->set_velocity(context, new_vel); }
void Sound_mgr::stop(const Audio_ctx context) { impl->stop(context); }