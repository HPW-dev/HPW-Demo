#pragma once
#include "util/macro.hpp"
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"
#include "util/str-util.hpp"

// Управляет аудио-системой
class Sound_mgr final {
private:
  struct Impl;

public:
  // для стартовой настройки аудиосистемы
  struct Config {
    std::size_t buffers = 4; // число сменяющихся буферов потока
    std::size_t buffer_sz = 1024 * 16; // размер одного буфера потока в байтах
    uint sounds = 100; // сколько звуков можно проиграть одновременно
  };

  // состояние аудио-системы
  struct Status {
    uint audios_running_now {};
    bool enabled {}; // false - звукy
  };

  // для упралвения конкретным звуком
  class Audio final {
  public:
    inline explicit Audio(Sound_mgr::Impl& master): _master {master} {}
    ~Audio() = default;
  
  private:
    Sound_mgr::Impl& _master;
  };

  explicit Sound_mgr(cr<Config> cfg);
  ~Sound_mgr();
  void clear(); // убрать все звуки
  void stop_all(); // останавливает все звуки (не удаляя их)
  void continue_all(); // продолжает воспроизведение всех звуков
  void update();
  cr<Status> status() const; // узнать текущее состояние аудио-системы
  Weak<Audio> make(cr<Str> name); // запустить звук

private:
  struct Impl;
  Unique<Impl> _impl {};
};

/*
#pragma once 
#include "util/mem-types.hpp"
#include "util/macro.hpp"
#include "util/str.hpp"
#include "audio.hpp"

class Entity;

// 3D координаты
struct Vec3 {
  real x {}, y {}, z {};
  Vec3() = default;
  inline explicit Vec3(const auto _x, const auto _y, const auto _z):
     x(_x), y(_y), z(_z) {}
};

// для настройки менеджера звуков
struct Sound_mgr_config {
  std::size_t buffers = 4; // число сменяющихся буферов потока
  std::size_t buffer_sz = 1024 * 16; // размер одного буфера потока в байтах
  uint sounds = 100; // сколько звуков можно проиграть одновременно
};

struct Device_name {
  constx uint BAD_ID = 0xFFFFu;
  std::size_t id {BAD_ID};
  Str name {};
};

// узнать какие звуковые устройства доступны
[[nodiscard]] Vector<Device_name> get_audio_devices();

// управляет воспроизведением звука (база для реализаций)
class Sound_mgr {
  nocopy(Sound_mgr);

public:
  explicit Sound_mgr() = default;
  virtual ~Sound_mgr() = default;
  // проиграть звук
  virtual Audio_id play(cr<Str> sound_name, const Vec3 source_position={}, const Vec3 source_velocity={},
    const real amplify=1.0, const bool repeat=false) = 0;
  // утсановить положение слушателя
  virtual void set_listener_pos(const Vec3 listener_pos) = 0;
  // останавливает проигрывание звука
  virtual void stop(const Audio_id sound_id) = 0;
  // продолжает воспроизведение остановленного звука
  virtual void continue_play(const Audio_id sound_id) = 0;
  // ставит все звуки на паузу
  virtual void stop_all() = 0;
  // убирает все звуки с паузы
  virtual void continue_all() = 0;
  // настроить громкость звука
  virtual void set_amplify(const Audio_id sound_id, const real amplify) = 0;
  // настроить позицию источника звука
  virtual void set_position(const Audio_id sound_id, const Vec3 new_pos) = 0;
  // настроить скорость источника звука
  virtual void set_velocity(const Audio_id sound_id, const Vec3 new_vel) = 0;
  // сменить тон звука
  virtual void set_pitch(const Audio_id sound_id, const real pitch) = 0;
  // проверить что трек запущет
  virtual bool is_playing(const Audio_id sound_id) const = 0;
  // загрузить звук
  virtual void add_audio(cr<Str> sound_name, cr<Audio> sound) = 0;
  // загрузить звук через перемещение
  virtual void move_audio(cr<Str> sound_name, Audio&& sound) = 0;
  // громкость всей системы
  virtual void set_master_gain(const real gain) = 0;
  // изменить влияние вектора скорости на эффект Допплера
  virtual void set_doppler_factor(const real doppler_factor) = 0;
  // выключить определённый звук
  virtual void disable(const Audio_id sound_id) = 0;
  // найти загруженный звук по имени
  virtual cr<Audio> find_audio(cr<Str> sound_name) const = 0;
  // заткнуть все звуки
  virtual void shutup() = 0;
}; // Sound_mgr

// реализация звука через OpenAL
class Sound_mgr_oal final: public Sound_mgr {
  struct Impl;
  Unique<Impl> impl {};
  nocopy(Sound_mgr_oal);

public:
  explicit Sound_mgr_oal(cr<Sound_mgr_config> config={});
  ~Sound_mgr_oal();
  Audio_id play(cr<Str> sound_name, const Vec3 source_position={}, const Vec3 source_velocity={},
    const real amplify=1.0, const bool repeat=false) override;
  void set_listener_pos(const Vec3 listener_pos) override;
  void stop(const Audio_id sound_id) override;
  void continue_play(const Audio_id sound_id) override;
  void set_amplify(const Audio_id sound_id, const real amplify) override;
  void set_position(const Audio_id sound_id, const Vec3 new_pos) override;
  void set_velocity(const Audio_id sound_id, const Vec3 new_vel) override;
  void set_pitch(const Audio_id sound_id, const real pitch) override;
  bool is_playing(const Audio_id sound_id) const override;
  void add_audio(cr<Str> sound_name, cr<Audio> sound) override;
  void move_audio(cr<Str> sound_name, Audio&& sound) override;
  void set_master_gain(const real gain) override;
  void set_doppler_factor(const real doppler_factor) override;
  void disable(const Audio_id sound_id) override;
  cr<Audio> find_audio(cr<Str> sound_name) const override;
  void shutup() override;
  void stop_all() override;
  void continue_all() override;
}; // Sound_mgr_oal

// Беззвучная заглушка
class Sound_mgr_nosound final: public Sound_mgr {
  nocopy(Sound_mgr_nosound);

public:
  inline explicit Sound_mgr_nosound() = default;
  inline ~Sound_mgr_nosound() = default;
  inline Audio_id play(cr<Str> sound_name, const Vec3 source_position={},
    const Vec3 source_velocity={}, const real amplify=1.0,
    const bool repeat=false) override { return BAD_AUDIO; }
  inline void set_listener_pos(const Vec3 listener_pos) override {}
  inline void stop(const Audio_id sound_id) override {}
  inline void continue_play(const Audio_id sound_id) override {}
  inline void set_amplify(const Audio_id sound_id, const real amplify) override {}
  inline void set_position(const Audio_id sound_id, const Vec3 new_pos) override {}
  inline void set_velocity(const Audio_id sound_id, const Vec3 new_vel) override {}
  inline void set_pitch(const Audio_id sound_id, const real pitch) override {}
  inline bool is_playing(const Audio_id sound_id) const override { return false; }
  inline void add_audio(cr<Str> sound_name, cr<Audio> sound) override {}
  inline void move_audio(cr<Str> sound_name, Audio&& sound) override {}
  inline void set_master_gain(const real gain) override {}
  inline void set_doppler_factor(const real doppler_factor) override {}
  inline void disable(const Audio_id sound_id) override {}
  cr<Audio> find_audio(cr<Str> sound_name) const override;
  inline void shutup() override {}
  inline void stop_all() override {}
  inline void continue_all() override {}
}; // Sound_mgr_nosound
*/