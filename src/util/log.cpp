#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include "log.hpp"
#include "util/str-util.hpp"
#include <windows.h>
#include <filesystem>
#include <iostream>
#include <chrono>
#include <format>
#include "util/platform.hpp"
#include "util/error.hpp"

namespace hpw {

Logger::~Logger() {
  // вернуть цвета как были
  set_color(Logger_stream::standard);
  close_log_file();
}

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
  return_if (Logger::config.stream == Logger_stream::info && !Logger::config.use_stream_info);
  return_if (Logger::config.stream == Logger_stream::warning && !Logger::config.use_stream_warning);
  return_if (Logger::config.stream == Logger_stream::debug && !Logger::config.use_stream_debug);
  return_if (Logger::config.stream == Logger_stream::error && !Logger::config.use_stream_error);

  std::lock_guard lock(Logger::mu);

  if (Logger::config.use_terminal) {
    if (Logger::config.stream == Logger_stream::error) {
      std::cerr << ss.str();
    } else {
      std::cout << ss.str();
      std::cout.flush();
    }
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

void reopen_log_file(cr<Str> fname) {
  iferror(fname.empty(), "log file name is empty");
  Logger::config.file.close();
  Str path;

  try {
    path = fname;
    conv_sep(path);
    path = std::filesystem::weakly_canonical(path).string();
    Logger::config.file.open(std::filesystem::path(path), std::ios_base::app);
  } catch (...) {
    std::cerr << "[Error] log file not created at \"" << fname << "\"\n";
    return;
  }
  
  return_if(Logger::config.file.bad());
  Logger::config.file << std::format("\n::::::::: start logging at {0:%H:%M %d.%m.%Y} :::::::::\n",
    std::chrono::system_clock::now());
  log_info << "log file opened at \"" << path << "\"";
}

void close_log_file() {
  return_if(!Logger::config.file);
  Logger::config.file << std::format("stop logging at {0:%H:%M %d.%m.%Y}\n",
    std::chrono::system_clock::now());
  Logger::config.file.close();
}

Logger::Logger_proxy::~Logger_proxy() {
  if (Logger::config.use_endl)
    _ss << std::endl;

  _master._print(_ss);
}

Logger::Logger_proxy::Logger_proxy(Logger& master): _master {master} {}
Logger::Logger_proxy::Logger_proxy(cr<Logger::Logger_proxy> other)
  : _master {other._master} { _ss << other._ss.str(); }
Logger::Logger_proxy::Logger_proxy(Logger::Logger_proxy&& other)
  : _master {other._master}, _ss {std::move(other._ss)} {}

} // npw ns

hpw::Logger& config_log_to_info(cr<std::source_location> sl) {
  cauto tmp = hpw::Logger::config.use_endl;
  hpw::Logger::config.use_endl = false;
  hpw::Logger::set_color(hpw::Logger_stream::info);
  hpw::logger.set_stream(hpw::Logger_stream::info);
  hpw::logger << "Info    | ";
  hpw::logger << hpw::Logger::get_source_location(sl);
  hpw::Logger::config.use_endl = tmp;
  return hpw::logger;
}
  
hpw::Logger& config_log_to_error(cr<std::source_location> sl) {
  cauto tmp = hpw::Logger::config.use_endl;
  hpw::Logger::config.use_endl = false;
  hpw::Logger::set_color(hpw::Logger_stream::error);
  hpw::logger.set_stream(hpw::Logger_stream::error);
  hpw::logger << "Error   | ";
  hpw::logger << hpw::Logger::get_source_location(sl);
  hpw::Logger::config.use_endl = tmp;
  return hpw::logger;
}

hpw::Logger& config_log_to_debug(cr<std::source_location> sl) {
  cauto tmp = hpw::Logger::config.use_endl;
  hpw::Logger::config.use_endl = false;
  hpw::Logger::set_color(hpw::Logger_stream::debug);
  hpw::logger.set_stream(hpw::Logger_stream::debug);
  hpw::logger << "Debug   | ";
  hpw::logger << hpw::Logger::get_source_location(sl);
  hpw::Logger::config.use_endl = tmp;
  return hpw::logger;
}
  
hpw::Logger& config_log_to_warning(cr<std::source_location> sl) {
  cauto tmp = hpw::Logger::config.use_endl;
  hpw::Logger::config.use_endl = false;
  hpw::Logger::set_color(hpw::Logger_stream::warning);
  hpw::logger.set_stream(hpw::Logger_stream::warning);
  hpw::logger << "Warning | ";
  hpw::logger << hpw::Logger::get_source_location(sl);
  hpw::Logger::config.use_endl = tmp;
  return hpw::logger;
}
  
hpw::Logger& config_log_to_none() {
  hpw::logger.set_stream(hpw::Logger_stream::null);
  return hpw::logger;
}
