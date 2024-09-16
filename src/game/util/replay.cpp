#include <iomanip>
#include <cassert>
#include <fstream>
#include <sstream>
#include <cstring>
#include <unordered_set>
#include "replay.hpp"
#include "game/util/version.hpp"
#include "game/util/keybits.hpp"
#include "game/util/score-table.hpp"
#include "game/util/game-util.hpp"
#include "game/hud/hud-util.hpp"
#include "game/core/core.hpp"
#include "game/core/huds.hpp"
#include "game/core/common.hpp"
#include "game/core/replays.hpp"
#include "game/core/user.hpp"
#include "game/core/difficulty.hpp"
#include "game/core/levels.hpp"
#include "game/core/scenes.hpp"
#include "game/core/tasks.hpp"
#include "game/scene/msgbox/msgbox-enter.hpp"
#include "util/file/file.hpp"
#include "util/math/random.hpp"
#include "util/vector-types.hpp"
#include "util/hpw-util.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"
#include "util/log.hpp"
#include "util/platform.hpp"
#include "util/unicode.hpp"

#ifdef ECOMEM
using Stream = std::fstream;
#else
struct Stream {
  Vector<char> data {};
  std::size_t pos {};

  inline Stream() {
    data.reserve(1024 * 100); // обычно столько весит реплей
  }

  inline void write(cp<char> in, const std::size_t sz) {
    assert(sz > 0);
    assert(in != nullptr);
    data.resize(data.size() + sz);
    assert(data.data() != nullptr);
    std::memcpy(data.data() + pos, in, sz);
    pos += sz;
  }

  inline void read(char* out, const std::size_t sz) {
    assert(sz > 0);
    assert(out != nullptr);
    iferror (pos + sz > data.size(), "прочитано больше данных, чем есть в Stream");
    std::memcpy(out, data.data() + pos, sz);
    pos += sz;
  }

  inline void set_pos(std::size_t new_pos) { pos = new_pos; }

  inline bool eof() const { return pos >= data.size(); }
};
#endif

// создаст окно с ошибкой при сохранении реплея
class Save_error final: public Task {
  Str _path {};
public:
  inline explicit Save_error(cr<Str> path): _path {path} {}

  inline void update(const Delta_time dt) override {
    cauto msg = utf32_to_8(get_locale_str("scene.replay.error.while_save_to"))
      + " \"" + (_path) + "\"";
    hpw_log("Error: " << msg << '\n');
    cauto title = get_locale_str("common.error");
    assert(hpw::scene_mgr);
    hpw::scene_mgr->add(new_shared<Scene_msgbox_enter>(utf8_to_32(msg), title));
    this->deactivate();
  }
};

enum class Platform: byte {
  error=0,
  windows,
  linux,
  macos,
  android,
};

enum class Bits: byte {
  error=0,
  x32,
  x64,
};

inline Platform get_platform() {
  #ifdef WINDOWS
    return Platform::windows;
  #else
    return Platform::linux;
  #endif
  return Platform::error;
}

inline Bits get_bits() {
  #ifdef is_x32
    return Bits::x32;
  #else
    return Bits::x64;
  #endif
  return Bits::error;
}

inline void write_str(Stream& file, cr<Str> str) {
  const uint32_t sz = str.size();
  file.write(cptr2ptr<cp<char>>(&sz), sizeof(sz));
  if (sz > 0)
    file.write(cptr2ptr<cp<char>>(str.data()), sz * sizeof(Str::value_type));
}

