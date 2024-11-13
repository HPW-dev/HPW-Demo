#include "log.hpp"
#include <cassert>
#include <syncstream>
#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <format>
#include "macro.hpp"

namespace { 
  Log_config g_config {};
  std::string g_log_fname {"log.txt"};
  std::ofstream g_log_file {};
  std::recursive_mutex g_log_file_mu {};
}

Log_config& log_get_config() noexcept { return ::g_config; }

void log_set_config(cr<Log_config> cfg) noexcept { ::g_config = cfg; }

static void make_log_file(cr<std::string> fname) {
  std::lock_guard lock(g_log_file_mu);

  try {
    ::g_log_file.open(fname);
    
    if (::g_log_file.is_open())
      std::cout << "log file \"" << fname << "\" created\n";
    else
      throw;
  } catch (...) {
    std::cerr << "error while creating log file (\"" << fname << "\")\n";
  }
}

void hpw_log(const std::string_view msg, const Log_stream stream,
const std::source_location location) noexcept {
  // фильтрация потоков
  const bool DEBUG_ENABLED = stream == Log_stream::debug && ::g_config.stream_debug;
  const bool INFO_ENABLED = stream == Log_stream::info && ::g_config.stream_info;
  const bool WARNING_ENABLED = stream == Log_stream::warning && ::g_config.stream_warning;
  const bool LOG_ENABLED = DEBUG_ENABLED || INFO_ENABLED || WARNING_ENABLED;
  return_if (!LOG_ENABLED);
  
  // добавить инфу о источнике
  std::string source;
  if (::g_config.print_source)
    source = std::format("{}:{}: ", location.file_name(), location.line());
  
  // определить в какой поток выводить:
  // cout
  if (::g_config.to_stdout) {
    std::osyncstream synced_out(std::cout);
    synced_out << source << msg;
  }

  // cerr
  if (::g_config.to_stderr) {
    std::osyncstream synced_out(std::cerr);
    synced_out << source << msg;
  }

  // в файл
  if (::g_config.to_file) {
    std::lock_guard lock(g_log_file_mu);

    if (g_log_file.is_open())
      make_log_file(::g_log_fname);

    if (g_log_file.is_open())
      ::g_log_file << source << msg;
  }
}

void set_log_filename(const char* fname) noexcept {
  assert(fname);
  ::g_log_fname = fname;
  make_log_file(::g_log_fname);
}

const char* get_log_filename() noexcept { return g_log_fname.c_str(); }
