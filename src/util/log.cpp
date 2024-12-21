#include "log.hpp"
#include <cassert>
#include <syncstream>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <string>
#include <mutex>
#include <format>
#include "macro.hpp"
#include "error.hpp"
#include "str-util.hpp"

namespace { 
  Log_config g_config {};
  std::string g_log_fname {};
  std::ofstream g_log_file {};
  std::recursive_mutex g_log_file_mu {};
}

Log_config& log_get_config() noexcept { return ::g_config; }

void log_set_config(cr<Log_config> cfg) noexcept { ::g_config = cfg; }

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
    if (g_log_file.is_open()) {
      ::g_log_file << source << msg;
      ::g_log_file.flush();
    }
  }
}

void log_open_file(const char* fname) noexcept {
  try {
    iferror(fname == nullptr || Str(fname).empty(), "fname is empty");

    std::lock_guard lock(g_log_file_mu);
    cauto old_name = ::g_log_fname;
    ::g_log_fname = fname;
    conv_sep(::g_log_fname);
    ::g_log_fname = std::filesystem::weakly_canonical(::g_log_fname).string();

    if (old_name != ::g_log_fname) {
      ::g_log_file.open(std::filesystem::path(::g_log_fname));
        
      if (::g_log_file.is_open())
        std::cout << "log file \"" << ::g_log_fname << "\" created\n";
      else
        error("log file is not opened");
    }
  } catch (cr<hpw::Error> err) {
    std::cerr << "error while creating log file (\"" << ::g_log_fname << "\":)\n" <<
      err.what() << '\n' <<
      "Output to log file disabled\n";
    g_config.to_file = false;
  } catch (...) {
    std::cerr << "error while creating log file (\"" << ::g_log_fname << "\")\n" <<
      "Output to log file disabled\n";
    g_config.to_file = false;
  }
}