inline void write_key_packet(Stream& file, cr<Key_packet> key_packet) {
  const uint32_t sz = key_packet.size();
  assert(sz < 1'000);
  file.write(cptr2ptr<cp<char>>(&sz), sizeof(sz));
  if (sz > 0)
    file.write(cptr2ptr<cp<char>>(key_packet.data()), sz * sizeof(Key_packet::value_type));
}

inline Str read_str(Stream& file) {
  uint32_t sz {0};
  file.read(ptr2ptr<char*>(&sz), sizeof(sz));
  return_if (sz == 0, {});

  Vector<Str::value_type> data(sz);
  file.read(ptr2ptr<char*>(data.data()), sz * sizeof(Str::value_type));
  return Str(data.begin(), data.end());
}

inline Key_packet read_key_packet(Stream& file) {
  uint32_t sz {};
  file.read(ptr2ptr<char*>(&sz), sizeof(sz));
  assert(sz < 1'000);
  return_if (sz == 0, {});

  Key_packet ret(sz);
  file.read(ptr2ptr<char*>(ret.data()), sz * sizeof(Key_packet::value_type));
  return ret;
}

template <typename T>
T read_data(Stream& file) {
  static_assert(sizeof(T) > 0);
  T ret;
  file.read(ptr2ptr<char*>(&ret), sizeof(T));
  return ret;
}

template <typename T>
void write_data(Stream& file, const T& data) {
  static_assert(sizeof(T) > 0);
  file.write(cptr2ptr<cp<char>>(&data), sizeof(T));
}

template <typename T>
void write_data(Stream& file, const T&& data) {
  static_assert(sizeof(T) > 0);
  file.write(cptr2ptr<cp<char>>(&data), sizeof(T));
}

struct Replay::Impl {
  Str m_ver {"v3.3"};
  Str m_path {};
  Stream m_file {};
  bool m_write_mode {};
  Info m_info {};
  bool m_nosave {false};

  inline ~Impl() { close(); }

  inline Impl(cr<Str> path, const bool write_mode, const bool nosave=false)
  : m_path { path }
  , m_write_mode { write_mode }
  , m_nosave { nosave }
  {
    conv_sep(m_path);
    #ifdef ECOMEM
    m_file.open(m_path, std::ios_base::binary |
      (write_mode ? std::ios_base::out : std::ios_base::in) );
    #endif

    if (write_mode)
      write_header();
    else
      read_header();
  } // c-tor

  // запись заголовка
  inline void write_header() {
    // версия реплея
    write_str(m_file, m_ver);
    // версия игры
    write_str(m_file, cstr_to_cxxstr(get_game_version()));
    // платформа
    write_data(m_file, get_platform());
    write_data(m_file, get_bits());
    // SHA256
    write_str(m_file, hpw::exe_sha256);
    write_str(m_file, hpw::data_sha256);
    // UPS
    const uint32_t target_ups = hpw::target_ups;
    write_data(m_file, target_ups);
    // сид рандома
    const uint32_t seed = get_rnd_seed();
    write_data(m_file, seed);
    // имя игрока
    write_str(m_file, utf32_to_8(hpw::player_name));
    // уровень сложности
    write_data(m_file, hpw::difficulty);
    // рекорд
    write_data(m_file, hpw::get_score());
    // дата
    write_str(m_file, get_data_str());
    // начальный уровень - туториал?
    write_data(m_file, hpw::first_level_is_tutorial);
    // с каким интерфейсом начали игру
    write_str(m_file, graphic::cur_hud);
  } // write_header

  // чтение заголовка
  inline void read_header() {
    #ifndef ECOMEM
    {
      // чтение с диска в память
      auto mem = mem_from_file(m_path);
      m_file.write(cptr2ptr<cp<char>>(mem.data()), mem.size());
      m_file.set_pos(0);
    }
    #endif

    // версия реплея
    auto ver = read_str(m_file);
    iferror(ver != m_ver, "версия реплея несовместима с игрой");
    // версия игры
    auto cur_game_ver = cstr_to_cxxstr(get_game_version());
    auto rep_game_ver = read_str(m_file);
    if (cur_game_ver != rep_game_ver) {
      // TODO показывать окно с ворнингом
      hpw_log("версия реплея не совпадает с версией игры\n");
      hpw_log("  текущая версия игры: " << cur_game_ver << '\n');
      hpw_log("  версия игры в реплее: " << rep_game_ver << '\n');
      hpw_log("  файл реплея: \"" << m_path << "\"\n");
    }
    
    // платформа
    cauto platform = read_data<Platform>(m_file);
    cauto bits = read_data<Bits>(m_file);
    if (bits != get_bits()) {
      hpw_log("реплей записан на системе с разрядностью отличающейся от вашей\n");
      // TODO окно с предупреждением
    }
    if (platform != get_platform()) {
      hpw_log("реплей записан на системе отличающейся от вашей\n");
      // TODO окно с предупреждением
    }
    // SHA256
    auto exe_sha256 = read_str(m_file);
    auto data_sha256 = read_str(m_file);
    if (exe_sha256 != hpw::exe_sha256
    || data_sha256 != hpw::data_sha256) {
      hpw_log("чексуммы в реплее не совпадают\n");
      hpw_log("SHA256 EXE игры: " << hpw::exe_sha256 << '\n');
      hpw_log("SHA256 EXE реплея: " << exe_sha256 << '\n');
      hpw_log("SHA256 DATA игры: " << hpw::data_sha256 << '\n');
      hpw_log("SHA256 DATA реплея: " << data_sha256 << '\n');
      // TODO вызов окна с надписью
    }
    // UPS
    cauto target_ups = read_data<uint32_t>(m_file);
    iferror(target_ups != scast<uint32_t>(hpw::target_ups),
      "UPS реплея не совпали с игрой");
    (void)target_ups; // чтоб небыло ворнинга при релизе
    // сид рандома
    cauto seed = read_data<uint32_t>(m_file);
    set_rnd_seed(seed);
    // имя игрока
    cauto player_name = utf8_to_32(read_str(m_file));
    // уровень сложности
    cauto difficulty = read_data<Difficulty>(m_file);
    hpw::difficulty = difficulty;
    // рекорд
    cauto score = read_data<int64_t>(m_file);
    // дата
    cauto date = read_str(m_file);
    // начальный уровень - туториал?
    hpw::first_level_is_tutorial = read_data<decltype(hpw::first_level_is_tutorial)>(m_file);
    // с каким интерфейсом начали игру
    graphic::cur_hud = read_str(m_file);
    graphic::hud = make_hud(graphic::cur_hud);

    m_info.path = m_path;
    m_info.date_str = date;
    m_info.date = to_date(date);
    m_info.first_level_is_tutorial = hpw::first_level_is_tutorial;
    m_info.difficulty = difficulty;
    m_info.score = score;
    m_info.player_name = player_name;
    m_info.hud_name = graphic::cur_hud;
  } // read_header

  inline Str get_data_str() const {
    auto t = std::time(nullptr);
    #ifdef LINUX
      struct ::tm lt;
      ::localtime_r(&t, &lt);
    #else // WINDOWS
      auto lt = *std::localtime(&t);
    #endif
    std::stringstream ss;
    ss << std::put_time(&lt, "%d.%m.%Y %H:%M:%S");
    return ss.str();
  }

  inline static Info get_info(cr<Str> path) {
    Impl replay(path, false, true);
    return replay.m_info;
  }

  inline void close() {
    #ifdef ECOMEM
      m_file.close();
    #else
      return_if(m_nosave); // не сейвить в файл
      return_if(m_file.data.empty());

      // чтоб два раза не вызвать close
      Scope _({}, [this]{ m_file.data.clear(); });

      // запись с буффера на диск
      std::ofstream file(m_path, std::ios_base::binary);

      if (!file.is_open()) {
        hpw::global_task_mgr.add(new_shared<Save_error>(m_path));
        return;
      }

      file.write(m_file.data.data(), m_file.data.size());
    #endif
  }

  inline void push(cr<Key_packet> key_packet) {
    assert(m_write_mode);
    write_key_packet(m_file, key_packet);
  }

  inline std::optional<Key_packet> pop() {
    assert(!m_write_mode);
    if (m_file.eof())
      return {};
    return read_key_packet(m_file);
  }

  // конвертирует дату и время из строки в удобный формат
  inline static Date to_date(cr<Str> date) {
    assert(!date.empty());
    // разделение на дату и время
    auto strs = split_str(date, ' ');
    cauto date_str = strs.at(0);
    cauto time_str = strs.at(1);
    Date ret;
    // разделить на DD.MM.YY
    strs = split_str(date_str, '.');
    ret.day = s2n<int>( strs.at(0) );
    ret.month = s2n<int>( strs.at(1) );
    ret.year = s2n<int>( strs.at(2) );
    // разделить на HH:MM:SS
    strs = split_str(time_str, ':');
    ret.hour = s2n<int>( strs.at(0) );
    ret.minute = s2n<int>( strs.at(1) );
    ret.second = s2n<int>( strs.at(2) );
    // проверить диапазоны
    iferror(ret.day == 0 || ret.day >= 32, "неправильный день (" << n2s(ret.day) << ")");
    iferror(ret.month == 0 || ret.month > 12, "неправильный месяц (" << n2s(ret.month) << ")");
    iferror(ret.year < 1815, "год не должен быть меньше чем 1815 (" << n2s(ret.year) << ")");
    iferror(ret.hour > 24, "неправильный час (" << n2s(ret.hour) << ")");
    iferror(ret.minute > 59, "неправильная минута (" << n2s(ret.minute) << ")");
    iferror(ret.second > 59, "неправильная секунда (" << n2s(ret.second) << ")");
    return ret;
  } // to_date
}; // Impl

Replay::Replay(cr<Str> path, bool write_mode)
  : impl{new_unique<Impl>(path, write_mode)} {}
Replay::~Replay() {}
void Replay::close() { impl->close(); }
void Replay::push(cr<Key_packet> key_packet) { impl->push(key_packet); }
std::optional<Key_packet> Replay::pop() { return impl->pop(); }
cp<Replay::Impl> Replay::get_impl() const { return impl.get(); }
Replay::Info Replay::get_info(cr<Str> path) { return Impl::get_info(path); }
