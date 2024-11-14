#pragma once
#include <source_location>
#include <string_view>

// для распределения логов по потокам
enum class Log_stream {
  null = 0, // вникуда
  info,
  warning,
  debug,
};

// настройки логгера
struct Log_config {
  bool to_stdout      : 1 {true}; // вывод в консоль
  bool to_stderr      : 1 {};     // вывод в поток ошибок в консоли
  bool to_file        : 1 {};     // вывод логов в файл
  //bool to_screen      : 1 {true}; // вывод на экран игры TODO
  bool stream_info    : 1 {true}; // канал обычной инфы
  bool stream_warning : 1 {true}; // канал предупреждений
  bool print_source   : 1 {true}; // показывать из какой строки кода и файла был вызва лог
  bool stream_debug   : 1 {};     // канал с более детальным выводом
}; // Log_config

Log_config& log_get_config() noexcept;
void log_set_config(const Log_config& cfg) noexcept;
void set_log_filename(const char* fname) noexcept;
const char* get_log_filename() noexcept;

// выводит лог в консоль или в файл
void hpw_log(
  const std::string_view msg,
  const Log_stream stream = Log_stream::info,
  const std::source_location location = std::source_location::current()
) noexcept;
