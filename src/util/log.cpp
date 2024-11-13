#include "log.hpp"
#include <syncstream>
#include <iostream>
#include "str.hpp"
#include "macro.hpp"

namespace { Log_config g_config {}; }

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
  Str source;
  if (::g_config.print_source)
    source = location.file_name() << ':' << location.line() << ": ";
  
  // определить в какой поток выводить
  if (::g_config.to_stdout) {
    std::osyncstream synced_out(std::cout);
    synced_out << source << msg;
  }

  if (::g_config.to_stderr) {
    std::osyncstream synced_out(std::cerr);
    synced_out << source << msg;
  }

  if (::g_config.to_file) {
    // TODO
  }
}
