#pragma once
#include "sound-buffer.hpp"
#include "util/macro.hpp"

namespace sound {

// контролирует воспроизведение отдельного трека
class Track final {
public:
  struct Config {
    bool playing {}; // false - выключить трек
    bool stoped {};  // true - поставить трек на паузу
    real gain {};    // громкость трека
    real pitch {};   // тон трека
    // TODO 3d vec
  };

  Uid uid {};
  Str name {};

  Track() = default;
  ~Track() = default;

#ifdef DISABLE_SOUND
  inline Config get_config() const { return {}; }
  inline void set_config(cr<Config> cfg) {}
#else
  Config get_config() const;
  void set_config(cr<Config> cfg); // изменить параметры воспроизведения
#endif
}; // Track

// для стартовой настройки аудиосистемы
struct Config final {
  bool enabled {true};        // false - без звука
  bool lazy_loading {true};   // true - грузить аудио файлы только в момент обращения к ним
  uint buffers = 4;           // число сменяющихся буферов потока
  uint buffer_sz = 1024 * 16; // размер одного буфера потока в байтах
  uint max_sounds = 100;      // сколько звуков можно проиграть одновременно
};

struct Info final {
  bool enabled {};            // true - аудио-система включена
  uint tracks_playing_now {}; // сколько треков играет сейчас
  uint tracks_stoped_now {};  // сколько треков на паузе сейчас
  uint total_played {};       // сколько треков было проиграно
  Str device_name {};         // название устройства вывода аудио
};

using Tracks = Vector<Shared<Track>>;

#ifdef DISABLE_SOUND
  inline void init(cr<Config> cfg = {}) {}
  inline void update() {}
  inline Info info() { return {}; }
  inline Shared<Track> play(cr<Buffer> buf) { return {}; }
  inline Shared<Track> play(cr<Str> path) { return {}; }
  inline Tracks all_tracks() { return {}; }
#else
  void init(cr<Config> cfg = {});     // запуск аудио-системы
  void update();                      // обновить состояние воспроизводимых треков
  Info info();                        // получить текущий статус аудио-системы
  Shared<Track> play(cr<Buffer> buf); // запустить трек из аудио-буффера
  Shared<Track> play(cr<Str> path);   // поискать трек по имени из ресурсов
  Tracks all_tracks();                // получить все треки, которые проигрываются или на паузе
#endif

} // sound ns
