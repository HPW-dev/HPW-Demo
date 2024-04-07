#pragma once 
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"
#include "audio.hpp"

struct Vec3 {
  real x {}, y {}, z {};

  Vec3() = default;
  inline explicit Vec3(const auto _x, const auto _y={}, const auto _z={})
  : x(_x)
  , y(_y)
  , z(_z)
  {}
};

// управляет воспроизведением звука
class Sound_mgr {
  struct Impl;
  Unique<Impl> impl {};

public:
  explicit Sound_mgr();
  ~Sound_mgr();
  // проиграть звук
  Audio_ctx play(CN<Audio> sound, const bool repeat=true, const real amplify=1.0,
    const Vec3 listener={}, const Vec3 pos={}, const Vec3 vel={});
  // останавливает проигрывание звука
  void stop(const Audio_ctx context);
  // настроить громкость звука
  void set_amplify(const Audio_ctx context, const real amplify);
  // настроить позицию источника звука
  void set_position(const Audio_ctx context, const Vec3 new_pos);
  // настроить скорость источника звука
  void set_velocity(const Audio_ctx context, const Vec3 new_vel);
  // проверить что трек запущет
  bool is_playing(const Audio_ctx context) const;
};
