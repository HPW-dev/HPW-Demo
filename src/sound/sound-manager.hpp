#pragma once 
#include "util/mem-types.hpp"
#include "util/macro.hpp"
#include "util/str.hpp"
#include "audio.hpp"

// 3D координаты
struct Vec3 {
  real x {}, y {}, z {};
  Vec3() = default;
  inline explicit Vec3(const auto _x, const auto _y, const auto _z): x(_x) , y(_y) , z(_z) {}
};

// управляет воспроизведением звука
class Sound_mgr {
  struct Impl;
  Unique<Impl> impl {};

public:
  explicit Sound_mgr();
  ~Sound_mgr();
  // проиграть звук
  Audio_id play(CN<Str> sound_name, const Vec3 source_position={}, const Vec3 source_velocity={},
    const real amplify=1.0, const bool repeat=false);
  // утсановить положение слушателя
  void set_listener_pos(const Vec3 listener_pos);
  // утсановить направление слушателя
  void set_listener_dir(const Vec3 listener_dir);
  // останавливает проигрывание звука
  void stop(const Audio_id sound_id);
  // настроить громкость звука
  void set_amplify(const Audio_id sound_id, const real amplify);
  // настроить позицию источника звука
  void set_position(const Audio_id sound_id, const Vec3 new_pos);
  // настроить скорость источника звука
  void set_velocity(const Audio_id sound_id, const Vec3 new_vel);
  // сменить тон звука
  void set_pitch(const Audio_id sound_id, const float pitch);
  // проверить что трек запущет
  bool is_playing(const Audio_id sound_id) const;
  // загрузить звук
  void add_audio(CN<Str> sound_name, CN<Audio> sound);
  // загрузить звук через перемещение
  void move_audio(CN<Str> sound_name, Audio&& sound);
  // обновить внутренее состояние
  void update();
  // громкость всей системы
  void set_master_gain(const float gain);
  // изменить влияние вектора скорости на эффект Допплера
  void set_doppler_factor(const float doppler_factor);
};
