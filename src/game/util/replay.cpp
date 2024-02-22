#include <cassert>
#include <fstream>
#include <sstream>
#include <cstring>
#include <unordered_set>
#include "replay.hpp"
#include "game/util/version.hpp"
#include "game/util/keybits.hpp"
#include "game/core/core.hpp"
#include "game/core/common.hpp"
#include "game/core/replays.hpp"
#include "util/vector-types.hpp"
#include "util/error.hpp"
#include "util/math/random.hpp"
#include "util/str-util.hpp"
#include "util/file/file.hpp"
#include "util/log.hpp"
#include "util/platform.hpp"

#ifdef ECOMEM
using Stream = std::fstream;
#else
struct Stream {
  Vector<char> data {};
  std::size_t pos {};

  inline Stream() {
    data.reserve(1024 * 100); // обычно столько весит реплей
  }

  inline void write(CP<char> in, std::size_t sz) {
    assert(sz > 0);
    assert(in != nullptr);
    data.resize(data.size() + sz);
    assert(data.data() != nullptr);
    std::memcpy(data.data() + pos, in, sz);
    pos += sz;
  }

  inline void read(char* out, std::size_t sz) {
    assert(sz > 0);
    assert(out != nullptr);
    iferror (pos + sz > data.size(), "прочитано больше, чем есть данных в Stream");
    std::memcpy(out, data.data() + pos, sz);
    pos += sz;
  }

  inline void set_pos(std::size_t new_pos) { pos = new_pos; }

  inline bool eof() const { return pos >= data.size(); }

};
#endif

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
  #endif
  #ifdef is_x64
    return Bits::x64;
  #endif
  return Bits::error;
}

inline void write_str(Stream& file, CN<Str> str) {
  uint32_t sz = str.size();
  file.write(cptr2ptr<CP<char>>(&sz), sizeof(sz));
  if (sz > 0)
    file.write(cptr2ptr<CP<char>>(str.data()), sz * sizeof(Str::value_type));
}

inline void write_key_packet(Stream& file, CN<Key_packet> key_packet) {
  uint32_t sz = key_packet.size();
  file.write(cptr2ptr<CP<char>>(&sz), sizeof(sz));
  if (sz > 0)
    file.write(cptr2ptr<CP<char>>(key_packet.data()), sz * sizeof(Key_packet::value_type));
}

inline Str read_str(Stream& file) {
  uint32_t sz {0};
  file.read(ptr2ptr<char*>(&sz), sizeof(sz));
  if (sz == 0)
    return {};

  Vector<Str::value_type> data(sz);
  file.read(ptr2ptr<char*>(data.data()), sz * sizeof(Str::value_type));
  return Str(data.begin(), data.end());
}

inline Key_packet read_key_packet(Stream& file) {
  uint32_t sz {};
  file.read(ptr2ptr<char*>(&sz), sizeof(sz));

  if (sz == 0)
    return {};

  Key_packet ret(sz);
  file.read(ptr2ptr<char*>(ret.data()), sz * sizeof(Key_packet::value_type));
  return ret;
}

struct Replay::Impl {

  Str m_ver {"v1.0"};
  Str m_path {};
  Stream m_file {};
  bool m_write_mode {};

  inline ~Impl() { close(); }

  inline Impl(CN<Str> path, bool write_mode)
  : m_path{ path }
  , m_write_mode{ write_mode }
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

  /// запись заголовка
  inline void write_header() {
    // версия реплея
    write_str(m_file, m_ver);
    // версия игры
    write_str(m_file, cstr_to_cxxstr(get_game_version()));
    // платформа
    auto bits = get_bits();
    auto platform = get_platform();
    m_file.write(cptr2ptr<CP<char>>(&platform), sizeof(platform));
    m_file.write(cptr2ptr<CP<char>>(&bits), sizeof(bits));
    // SHA256
    write_str(m_file, hpw::exe_sha256);
    write_str(m_file, hpw::data_sha256);
    // UPS
    uint32_t target_ups = hpw::target_ups;
    m_file.write(cptr2ptr<CP<char>>(&target_ups), sizeof(target_ups));
    // сид рандома
    uint32_t seed = get_rnd_seed();
    m_file.write(cptr2ptr<CP<char>>(&seed), sizeof(seed));
  } // write_header

  /// чтение заголовка
  inline void read_header() {
    #ifndef ECOMEM
    {
      // чтение с диска в память
      auto mem = mem_from_file(m_path);
      m_file.write(cptr2ptr<CP<char>>(mem.data()), mem.size());
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
      hpw_log("версия игры: " << cur_game_ver << '\n');
      hpw_log("версия игры в реплее: " << rep_game_ver << '\n');
    }
    // платформа
    Bits bits;
    Platform platform;
    m_file.read(ptr2ptr<char*>(&platform), sizeof(platform));
    m_file.read(ptr2ptr<char*>(&bits), sizeof(bits));
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
    uint32_t target_ups {};
    m_file.read(ptr2ptr<char*>(&target_ups), sizeof(target_ups));
    assert(target_ups == scast<uint32_t>(hpw::target_ups));
    // сид рандома
    uint32_t seed {};
    m_file.read(ptr2ptr<char*>(&seed), sizeof(seed));
    set_rnd_seed(seed);
  } // read_header

  inline void close() {
    #ifdef ECOMEM
      m_file.close();
    #else
      // запись с буффера на диск
      std::ofstream file(m_path, std::ios_base::binary);
      iferror(!file || file.bad(), "не удалось записать реплей по пути \""
        << m_path << "\"");
      file.write(m_file.data.data(), m_file.data.size());
    #endif
  }

  inline void push(CN<Key_packet> key_packet) {
    assert(m_write_mode);
    write_key_packet(m_file, key_packet);
  }

  inline std::optional<Key_packet> pop() {
    assert(!m_write_mode);
    if (m_file.eof())
      return {};
    return read_key_packet(m_file);
  }

}; // Impl

Replay::Replay(CN<Str> path, bool write_mode)
  : impl{new_unique<Impl>(path, write_mode)} {}
Replay::~Replay() {}
void Replay::close() { impl->close(); }
void Replay::push(CN<Key_packet> key_packet) { impl->push(key_packet); }
std::optional<Key_packet> Replay::pop() { return impl->pop(); }
CP<Replay::Impl> Replay::get_impl() const { return impl.get(); }
