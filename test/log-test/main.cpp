#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <source_location>
#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>
#include <mutex>
#include <format>
#include "util/platform.hpp"
#include "util/macro.hpp"
#include "util/str.hpp"
#include "util/error.hpp"

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
  Logger_stream stream {};
  std::ofstream file {};
};
  
void close_log_file();

// главный класс логгера
class Logger final {
  nocopy(Logger);
  inline static std::mutex mu; // для запрета одновременной записи

  void _print(std::stringstream& ss) const;
  
public:
  inline static Logger_config config;
  
  Logger() = default;

  ~Logger() {
    // вернуть цвета как были
    set_color(Logger_stream::standard);
    close_log_file();
  }

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

void Logger::set_stream(Logger_stream stream) {
  Logger::config.stream = stream;
}

void Logger::set_color(Logger_stream stream) {
#ifdef WINDOWS
  // выбор винапишного цвета
  WORD color_console;
  switch (stream) {
    default:
    case Logger_stream::info: color_console = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN; break;
    case Logger_stream::warning: color_console = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN; break;
    case Logger_stream::debug: color_console = FOREGROUND_INTENSITY | FOREGROUND_BLUE; break;
    case Logger_stream::error: color_console = FOREGROUND_INTENSITY | FOREGROUND_RED; break;
  }

  ::HANDLE stdout_console = ::GetStdHandle(STD_OUTPUT_HANDLE);
  ::HANDLE stderr_console = ::GetStdHandle(STD_ERROR_HANDLE);
  if (stdout_console != INVALID_HANDLE_VALUE)
    ::SetConsoleTextAttribute(stdout_console, color_console);
  if (stderr_console != INVALID_HANDLE_VALUE)
    ::SetConsoleTextAttribute(stderr_console, color_console);
#else // Linux
  #pragma message("TODO need Linux cli colors")
#endif
}

void Logger::_print(std::stringstream& ss) const {
  return_if (Logger::config.stream == Logger_stream::null);

  if (Logger::config.use_endl)
    ss << std::endl;
  
  std::lock_guard lock(Logger::mu);

  if (Logger::config.stream == Logger_stream::error) {
    std::cerr << ss.str();
  } else {
    std::cout << ss.str();
    std::cout.flush();
  }
  
  if (Logger::config.file) {
    Logger::config.file << ss.str();
    Logger::config.file.flush();
  }
}

Str Logger::get_source_location(cr<std::source_location> sl) {
  if (Logger::config.print_source)
    return std::format("{}:{}: ", sl.file_name(), sl.line());
  
  return {};
}

inline Logger logger {};

// открыть файл лога для вставки в конец
void reopen_log_file(cr<Str> fname) {
  iferror(fname.empty(), "log file name is empty");
  Logger::config.file.open(fname, std::ios_base::app);
  return_if(Logger::config.file.bad());
  
  Logger::config.file << std::format("\n::::::::: start logging at {0:%H:%M %d.%m.%Y} :::::::::\n",
    std::chrono::system_clock::now());
}

void close_log_file() {
  return_if(!Logger::config.file);
  Logger::config.file << std::format("\nstop logging at {0:%H:%M %d.%m.%Y}\n",
    std::chrono::system_clock::now());
  Logger::config.file.close();
}

} // npw ns

#define log_info { \
  hpw::Logger::config.use_endl = false; \
  hpw::Logger::set_color(hpw::Logger_stream::info); \
  hpw::logger.set_stream(hpw::Logger_stream::info); \
  hpw::logger << "[Info] "; \
  hpw::logger << hpw::Logger::get_source_location(); \
  hpw::Logger::config.use_endl = true; \
} \
hpw::logger

#define log_error { \
  hpw::Logger::config.use_endl = false; \
  hpw::Logger::set_color(hpw::Logger_stream::error); \
  hpw::logger.set_stream(hpw::Logger_stream::error); \
  hpw::logger << "[Error] "; \
  hpw::logger << hpw::Logger::get_source_location(); \
  hpw::Logger::config.use_endl = true; \
} \
hpw::logger

#define log_debug { \
  hpw::Logger::config.use_endl = false; \
  hpw::Logger::set_color(hpw::Logger_stream::debug); \
  hpw::logger.set_stream(hpw::Logger_stream::debug); \
  hpw::logger << "[Debug] "; \
  hpw::logger << hpw::Logger::get_source_location(); \
  hpw::Logger::config.use_endl = true; \
} \
hpw::logger

#define log_warning { \
  hpw::Logger::config.use_endl = false; \
  hpw::Logger::set_color(hpw::Logger_stream::warning); \
  hpw::logger.set_stream(hpw::Logger_stream::warning); \
  hpw::logger << "[Warning] "; \
  hpw::logger << hpw::Logger::get_source_location(); \
  hpw::Logger::config.use_endl = true; \
} \
hpw::logger

int main() {
  log_info << "info text test";
  log_warning << "warning text test";
  log_error << "error text test";
  log_debug << "debug text test";
  
  hpw::reopen_log_file("delme.txt");
  log_info << "output to log file test";
}
