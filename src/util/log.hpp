#pragma once
#include <source_location>
#include <sstream>
#include <mutex>
#include <fstream>
#include "util/macro.hpp"
#include "util/str.hpp"

namespace hpw {

// Названия потоков
enum class Logger_stream {
  standard = 0,
  info = standard,
  warning,
  debug,
  error,
  null,
};
  
// настройки логгера
struct Logger_config {
  bool print_source {true};
  bool use_endl {true};
  bool use_terminal {true};
  Logger_stream stream {};
  std::ofstream file {};
};
  
// главный класс логгера
class Logger final {
  nocopy(Logger);
  inline static std::mutex mu; // для запрета одновременной записи

  void _print(std::stringstream& ss) const;
  
public:
  inline static Logger_config config;

  // для накопления данных и вызова endl в конце
  class Logger_proxy final {
  public:
    Logger_proxy(Logger& master);
    Logger_proxy(cr<Logger_proxy> other);
    Logger_proxy(Logger_proxy&& other);
    ~Logger_proxy();

    Logger_proxy& operator << (auto val) {
      _ss << val;
      return *this;
    }

  private:
    Logger& _master;
    std::stringstream _ss {};
  }; // Logger_proxy

  Logger() = default;
  ~Logger();

  inline Logger_proxy operator << (auto val) {
    Logger_proxy proxy(*this);
    proxy << val;
    return proxy;
  }

  void set_stream(Logger_stream stream);
  static void set_color(Logger_stream stream);
  static Str get_source_location(cr<std::source_location> sl = std::source_location::current());
}; // Logger

inline Logger logger {};

void close_log_file();
// открыть файл лога для вставки в конец
void reopen_log_file(cr<Str> fname);

} // npw ns

hpw::Logger& config_log_to_info(cr<std::source_location> sl = std::source_location::current());
hpw::Logger& config_log_to_warning(cr<std::source_location> sl = std::source_location::current());
hpw::Logger& config_log_to_debug(cr<std::source_location> sl = std::source_location::current());
hpw::Logger& config_log_to_error(cr<std::source_location> sl = std::source_location::current());
hpw::Logger& config_log_to_none(cr<std::source_location> sl = std::source_location::current());

#define log_info config_log_to_info()
#define log_warning config_log_to_warning()
#define log_debug config_log_to_debug()
#define log_error config_log_to_error()
#define log_none config_log_to_none()
