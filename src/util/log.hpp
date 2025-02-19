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
  
  Logger() = default;
  ~Logger();

  inline Logger& operator << (auto val) {
    std::stringstream ss;
    ss << val;
    _print(ss);
    return *this;
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

#define log_info { \
  cauto tmp = hpw::Logger::config.use_endl; \
  hpw::Logger::config.use_endl = false; \
  hpw::Logger::set_color(hpw::Logger_stream::info); \
  hpw::logger.set_stream(hpw::Logger_stream::info); \
  hpw::logger << "Info    | "; \
  hpw::logger << hpw::Logger::get_source_location(); \
  hpw::Logger::config.use_endl = tmp; \
} \
hpw::logger

#define log_error { \
  cauto tmp = hpw::Logger::config.use_endl; \
  hpw::Logger::config.use_endl = false; \
  hpw::Logger::set_color(hpw::Logger_stream::error); \
  hpw::logger.set_stream(hpw::Logger_stream::error); \
  hpw::logger << "Error   | "; \
  hpw::logger << hpw::Logger::get_source_location(); \
  hpw::Logger::config.use_endl = tmp; \
} \
hpw::logger

#define log_debug { \
  cauto tmp = hpw::Logger::config.use_endl; \
  hpw::Logger::config.use_endl = false; \
  hpw::Logger::set_color(hpw::Logger_stream::debug); \
  hpw::logger.set_stream(hpw::Logger_stream::debug); \
  hpw::logger << "Debug   | "; \
  hpw::logger << hpw::Logger::get_source_location(); \
  hpw::Logger::config.use_endl = tmp; \
} \
hpw::logger

#define log_warning { \
  cauto tmp = hpw::Logger::config.use_endl; \
  hpw::Logger::config.use_endl = false; \
  hpw::Logger::set_color(hpw::Logger_stream::warning); \
  hpw::logger.set_stream(hpw::Logger_stream::warning); \
  hpw::logger << "Warning | "; \
  hpw::logger << hpw::Logger::get_source_location(); \
  hpw::Logger::config.use_endl = tmp; \
} \
hpw::logger

#define log_null { \
  hpw::logger.set_stream(hpw::Logger_stream::null); \
} \
hpw::logger
